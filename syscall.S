    .syntax unified
    .thumb
    .text
    .align 2                // Align the function code to a 4-byte (2^n) word boundary.
    .thumb                  // Use THUMB-2 instrctions instead of ARM.
    .globl syscall         // Make the function globally accessible.
    .thumb_func // Use THUMB-2 for the following function.

syscall:
	// Do syscall. Syscall args are already in place (r0, r1, r2, r3)
	svc 42
	// syscall result is already in r0.
	// no need to `bx LR` because kernel returns control directly to LR
	bx lr
