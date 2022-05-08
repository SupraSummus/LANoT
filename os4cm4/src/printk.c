#include <os4cm4/log_backend.h>
#include <os4cm4/printk.h>

#define NANOPRINTF_IMPLEMENTATION
//#define NANOPRINTF_VISIBILITY_STATIC
#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0
#include <nanoprintf.h>

int printk (const char * format, ...) {
        va_list args;
        va_start(args, format);
        int len = npf_vsnprintf(printk_buffer, PRINTK_BUFFER_SIZE, format, args);
        va_end(args);
        if (len > PRINTK_BUFFER_SIZE) {
                log_backend_write(printk_buffer, PRINTK_BUFFER_SIZE);
                log_backend_write("printk buffer overrun\n", 22);
        } else {
                log_backend_write(printk_buffer, len);
        }
        return len;
}

// https://stackoverflow.com/a/7776146
void hex_dump (const char * desc, const void * addr, const int len) {
    int i;
    unsigned char buff[17];
    const unsigned char * pc = (const unsigned char *)addr;

    // Output description if given.

    if (desc != NULL)
        printk ("%s (%p):\n", desc, addr);

    // Length checks.

    if (len == 0) {
        printk("  ZERO LENGTH\n");
        return;
    }
    else if (len < 0) {
        printk("  NEGATIVE LENGTH: %d\n", len);
        return;
    }

    // Process every byte in the data.

    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Don't print ASCII buffer for the "zeroth" line.

            if (i != 0)
                printk ("  %s\n", buff);

            // Output the offset.

            printk ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printk (" %02x", pc[i]);

        // And buffer a printable ASCII character for later.

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.

    while ((i % 16) != 0) {
        printk ("   ");
        i++;
    }

    // And print the final ASCII buffer.

    printk ("  %s\n", buff);
}
