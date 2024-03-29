#include "cmsis_wasm_time.h"
#include "cmsis_wasm_thread_sync.h"

osStatus_t osKernelStart(void)
{
  WasmThreadSyncInit();
  (void)WasmTimerInit();
  CMSIS_IMPL_INFO("osKernelStart");
  return osOK;
}

uint32_t osKernelGetTickCount(void)
{
  return WasmTimeGetTickCount();
}
