#include "cmsis_wasm_thread_sync.h"
#include "cmsis_wasm_time.h"

typedef struct {
  CMSIS_IMPL_QUEUE	queue;
  uint32_t			timeout;
  osStatus_t			ercd;
  pthread_cond_t      cond;
} WasmTaskWaitInfoType;

typedef struct {
  CMSIS_IMPL_QUEUE			wait_queue;
  void* data;
  WasmTaskWaitInfoType		winfo;
} WasmTaskWaitQueueEntryType;

static void WasmTaskSyncWaitInfoInit(WasmTaskWaitInfoType* winfop, uint32_t timeout);



static pthread_mutex_t wasm_mutex;
static pthread_cond_t wasm_cond;

void WasmThreadSyncInit(void)
{
  pthread_mutex_init(&wasm_mutex, NULL);
  pthread_cond_init(&wasm_cond, NULL);
  return;
}

void WasmThreadSyncLock(void)
{
  pthread_mutex_lock(&wasm_mutex);
  return;
}
void WasmThreadSyncUnlock(void)
{
  pthread_mutex_unlock(&wasm_mutex);
  return;
}

/* WAMRのpthread_cond_timedwaitではtimespecの代わりにusecondsを使用する
   そのため、絶対時間として時間を加算する必要が無い */
// static void add_timespec(struct timespec* tmop, uint32_t timeout)
// {
//   uint64_t timeout64 = timeout;
//   clock_gettime(CLOCK_REALTIME, tmop);
//   tmop->tv_nsec += (timeout64 * TIMESPEC_MSEC * TIMESPEC_MSEC);
//   if (tmop->tv_nsec >= TIMESPEC_NANOSEC) {
//     struct timespec over_tmo;
//     over_tmo.tv_sec = (tmop->tv_nsec / TIMESPEC_NANOSEC);
//     over_tmo.tv_nsec = (over_tmo.tv_sec * TIMESPEC_NANOSEC);
//     tmop->tv_sec += over_tmo.tv_sec;
//     tmop->tv_nsec -= over_tmo.tv_nsec;
//   }
//   return;
// }

osStatus_t WasmThreadSyncSleep(uint32_t timeout)
{
  osStatus_t ret = osOK;
  // struct timespec tmo;
  uint32_t useconds;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);

  pthread_mutex_lock(&mutex);
  // add_timespec(&tmo, timeout);
  useconds = timeout * TIMESPEC_MSEC;
  int err = pthread_cond_timedwait(&cond, &mutex, useconds);
  if (err != ETIMEDOUT) {
    ret = osError;
  }
  pthread_mutex_unlock(&mutex);
  return ret;
}

void* WasmThreadSyncWait(WasmQueueHeadType* waiting_queue, uint32_t timeout, osStatus_t* ercdp)
{
  WasmTaskWaitQueueEntryType wait_info;
  // struct timespec tmo;
  uint32_t useconds;

  wait_info.data = NULL;
  WasmTaskSyncWaitInfoInit(&wait_info.winfo, timeout);

  if (waiting_queue != NULL) {
    WasmQueueHeadAddTail(waiting_queue, &wait_info.wait_queue);
  }

  // add_timespec(&tmo, timeout);
  useconds = timeout * TIMESPEC_MSEC;
  int err = pthread_cond_timedwait(&wait_info.winfo.cond, &wasm_mutex, useconds);
  if (waiting_queue != NULL) {
    WasmQueueHeadRemoveEntry(waiting_queue, &wait_info.wait_queue);
  }
  if (ercdp != NULL) {
    if ((err != 0) && (err != ETIMEDOUT)) {
      *ercdp = osError;
    } else if (err == ETIMEDOUT) {
      *ercdp = osErrorTimeoutResource;
    } else {
      *ercdp = wait_info.winfo.ercd;
    }
  }
  return wait_info.data;
}
bool_t WasmThreadSyncWakeupFirstEntry(WasmQueueHeadType* waiting_queue, void* data, osStatus_t ercd)
{
  WasmTaskWaitQueueEntryType *wait_infop = (WasmTaskWaitQueueEntryType*)(waiting_queue->entries);
  if (wait_infop != NULL) {
    wait_infop->data = data;
    wait_infop->winfo.ercd = ercd;
    pthread_cond_signal(&wait_infop->winfo.cond);
    return true;
  } else {
    return false;
  }
}


static void WasmTaskSyncWaitInfoInit(WasmTaskWaitInfoType* winfop, uint32_t timeout)
{
  winfop->timeout = timeout;
  cmsis_impl_queue_initialize(&winfop->queue);
  winfop->ercd = osOK;
  pthread_cond_init(&winfop->cond, NULL);
  return;
}

