#include <lanot/log_backend.h>

#define NANOPRINTF_IMPLEMENTATION
//#define NANOPRINTF_VISIBILITY_STATIC
#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0
#include <nanoprintf.h>

#define printf_buffer_size (1024)
static char printf_buffer[printf_buffer_size];

int printf(const char * format, ...) {
        va_list args;
        va_start(args, format);
        int len = npf_vsnprintf(printf_buffer, printf_buffer_size, format, args);
        va_end(args);
        if (len > printf_buffer_size) {
                log_backend_write(printf_buffer, printf_buffer_size);
                log_backend_write("printf buffer overrun\n", 22);
        } else {
                log_backend_write(printf_buffer, len);
        }
        return len;
}
