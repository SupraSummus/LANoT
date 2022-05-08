#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <os4cm4/mem.h>
#include <os4cm4/printk.h>

#define CAP_TYPE_EMPTY (0)
#define CAP_TYPE_CAP_TABLE (1)
#define CAP_TYPE_THREAD (2)
#define CAP_TYPE_MEM (3)
#define CAP_TYPES_COUNT (4)

#define CAP_SIZE_SHIFT (4)
#define CAP_SIZE (1 << CAP_SIZE_SHIFT)

struct cap_t {
        /* Members of this struct are all mixed because we are SAVING SPACE HARD MODE */

        /* Pointer to the object data.
         * Lower 5 bits are always 0 because all objects are 32B aligned.
         */
        uint32_t ptr : (32 - MEM_MIN_SIZE_SHIFT);

        /* size of the object is 2^size_shift bytes */
        uint32_t size_shift : 5;

        uint32_t derived_ptr : (32 - CAP_SIZE_SHIFT);

        /* Type of the object. */
        uint32_t type : 4;

        uint32_t next_sibling_ptr : (32 - CAP_SIZE_SHIFT);
        uint32_t perms_high : 4;
        uint32_t prev_sibling_ptr : (32 - CAP_SIZE_SHIFT);
        uint32_t perms_low : 4;
};

_Static_assert(sizeof(struct cap_t) == CAP_SIZE);

static inline void * cap_ptr_get (struct cap_t * cap) {
        return (void *)(cap->ptr << MEM_MIN_SIZE_SHIFT);
}

struct cap_t * cap_find (struct cap_t * root_cap, uint32_t path, unsigned int depth);

bool is_aligned (void const* ptr, unsigned int size_shift);

bool cap_is_sane (struct cap_t const* cap);

void cap_clear (struct cap_t * cap);

void cap_set (
        struct cap_t * cap,
        unsigned int type,
        void const* object_data,
        unsigned int size_shift
);

/**
 * capability table
 */

static inline void cap_set_cap_table (
	struct cap_t * cap,
	struct cap_t * caps,
	unsigned int entries_shift
) {
	cap_set(cap, CAP_TYPE_CAP_TABLE, caps, entries_shift + CAP_SIZE_SHIFT);
}

static inline struct cap_t * cap__cap_table__caps (struct cap_t * cap) {
	assert(cap->type == CAP_TYPE_CAP_TABLE);
	return cap_ptr_get(cap);
}

static inline unsigned int cap__cap_table__entries_shift (struct cap_t * cap) {
	assert(cap->type == CAP_TYPE_CAP_TABLE);
	assert(cap->size_shift >= CAP_SIZE_SHIFT);
	return cap->size_shift - CAP_SIZE_SHIFT;
}

/**
 * thread
 */

struct thread_t;
void cap_set_thread (
        struct cap_t * cap,
        struct thread_t * thread
);

/**
 * memory region
 */

void cap_set_mem (
        struct cap_t * cap,
        void const * ptr,
        unsigned int size_shift,
        unsigned int perms
);

static inline bool cap_is_mem (struct cap_t const * cap) {
	return cap->type == CAP_TYPE_MEM;
}
