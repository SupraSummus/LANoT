struct queue_t {
        uint32_t start;
        uint32_t length;
        uint32_t max_length;
        struct thread_t * waiting;
        uint32_t elements[];
};

static inline uint32_t queue_write_position (struct queue_t * queue) {
        return (queue->start + queue->length) % queue->max_length;
}

void queue_put (struct queue_t * queue, uint32_t v) {
        critical_start();
                assert(queue->length < queue->max_length);
                queue->elements[queue_write_position(queue)] = v;
                queue->length ++;
        critical_end();
}

uint32_t queue_get (struct queue_t * queue) {
        critical_start();

}
