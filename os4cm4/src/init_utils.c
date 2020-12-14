#include <stdbool.h>
#include <os4cm4/cap.h>
#include <stddef.h>
#include <assert.h>

void * max_ptr(void * a, void * b) {
        if (a > b) return a;
        else return b;
}

void * min_ptr(void * a, void * b) {
        if (a < b) return a;
        else return b;
}

bool is_intersected(
        void * ptr_a, size_t size_a,
        void * ptr_b, size_t size_b,
        void * * intersected_ptr, size_t * intersected_size
) {
        *intersected_ptr = max_ptr(ptr_a, ptr_b);
        void * intersected_end = min_ptr(ptr_a + size_a, ptr_b + size_b);
        *intersected_size = intersected_end - *intersected_ptr;
        return *intersected_ptr < intersected_end;
}

struct cap_t * mem_find_intersecting (
        struct cap_t * caps_start, struct cap_t * caps_past_end,
        void * ptr, size_t size,
        void * * intersected_ptr, size_t * intersected_size
) {
        for (struct cap_t * cap = caps_start; cap < caps_past_end; cap ++) {
                if (cap->type != CAP_TYPE_MEM) continue;
                if (is_intersected(
                        cap_ptr_get(cap), 1U << cap->size_shift,
                        ptr, size,
                        intersected_ptr, intersected_size
                )) return cap;
        }
        return NULL;
}

struct cap_t * cap_find_free (
        struct cap_t * caps_start, struct cap_t * caps_past_end
) {
        for (struct cap_t * cap = caps_start; cap < caps_past_end; cap ++) {
                if (cap->type == CAP_TYPE_EMPTY) return cap;
        }
        return NULL;
}

void mem_remove(
        struct cap_t * caps_start, struct cap_t * caps_past_end,
        void * ptr, size_t size
) {
        debug_printk("mem_remove %p - %p\n", ptr, ptr + size);

        while (true) {
                // find intersecting region
                void * intersected_ptr;
                size_t intersected_size;
                struct cap_t * cap = mem_find_intersecting(
                        caps_start, caps_past_end,
                        ptr, size,
                        &intersected_ptr, &intersected_size
                );

                // no intersection? our work is done
                if (cap == NULL) return;

                if (
                        (1U << cap->size_shift) == intersected_size ||
                        cap->size_shift == MEM_MIN_SIZE_SHIFT  // we cant split that
                ) {
                        // region is contained inside restricted area
                        debug_printk(
                                "removing mem %p - %p\n",
                                cap_ptr_get(cap),
                                cap_ptr_get(cap) + (1 << cap->size_shift)
                        );
                        cap_clear(cap);

                } else if ((1U << cap->size_shift) > intersected_size) {
                        // region contains part of restricted area
                        debug_printk("spliting mem %p, %#x\n", cap_ptr_get(cap), 1 << cap->size_shift);
                        cap->size_shift --;
                        struct cap_t * new_cap = cap_find_free(caps_start, caps_past_end);
                        cap_set_mem(
                                new_cap,
                                cap_ptr_get(cap) + (1 << cap->size_shift),
                                cap->size_shift,
                                0
                        );

                } else {
                        assert(false);
                }
        }
}
