#include <os4cm4/printk.h>

void abort (void) {
        while (1);
}

void __assert_func (
        const char *file,
        int line,
        const char *func,
        const char *failedexpr
) {
        printk(
                "assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
                failedexpr, file, line,
                func ? ", function: " : "", func ? func : ""
        );
        abort();
}
