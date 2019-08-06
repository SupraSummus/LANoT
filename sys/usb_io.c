#include <errno.h>
#include <unistd.h>

#include "app_usbd_core.h"
#include "app_usbd.h"
#include "app_usbd_cdc_acm.h"
#include "nrf_drv_clock.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"

#include "io.h"
#include "log.h"

static void cdc_acm_user_ev_handler(
	app_usbd_class_inst_t const * p_inst,
	app_usbd_cdc_acm_user_event_t event
);
static int usb_io_read(void * buf, size_t len, void *);
static int usb_io_write(void * buf, size_t len, void *);

#define CLASSES_COUNT (2)

APP_USBD_CDC_ACM_GLOBAL_DEF(
	m_app_cdc_acm_0,
	cdc_acm_user_ev_handler,
	0,  // CDC_ACM_COMM_INTERFACE
	1,  // CDC_ACM_DATA_INTERFACE
	NRF_DRV_USBD_EPIN2,  // CDC_ACM_COMM_EPIN
	NRF_DRV_USBD_EPIN1,  // CDC_ACM_DATA_EPIN
	NRF_DRV_USBD_EPOUT1,  // CDC_ACM_DATA_EPOUT
	APP_USBD_CDC_COMM_PROTOCOL_NONE
);

APP_USBD_CDC_ACM_GLOBAL_DEF(
	m_app_cdc_acm_1,
	cdc_acm_user_ev_handler,
	2,  // CDC_ACM_COMM_INTERFACE
	3,  // CDC_ACM_DATA_INTERFACE
	NRF_DRV_USBD_EPIN4,  // CDC_ACM_COMM_EPIN
	NRF_DRV_USBD_EPIN3,  // CDC_ACM_DATA_EPIN
	NRF_DRV_USBD_EPOUT2,  // CDC_ACM_DATA_EPOUT
	APP_USBD_CDC_COMM_PROTOCOL_NONE
);

static int usb_io_class_index(app_usbd_cdc_acm_t const * const class) {
	if (class == &m_app_cdc_acm_0) return 0;
	if (class == &m_app_cdc_acm_1) return 1;
	return -1;
}

static app_usbd_cdc_acm_t const * usb_io_class_get(int i) {
	switch (i) {
		case 0: return &m_app_cdc_acm_0;
		case 1: return &m_app_cdc_acm_1;
		default: return NULL;
	}
}

struct usb_io_status_t {
	#define USB_IO_ACTIVE (0x01)
	StaticEventGroup_t _usb_io_status;
	EventGroupHandle_t usb_io_status;

	SemaphoreHandle_t read_mutex;
	StaticSemaphore_t _read_mutex;
	SemaphoreHandle_t rx_done;
	StaticSemaphore_t _rx_done;

	SemaphoreHandle_t write_mutex;
	StaticSemaphore_t _write_mutex;
	SemaphoreHandle_t tx_done;
	StaticSemaphore_t _tx_done;
};

struct usb_io_status_t statuses[CLASSES_COUNT];

// cdc acm class index -> read fd to register or -1
static const int read_classes[CLASSES_COUNT] = {
	STDIN_FILENO,
	3  // in case someone wants to read things written to logging instance
};

// cdc acm class index -> write fd to register or -1
static const int write_classes[CLASSES_COUNT] = {
	STDOUT_FILENO,
	3
};

/**
 * now follows generic stuff 
 * =========================
 **/

static struct usb_io_status_t * usb_io_status_get(int i) {
	if (i < 0 || i >= CLASSES_COUNT) return NULL;
	return statuses + i;
}

static void usb_io_class_init(int i) {
	// init status struct
	struct usb_io_status_t * status = usb_io_status_get(i);
	assert(status != NULL);

	status->usb_io_status = xEventGroupCreateStatic(&status->_usb_io_status);
	status->read_mutex = xSemaphoreCreateMutexStatic(&status->_read_mutex);
	status->rx_done = xSemaphoreCreateBinaryStatic(&status->_rx_done);
	status->write_mutex = xSemaphoreCreateMutexStatic(&status->_write_mutex);
	status->tx_done = xSemaphoreCreateBinaryStatic(&status->_tx_done);

	assert(status->usb_io_status != NULL);
	assert(status->read_mutex != NULL);
	assert(status->rx_done != NULL);
	assert(status->write_mutex != NULL);
	assert(status->tx_done != NULL);

	// attach class to driver
	app_usbd_cdc_acm_t const * const class = usb_io_class_get(i);
	assert(class != NULL);
	app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(class);
	ret_code_t ret = app_usbd_class_append(class_cdc_acm);
	APP_ERROR_CHECK(ret);

	// register class under read and write fds
	io_register(read_classes[i], io_direction_reader, usb_io_read, (void*)i);
	io_register(write_classes[i], io_direction_writer, usb_io_write, (void*)i);
}

