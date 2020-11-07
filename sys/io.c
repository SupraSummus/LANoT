#include <assert.h>
#include <errno.h>
#include <unistd.h>

#include "io.h"

#define LOG_SUBSYSTEM "io"
#include <lanot/log.h>

struct io_endpoint_t {
	io_handler_t * read_handler;
	void * read_handler_private_data;

	io_handler_t * write_handler;
	void * write_handler_private_data;

	io_use_synchronous_mode_handler_t * use_synchronous_mode_handler;
	void * use_synchronous_mode_handler_private_data;
};

static int log_write_handler (void * buf, size_t len, void * p) {
	(void)p;
	return log_write(buf, len);
}

static int log_use_synchronous_mode_handler (void * p) {
	(void)p;
	int ret = 0;

	if (log_use_synchronous_mode()) {
		ret = 0;
	} else {
		errno = EPERM;
		ret = -1;
	}

	return ret;
}

static struct io_endpoint_t endpoints[IO_FD_COUNT] = {
	{},
	{},
	{
		.write_handler = log_write_handler,
		.use_synchronous_mode_handler = log_use_synchronous_mode_handler
	}
};

inline static struct io_endpoint_t * get_endpoint(int fd) {
	if (fd < 0 || fd >= IO_FD_COUNT) return NULL;  // fd out of range
	return &endpoints[fd];
}

int _read(int fd, char * buf, int len) {
	struct io_endpoint_t * endpoint = get_endpoint(fd);

	//  fd out of range     no handler registered
	if (endpoint == NULL || endpoint->read_handler == NULL) {
		errno = EBADF;
		return  -1;
	}

	return endpoint->read_handler(buf, len, endpoint->read_handler_private_data);
}

int _write(int fd, char * buf, int len) {
	struct io_endpoint_t * endpoint = get_endpoint(fd);

	//  fd out of range     no handler registered
	if (endpoint == NULL || endpoint->write_handler == NULL) {
		errno = EBADF;
		return  -1;
	}

	return endpoint->write_handler(buf, len, endpoint->write_handler_private_data);
}

int io_use_synchronous_mode(int fd) {
	struct io_endpoint_t * endpoint = get_endpoint(fd);

	// fd out of range
	if (endpoint == NULL) {
		errno = EBADF;
		return  -1;
	}

	// no handler registered
	if (endpoint->use_synchronous_mode_handler == NULL) {
		errno = ENOTSUP;
		return -1;
	}

	return endpoint->use_synchronous_mode_handler(endpoint->use_synchronous_mode_handler_private_data);
}

void io_register_read_handler (int fd, io_handler_t * handler, void * priv) {
	INFO(
		"io_register_read_handler fd=%d handler=%p priv=%p",
		fd, handler, priv
	);
	assert(handler != NULL);
	struct io_endpoint_t * ep = get_endpoint(fd);
	assert(ep != NULL);
	assert(ep->read_handler == NULL);
	ep->read_handler = handler;
	ep->read_handler_private_data = priv;
}

void io_register_write_handler (int fd, io_handler_t * handler, void * priv) {
	INFO(
		"io_register_write_handler fd=%d handler=%p priv=%p",
		fd, handler, priv
	);
	assert(handler != NULL);
	struct io_endpoint_t * ep = get_endpoint(fd);
	assert(ep != NULL);
	assert(ep->write_handler == NULL);
	ep->write_handler = handler;
	ep->write_handler_private_data = priv;
}

void io_register_use_synchronous_mode_handler (int fd, io_use_synchronous_mode_handler_t * handler, void * priv) {
	INFO(
		"io_register_use_synchronous_mode_handler fd=%d handler=%p priv=%p",
		fd, handler, priv
	);
	assert(handler != NULL);
	struct io_endpoint_t * ep = get_endpoint(fd);
	assert(ep != NULL);
	assert(ep->use_synchronous_mode_handler == NULL);
	ep->use_synchronous_mode_handler = handler;
	ep->use_synchronous_mode_handler_private_data = priv;
}
