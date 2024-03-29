#ifndef _CMSIS_SEMAPHORES_PRIVATE_H_
#define _CMSIS_SEMAPHORES_PRIVATE_H_

#include "cmsis_wasm_queue.h"
#define WASM_SEM_HEAD_MAGICNO		0xDEADBEED
typedef struct {
  uint32_t			max_count;
  uint32_t			count;
  uint32_t			magicno;
  WasmQueueHeadType	waiting;
} CmsisSemType;

extern osStatus_t osSemaphoreAcquire_nolock(CmsisSemType* semp, uint32_t timeout);
extern osStatus_t osSemaphoreRelease_nolock(CmsisSemType* semp);


#endif /* _CMSIS_SEMAPHORES_PRIVATE_H_ */
