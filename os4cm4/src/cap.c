#include <assert.h>
#include <os4cm4/cap.h>
#include <os4cm4/thread.h>
#include <stddef.h>

struct cap_t * cap_find (struct cap_t * root_cap, uint32_t path, unsigned int depth) {
        assert(depth <= 32);
        while (depth > 0 && root_cap != NULL) {
                if (root_cap->type == CAP_TYPE_CAP_TABLE) {
                        int shift = root_cap->size_shift - CAP_SIZE_SHIFT;
                        int cap_index = path >> (32 - shift);
                        root_cap = (struct cap_t *)(cap_ptr_get(root_cap)) + cap_index;
                        depth --;
                        path <<= shift;
                } else {
                        root_cap = NULL;
                }
        }
        return root_cap;
}

bool is_aligned (void const* ptr, unsigned int size_shift) {
        assert(size_shift <= 32);
        if ((unsigned int)ptr << (32 - size_shift)) return false;
        return true;
}

bool cap_is_sane (struct cap_t const* cap) {
        if (!is_aligned(cap, CAP_SIZE_SHIFT)) return false;
        if (cap == NULL) return false;
        return true;
}

void cap_clear (struct cap_t * cap) {
        assert(cap_is_sane(cap));
        assert(cap->type != CAP_TYPE_EMPTY);
        cap->type = CAP_TYPE_EMPTY;
}

void cap_set (
        struct cap_t * cap,
        unsigned int type,
        void const* object_data,
        unsigned int size_shift
) {
        assert(cap_is_sane(cap));
        assert(cap->type == CAP_TYPE_EMPTY);
        assert(type < CAP_TYPES_COUNT);
        assert(size_shift >= MEM_MIN_SIZE_SHIFT);  // object data is at least 32B long
        assert(is_aligned(object_data, size_shift));
        cap->type = type;
        cap->ptr = (unsigned int)object_data >> MEM_MIN_SIZE_SHIFT;
        cap->size_shift = size_shift;
}

void cap_set_cap_table (
        struct cap_t * cap,
        struct cap_t * caps,
        unsigned int entries_shift
) {
        cap_set(cap, CAP_TYPE_CAP_TABLE, caps, entries_shift + CAP_SIZE_SHIFT);
}

void cap_set_thread (
        struct cap_t * cap,
        struct thread_t * thread
) {
        cap_set(cap, CAP_TYPE_THREAD, thread, THREAD_SIZE_SHIFT);
}

void cap_set_mem (
        struct cap_t * cap,
        void const* ptr,
        unsigned int size_shift,
        unsigned int perms
) {
        (void)perms;  // TODO save them
        cap_set(cap, CAP_TYPE_MEM, ptr, size_shift);
}
