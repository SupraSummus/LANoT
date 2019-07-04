#include <errno.h>
#include <unistd.h>

#include "app_usbd_core.h"
#include "app_usbd.h"
#include "app_usbd_cdc_acm.h"
#include "nrf_atomic.h"

#define READ_SIZE 1

static char m_rx_buffer[READ_SIZE];

static nrf_atomic_flag_t tx_done;

static void cdc_acm_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_cdc_acm_user_event_t event)
{
    app_usbd_cdc_acm_t const * p_app_cdc_acm = app_usbd_cdc_acm_class_get(p_inst);

    switch (event) {
        case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
        {
            /*Setup first transfer*/
            app_usbd_cdc_acm_read(
				p_app_cdc_acm,
				m_rx_buffer,
				READ_SIZE
			);

            nrf_atomic_flag_clear(&tx_done);

            break;
        }

        case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
        {
            ret_code_t ret;
            do
            {
                /* Fetch data until internal buffer is empty */
                ret = app_usbd_cdc_acm_read(p_app_cdc_acm,
                                            m_rx_buffer,
                                            READ_SIZE);
            } while (ret == NRF_SUCCESS);
            break;
        }

		case APP_USBD_CDC_ACM_USER_EVT_TX_DONE: {
			nrf_atomic_flag_clear(&tx_done);
			break;
		}

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
    APP_USBD_CDC_COMM_PROTOCOL_AT_V250
);

int _write(int file, char * buf, int nbytes) {
    /* We only handle stdout */
    if (file != STDOUT_FILENO) {
        errno = EBADF;
        return -1;
    }

	//while (!nrf_atomic_flag_set_fetch(&tx_done)) ;  // active waiting ;(
	//app_usbd_cdc_acm_write(&m_app_cdc_acm, buf, nbytes);
	return nbytes;
}

void usb_io_init(void) {
	ret_code_t err_code;

	

	// USB CDC
    static const app_usbd_config_t m_usbd_config = {0};
    err_code = app_usbd_init(&m_usbd_config);
    APP_ERROR_CHECK(err_code);

    app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&m_app_cdc_acm);
    err_code = app_usbd_class_append(class_cdc_acm);
    APP_ERROR_CHECK(err_code);

    app_usbd_enable();
    app_usbd_start();
}
