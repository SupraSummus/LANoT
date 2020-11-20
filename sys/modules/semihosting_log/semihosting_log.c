#include <lanot/log_backend.h>

static void send_command (int command, void * message) {
	__asm volatile(
		"mov r0, %[cmd];"
		"mov r1, %[msg];"
		"bkpt #0xAB"
		:
		: [cmd] "r" (command), [msg] "r" (message)
		: "r0", "r1", "memory"
	);
}

ssize_t log_backend_write (const char * buf, size_t len) {
        uint32_t message[] = {
                1,  //stdout
                (uint32_t)buf,
                len //size of string
        };

        //Send semihosting command
        send_command(0x05, message);

        return len;
}

void log_backend_use_synchronous_mode (void) {
	log_backend_write("lb sync\n", 8);
}

void semihosting_end (void) {
	uint32_t message[] = {
		0  // exit code
	};
	send_command(60, message);
}
