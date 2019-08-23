#include "nimble/nimble_port.h"
#include "transport/ram/ble_hci_ram.h"

#define LOG_SUBSYSTEM "bt"
#include "log.h"

void lanot_bt_init() {
	INFO("starting BT");
	ble_hci_ram_init();
	nimble_port_init();
}
