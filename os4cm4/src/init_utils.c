#include <stdbool.h>
#include <os4cm4/cap.h>
#include <stddef.h>
#include <assert.h>

bool is_intersected(
        void * ptr_a, unsigned int size_shift_a,
        void * ptr_b, unsigned int size_shift_b
) {
        void * end_a = ptr_a + (1 << size_shift_a);
        void * end_b = ptr_b + (1 << size_shift_b);
        if (end_a <= ptr_b) return false;
        if (end_b <= ptr_a) return false;
        return true;
}

struct cap_t * mem_find_intersecting (
        struct cap_t * caps_start, struct cap_t * caps_past_end,
        void * ptr, unsigned int size_shift
) {
        for (struct cap_t * cap = caps_start; cap < caps_past_end; cap ++) {
                if (cap->type != CAP_TYPE_MEM) continue;
                if (is_intersected(
                        cap_ptr_get(cap), cap->size_shift,
                        ptr, size_shift
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
        void * ptr, unsigned int size_shift
) {
        assert(size_shift >= MEM_MIN_SIZE_SHIFT);
        assert(is_aligned(ptr, size_shift));

        debug_printk("mem_remove %p, %i\n", ptr, size_shift);

        struct cap_t * cap;
        while ((cap = mem_find_intersecting(
                caps_start, caps_past_end,
                ptr, size_shift
        ))) {
                if (cap->size_shift <= size_shift) {
                        // region is contained inside restricted area
                        debug_printk("removing mem %p, %#x\n", cap_ptr_get(cap), 1 << cap->size_shift);
                        cap_clear(cap);

                } else {
                        // region contains restricted area
                        debug_printk("spliting mem %p, %#x\n", cap_ptr_get(cap), 1 << cap->size_shift);
                        assert(cap->size_shift > MEM_MIN_SIZE_SHIFT);
                        cap->size_shift --;
                        struct cap_t * new_cap = cap_find_free(caps_start, caps_past_end);
                        cap_set_mem(
                                new_cap,
                                cap_ptr_get(cap) + (1 << cap->size_shift),
                                cap->size_shift,
                                0
                        );
                }
        }
}
