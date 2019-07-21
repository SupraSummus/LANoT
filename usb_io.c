#include <errno.h>
#include <unistd.h>

#include "app_usbd_core.h"
#include "app_usbd.h"
#include "app_usbd_cdc_acm.h"
#include "nrf_drv_clock.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"

#define USB_IO_ACTIVE (0x01)
static EventGroupHandle_t usb_io_status;

static SemaphoreHandle_t _read_mutex;
static SemaphoreHandle_t rx_done;

static SemaphoreHandle_t _write_mutex;
static SemaphoreHandle_t tx_done;

static void cdc_acm_user_ev_handler(
	app_usbd_class_inst_t const * p_inst,
	app_usbd_cdc_acm_user_event_t event
) {
	switch (event) {
		case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN: {
			// allow transfer
			xEventGroupSetBitsFromISR(usb_io_status, USB_IO_ACTIVE, NULL);
			break;
		}

		case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE: {
			// disallow transfer
			xEventGroupClearBitsFromISR(usb_io_status, USB_IO_ACTIVE);
			break;
		}

		case APP_USBD_CDC_ACM_USER_EVT_RX_DONE: {
			xSemaphoreGiveFromISR(rx_done, NULL);
			break;
		}

		case APP_USBD_CDC_ACM_USER_EVT_TX_DONE: {
			xSemaphoreGiveFromISR(tx_done, NULL);
			break;
		}

		default: {
			break;
		}
	}
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

APP_USBD_CDC_ACM_GLOBAL_DEF(
	m_app_cdc_acm,
	cdc_acm_user_ev_handler,
	0,  // CDC_ACM_COMM_INTERFACE
	1,  // CDC_ACM_DATA_INTERFACE
	NRF_DRV_USBD_EPIN2,  // CDC_ACM_COMM_EPIN
	NRF_DRV_USBD_EPIN1,  // CDC_ACM_DATA_EPIN
	NRF_DRV_USBD_EPOUT1,  // CDC_ACM_DATA_EPOUT
	APP_USBD_CDC_COMM_PROTOCOL_NONE
);

int _read (int file, char *ptr, int len) {
	if (file != STDIN_FILENO) {
		errno = EBADF;
		return  -1;
	}

	size_t local_rx_size = 0;
	ret_code_t ret;

	xEventGroupWaitBits(
		usb_io_status, USB_IO_ACTIVE,
		pdFALSE, pdFALSE,
		portMAX_DELAY
	);

	xSemaphoreTake(_read_mutex, portMAX_DELAY);

		ret = app_usbd_cdc_acm_read_any(
			&m_app_cdc_acm,
			ptr,
			len
		);

		if (ret == NRF_SUCCESS) {
			// pass - data was already available

		} else if (ret == NRF_ERROR_IO_PENDING) {
			// wait for rx done
			xSemaphoreTake(rx_done, portMAX_DELAY);

		} else {
			APP_ERROR_CHECK(ret);
		}

		local_rx_size = app_usbd_cdc_acm_rx_size(&m_app_cdc_acm);

	xSemaphoreGive(_read_mutex);

	return local_rx_size;
}

int _write(int file, char * buf, int nbytes) {
	/* We only handle stdout */
	if (file != STDOUT_FILENO) {
		errno = EBADF;
		return -1;
	}

	ret_code_t ret;

	xEventGroupWaitBits(
		usb_io_status, USB_IO_ACTIVE,
		pdFALSE, pdFALSE,
		portMAX_DELAY
	);

	xSemaphoreTake(_write_mutex, portMAX_DELAY);

		// do tx
		ret = app_usbd_cdc_acm_write(&m_app_cdc_acm, buf, nbytes);
		APP_ERROR_CHECK(ret);

		// wait for tx done
		xSemaphoreTake(tx_done, portMAX_DELAY);

	xSemaphoreGive(_write_mutex);

	return nbytes;
}

void usb_io_init(void) {
	ret_code_t ret;

	usb_io_status = xEventGroupCreate();
	_read_mutex = xSemaphoreCreateMutex();
	rx_done = xSemaphoreCreateBinary();
	_write_mutex = xSemaphoreCreateMutex();
	tx_done = xSemaphoreCreateBinary();

	// init usbd
	static const app_usbd_config_t m_usbd_config = {
		.ev_state_proc = usbd_user_ev_handler
	};
	ret = app_usbd_init(&m_usbd_config);
	APP_ERROR_CHECK(ret);

	// init cdc acm class
	app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&m_app_cdc_acm);
	ret = app_usbd_class_append(class_cdc_acm);
	APP_ERROR_CHECK(ret);

	ret = app_usbd_power_events_enable();
	APP_ERROR_CHECK(ret);
}
