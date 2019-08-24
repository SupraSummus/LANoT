#include <stdlib.h>

#include "lanot/bt.h"

void board_startup_hook(void) {
	// start bt
	lanot_bt_init();

	abort();  // just for testing
}
