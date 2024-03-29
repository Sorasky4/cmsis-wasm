#ifndef _CMSIS_WASM_H_
#define _CMSIS_WASM_H_

#include "cmsis_impl_types.h"

#define WASM_THREAD_SYNC_WAIT_FOREVER		(-1U)

extern bool_t CurrentContextIsISR(void);

#define TIMESPEC_NANOSEC    1000000000LLU
#define TIMESPEC_MSEC       1000LLU

#define _GNU_SOURCE
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


#endif /* _CMSIS_WASM_H_ */