static void cdc_acm_user_ev_handler(
	app_usbd_class_inst_t const * p_inst,
	app_usbd_cdc_acm_user_event_t event
) {
	app_usbd_cdc_acm_t const * class = app_usbd_cdc_acm_class_get(p_inst);
	int i = usb_io_class_index(class);
	struct usb_io_status_t * status = usb_io_status_get(i);
	assert(status != NULL);
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	BaseType_t ret;

	switch (event) {
		case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN: {
			// allow transfer
			ret = xEventGroupSetBitsFromISR(status->usb_io_status, USB_IO_ACTIVE, &xHigherPriorityTaskWoken);
			assert(ret == pdPASS);
			break;
		}

		case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE: {
			// disallow transfer
			xEventGroupClearBitsFromISR(status->usb_io_status, USB_IO_ACTIVE);
			break;
		}

		case APP_USBD_CDC_ACM_USER_EVT_RX_DONE: {
			xSemaphoreGiveFromISR(status->rx_done, &xHigherPriorityTaskWoken);
			INFO("random log from inside ISR (woken: %s)", xHigherPriorityTaskWoken ? "yes" : "no");
			break;
		}

		case APP_USBD_CDC_ACM_USER_EVT_TX_DONE: {
			xSemaphoreGiveFromISR(status->tx_done, &xHigherPriorityTaskWoken);
			break;
		}

		default: {
			break;
		}
	}

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

static void usbd_user_ev_handler(app_usbd_event_type_t event) {
	switch (event) {
		case APP_USBD_EVT_DRV_SUSPEND:
			break;

		case APP_USBD_EVT_DRV_RESUME:
			break;

		case APP_USBD_EVT_STARTED:
			break;

		case APP_USBD_EVT_STOPPED:
			app_usbd_disable();
			break;

		case APP_USBD_EVT_POWER_DETECTED:
			if (!nrf_drv_usbd_is_enabled()) {
				app_usbd_enable();
			}
			break;

		case APP_USBD_EVT_POWER_REMOVED:
			app_usbd_stop();
			break;

		case APP_USBD_EVT_POWER_READY:
			app_usbd_start();
			break;

		default:
			break;
	}
}

int usb_io_read (void *ptr, size_t len, void * priv) {
	int class_index = (int)priv;
	app_usbd_cdc_acm_t const * const class = usb_io_class_get(class_index);
	assert(class != NULL);
	struct usb_io_status_t * status = usb_io_status_get(class_index);
	assert(status != NULL);

	size_t local_rx_size = 0;
	ret_code_t ret;

	xEventGroupWaitBits(
		status->usb_io_status, USB_IO_ACTIVE,
		pdFALSE, pdFALSE,
		portMAX_DELAY
	);

	xSemaphoreTake(status->read_mutex, portMAX_DELAY);

		ret = app_usbd_cdc_acm_read_any(class, ptr, len);

		if (ret == NRF_SUCCESS) {
			// pass - data was already available

		} else if (ret == NRF_ERROR_IO_PENDING) {
			// wait for rx done
			xSemaphoreTake(status->rx_done, portMAX_DELAY);

		} else {
			APP_ERROR_CHECK(ret);
		}

		local_rx_size = app_usbd_cdc_acm_rx_size(class);

	xSemaphoreGive(status->read_mutex);

	return local_rx_size;
}

int usb_io_write(void * buf, size_t nbytes, void * priv) {
	int class_index = (int)priv;
	app_usbd_cdc_acm_t const * const class = usb_io_class_get(class_index);
	assert(class != NULL);
	struct usb_io_status_t * status = usb_io_status_get(class_index);
	assert(status != NULL);

	ret_code_t ret;

	xEventGroupWaitBits(
		status->usb_io_status, USB_IO_ACTIVE,
		pdFALSE, pdFALSE,
		portMAX_DELAY
	);

	xSemaphoreTake(status->write_mutex, portMAX_DELAY);

		// do tx
		ret = app_usbd_cdc_acm_write(class, buf, nbytes);
		APP_ERROR_CHECK(ret);

		// wait for tx done
		xSemaphoreTake(status->tx_done, portMAX_DELAY);

	xSemaphoreGive(status->write_mutex);

	return nbytes;
}

void usb_io_init(void) {
	ret_code_t ret;

	INFO("initializing usb");

	// init usbd
	static const app_usbd_config_t m_usbd_config = {
		.ev_state_proc = usbd_user_ev_handler
	};
	ret = app_usbd_init(&m_usbd_config);
	APP_ERROR_CHECK(ret);

	// attach cdc acm classes
	for (int i = 0; i < CLASSES_COUNT; i ++) {
		usb_io_class_init(i);
	}

	// enable
	ret = app_usbd_power_events_enable();
	APP_ERROR_CHECK(ret);
}
