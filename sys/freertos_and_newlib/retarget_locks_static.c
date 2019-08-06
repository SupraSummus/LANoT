/* based on https://gist.github.com/thomask77/65591d78070ace68885d3fd05cdebe3a */

// -------------------- Retarget Locks --------------------
//
// share/doc/gcc-arm-none-eabi/pdf/libc.pdf:
//
// Newlib was configured to allow the target platform to provide the locking routines and
// static locks at link time. As such, a dummy default implementation of these routines and
// static locks is provided for single-threaded application to link successfully out of the box on
// bare-metal systems.
//
// For multi-threaded applications the target platform is required to provide an implementa-
// tion for *all* these routines and static locks. If some routines or static locks are missing, the
// link will fail with doubly defined symbols.
//
// (see also newlib/libc/misc/lock.c)
//
#include <sys/lock.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

StaticSemaphore_t __lock___sinit_recursive_mutex;
StaticSemaphore_t __lock___sfp_recursive_mutex;
StaticSemaphore_t __lock___atexit_recursive_mutex;
StaticSemaphore_t __lock___at_quick_exit_mutex;
StaticSemaphore_t __lock___malloc_recursive_mutex;
StaticSemaphore_t __lock___env_recursive_mutex;
StaticSemaphore_t __lock___tz_mutex;
StaticSemaphore_t __lock___dd_hash_mutex;
StaticSemaphore_t __lock___arc4random_mutex;


__attribute__((constructor))
static void init_retarget_locks(void)
{
    xSemaphoreCreateRecursiveMutexStatic(&__lock___sinit_recursive_mutex );
    xSemaphoreCreateRecursiveMutexStatic(&__lock___sfp_recursive_mutex   );
    xSemaphoreCreateRecursiveMutexStatic(&__lock___atexit_recursive_mutex);
    xSemaphoreCreateMutexStatic         (&__lock___at_quick_exit_mutex   );
//    xSemaphoreCreateRecursiveMutexStatic(&__lock___malloc_recursive_mutex);  // see below
    xSemaphoreCreateRecursiveMutexStatic(&__lock___env_recursive_mutex   );
    xSemaphoreCreateMutexStatic         (&__lock___tz_mutex              );
    xSemaphoreCreateMutexStatic         (&__lock___dd_hash_mutex         );
    xSemaphoreCreateMutexStatic         (&__lock___arc4random_mutex      );
}


// Special case for malloc/free. Without this, the full
// malloc_recursive_mutex would be used, which is much slower.
//
struct _reent;

void __malloc_lock(struct _reent *r)
{
    (void)r;
    // TODO: suspend scheduler would be sufficient propably
    taskENTER_CRITICAL();
}


void __malloc_unlock(struct _reent *r)
{
    (void)r;
    taskEXIT_CRITICAL();
}


/* standard semaphore locking */


void __retarget_lock_acquire(_LOCK_T lock)
{
    xSemaphoreTake((SemaphoreHandle_t)lock, portMAX_DELAY);
}


void __retarget_lock_acquire_recursive(_LOCK_T lock)
{
    xSemaphoreTakeRecursive((SemaphoreHandle_t)lock, portMAX_DELAY);
}


int __retarget_lock_try_acquire(_LOCK_T lock)
{
    return xSemaphoreTake((SemaphoreHandle_t)lock, 0);
}


int __retarget_lock_try_acquire_recursive(_LOCK_T lock)
{
    return xSemaphoreTakeRecursive((SemaphoreHandle_t)lock, 0);
}


void __retarget_lock_release(_LOCK_T lock)
{
    xSemaphoreGive((SemaphoreHandle_t)lock);
}


void __retarget_lock_release_recursive(_LOCK_T lock)
{
    xSemaphoreGiveRecursive((SemaphoreHandle_t)lock);
}
