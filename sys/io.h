#ifndef IO_H
#define IO_H

#define IO_FD_COUNT (4)

// io_handler_t is both for read and write operations
typedef int (io_handler_t) (void * buf, size_t len, void * priv);
typedef int (io_use_synchronous_mode_handler_t) (void * priv);

extern void io_register_read_handler(int fd, io_handler_t * fn, void * priv);
extern void io_register_write_handler(int fd, io_handler_t * fn, void * priv);
extern void io_register_use_synchronous_mode_handler(int fd, io_use_synchronous_mode_handler_t * fn, void * priv);

/**
 * Switch FD to synchronous mode for write requests. Synchronous mode
 *  * wont use interrupts and
 *  * will fully block (no return until request is completed).
 *
 * Once enabled synchronous mode cannot be disabled. Also, it can be enabled only once per FD - subsequent calls will return errors.
 */
extern int io_use_synchronous_mode(int fd);

#endif
