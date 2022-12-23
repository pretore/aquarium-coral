#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <errno.h>

#include "cmocka.h"

bool posix_memalign_is_overridden = false;
int cmocka_test_posix_memalign(void **const out,
                               const size_t alignment,
                               const size_t size,
                               const char *const file,
                               const int line) {
    if (alignment % sizeof(void *)) {
        return EINVAL;
    }
    { /* check for power of 2 */
        size_t i = alignment - 1;
        do { /* expecting bit pattern of consecutive ones */
            if (!(i & 1)) {
                return EINVAL;
            }
            i >>= 1;
        } while (i);
    }
    if (posix_memalign_is_overridden) {
        return ENOMEM;
    }
    void *const ptr = _test_malloc(size, file, line);
    if (!ptr) {
        return ENOMEM;
    }
    *out = ptr;
    return 0;
}
