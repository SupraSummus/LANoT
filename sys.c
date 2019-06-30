#include <unistd.h>

#include "FreeRTOS.h"
#include "task.h"

extern void user_main(void);

int main(void)
{
    user_main();

    vTaskStartScheduler();

	/* Should not reach here as the scheduler is already started. */
	for( ; ; )
	{
	}
}
