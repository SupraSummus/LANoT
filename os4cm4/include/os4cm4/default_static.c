/**
 * Include this file in your program implementation to use default static memory objects.
 *
 * It's perfectly okay if you define them yourself. This is just a helper to get rid of the boilerplate.
 */

struct kernel_t kernel __attribute__ ((aligned (KERNEL_MEM_SIZE)));
static struct thread_t root_thread __attribute__ ((aligned (THREAD_SIZE)));

/* minimum cap table size is 2 */
_Static_assert(ROOT_THREAD_CAP_TABLE_ENTRIES_SHIFT >= 1);
static struct cap_t root_thread_caps[1 << ROOT_THREAD_CAP_TABLE_ENTRIES_SHIFT] __attribute__ ((aligned (
	(1 << ROOT_THREAD_CAP_TABLE_ENTRIES_SHIFT) * CAP_SIZE
)));

char printk_buffer[PRINTK_BUFFER_SIZE];


static struct cap_t * default_static_init(void) {
	static struct cap_t * caps_free = root_thread_caps;

	// root thread cap is a cap table
	cap_set_cap_table(
		&(root_thread.cap),
		root_thread_caps,
		ROOT_THREAD_CAP_TABLE_ENTRIES_SHIFT
	);

	// root thread has a cap for itself
	cap_set_thread(
		caps_free,
		&root_thread
	);
	caps_free ++;

	// caps for free RAM
	extern const uint32_t end;
	extern const uint32_t __stack;
	debug_printk("end %p, __stack %p\n", &end, &__stack);
	caps_free = mem_add(
		caps_free, root_thread_caps + (1 << ROOT_THREAD_CAP_TABLE_ENTRIES_SHIFT),
		&end, &__stack + 1,
		MEM_PERM_R | MEM_PERM_W | MEM_PERM_X
	);

	return caps_free;
}
