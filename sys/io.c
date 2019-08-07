#include <assert.h>
#include <errno.h>
#include <unistd.h>

#include "io.h"
#include "log.h"

struct io_endpoint_t {
	io_handler_t * handler;
	void * priv;
};

static struct io_endpoint_t endpoints[IO_FD_COUNT * 2];

inline static struct io_endpoint_t * get_endpoint(int fd, io_direction_t direction) {
	int ep_index = -1;
	if (fd < 0 || fd >= IO_FD_COUNT) return NULL;  // fd out of range
	if (direction == io_direction_reader) ep_index = fd * 2;
	if (direction == io_direction_writer) ep_index = fd * 2 + 1;
	if (ep_index == -1) return NULL; // unknown direction
	return &endpoints[ep_index];
}

inline static int io_call (int fd, io_direction_t dir, char *ptr, int len) {
	struct io_endpoint_t * endpoint = get_endpoint(fd, dir);

	//  fd out of range     no handler registered
	if (endpoint == NULL || endpoint->handler == NULL) {
		errno = EBADF;
		return  -1;
	}

	return endpoint->handler(ptr, len, endpoint->priv);
}

int _read(int fd, char * buf, int len) {
	return io_call(fd, io_direction_reader, buf, len);
}

int _write(int fd, char * buf, int len) {
	return io_call(fd, io_direction_writer, buf, len);
}

void io_register (int fd, io_direction_t dir, io_handler_t * handler, void * priv) {
	INFO(
		"io_register fd=%d dir=%d handler=%p data=%p",
		fd, dir, handler, priv
	);
	assert(handler != NULL);
	struct io_endpoint_t * ep = get_endpoint(fd, dir);
	assert(ep != NULL);
	assert(ep->handler == NULL);
	ep->handler = handler;
	ep->priv = priv;
}
