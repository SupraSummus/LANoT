#ifndef IO_HELPERS_H
#define IO_HELPERS_H

static inline ssize_t read_all(int fd, void * buf, size_t buf_size) {
    size_t received = 0;
    while (buf_size > 0) {
        ssize_t r = read(fd, buf, buf_size);
        if (r < 0) return r;
        if (r == 0) break;
        assert(r <= buf_size);  // paranoid check
        received += r;
        buf += r;
        buf_size -= r;
    }
    return received;
}

static inline ssize_t write_all(int fd, void const * buf, size_t buf_size) {
    size_t sent = 0;
    while (buf_size > 0) {
        ssize_t r = write(fd, buf, buf_size);
        if (r < 0) return r;
        if (r == 0) break;
        assert(r <= buf_size);  // paranoid check
        sent += r;
        buf += r;
        buf_size -= r;
    }
    return sent;
}

static inline bool write_uint8(int fd, uint8_t v) {
    return write_all(fd, &v, sizeof(uint8_t)) == sizeof(uint8_t);
}

static inline bool read_uint32(int fd, uint32_t * v) {
    if (read_all(fd, v, sizeof(uint32_t)) != sizeof(uint32_t)) return false;
    //*v = ntohl(*v);  // network order to host
    return true;
}

#endif
