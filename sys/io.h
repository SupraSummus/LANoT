#ifndef IO_H
#define IO_H

#define IO_FD_COUNT (4)

typedef int (io_handler_t) (void * buf, size_t len, void * priv);

typedef enum _io_direction_t {
	io_direction_reader,
	io_direction_writer,
} io_direction_t;

extern void io_register(int fd, io_direction_t dir, io_handler_t * fn, void * priv);

#endif
