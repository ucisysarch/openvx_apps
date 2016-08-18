/* 
 * Copyright (c) 2012-2016 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * KHRONOS STANDARDS. THE UNMODIFIED, NORMATIVE VERSIONS OF KHRONOS
 * SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT
 *    https://www.khronos.org/registry/
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */

#ifndef _OPENVX_INT_OSAL_H_
#define _OPENVX_INT_OSAL_H_

/*!
 * \file
 * \brief The internal operating system abstraction layer.
 * \author Erik Rainey <erik.rainey@gmail.com>
 *
 * \defgroup group_int_osal Internal OSAL API
 * \ingroup group_internal
 * \brief The Internal Operating System Abstraction Layer API.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Creates a semaphore object and sets to a given count.
 * \ingroup group_int_osal
 */
vx_bool ownCreateSem(vx_sem_t *sem, vx_uint32 count);

/*! \brief Releases a semaphore object.
 * \ingroup group_int_osal
 */
void ownDestroySem(vx_sem_t *sem);

/*! \brief
 * \ingroup group_int_osal
 */
vx_bool ownSemPost(vx_sem_t *sem);

/*! \brief
 * \ingroup group_int_osal
 */
vx_bool ownSemWait(vx_sem_t *sem);

/*! \brief
 * \ingroup group_int_osal
 */
vx_bool ownSemTryWait(vx_sem_t *sem);

/*! \brief
 * \ingroup group_int_osal
 */
vx_bool ownJoinThread(vx_thread_t thread, vx_value_t *value);

/*! \brief
 * \ingroup group_int_osal
 */
vx_thread_t ownCreateThread(vx_thread_f func, void *arg);

/*! \brief
 * \ingroup group_int_osal
 */
void ownSleepThread(vx_uint32 milliseconds);

/*! \brief
 * \ingroup group_int_osal
 */
vx_uint64 ownCaptureTime();

/*! \brief
 * \ingroup group_int_osal
 */
vx_uint64 ownGetClockRate();

/*! \brief
 * \ingroup group_int_osal
 */
void ownStartCapture(vx_perf_t *perf);

/*! \brief
 * \ingroup group_int_osal
 */
void ownStopCapture(vx_perf_t *perf);

/*! \brief
 * \ingroup group_int_osal
 */
void ownInitPerf(vx_perf_t *perf);

/*! \brief Prints the performance information.
 * \ingroup group_int_osal
 */
void ownPrintPerf(vx_perf_t *perf);

/*! \brief
 * \ingroup group_int_osal
 */
vx_bool ownDeinitEvent(vx_event_t *e);

/*! \brief
 * \ingroup group_int_osal
 */
vx_bool ownInitEvent(vx_event_t *e, vx_bool autoreset);

/*! \brief
 * \ingroup group_int_osal
 */
vx_bool ownWaitEvent(vx_event_t *e, vx_uint32 timeout);

/*! \brief
 * \ingroup group_int_osal
 */
vx_bool ownSetEvent(vx_event_t *e);

/*! \brief
 * \ingroup group_int_osal
 */
vx_bool ownResetEvent(vx_event_t *e);

/*! \brief
 * \ingroup group_int_osal
 */
void ownInitQueue(vx_queue_t *q);

/*! \brief
 * \ingroup group_int_osal
 */
vx_queue_t *ownCreateQueue();

/*! \brief
 * \ingroup group_int_osal
 */
void ownDestroyQueue(vx_queue_t **pq);

/*! \brief
 * \ingroup group_int_osal
 */
vx_bool ownWriteQueue(vx_queue_t *q, vx_value_set_t *data);

/*! \brief
 * \ingroup group_int_osal
 */
vx_bool ownReadQueue(vx_queue_t *q, vx_value_set_t **data);

/*! \brief
 * \ingroup group_int_osal
 */
void ownPopQueue(vx_queue_t *q);

/*! \brief
 * \ingroup group_int_osal
 */
void ownDeinitQueue(vx_queue_t *q);

/*! \brief
 * \ingroup group_int_osal
 */
vx_module_handle_t ownLoadModule(vx_char * name);

/*! \brief
 * \ingroup group_int_osal
 */
void ownUnloadModule(vx_module_handle_t mod);

/*! \brief
 * \ingroup group_int_osal
 */
vx_symbol_t ownGetSymbol(vx_module_handle_t mod, vx_char * name);

/*! \brief Converts a vx_uint64 to a float in milliseconds.
 * \ingroup group_int_osal
 */
vx_float32 ownTimeToMS(vx_uint64 c);

void ownDestroyThreadpool(vx_threadpool_t **ppool);

vx_threadpool_t *ownCreateThreadpool(vx_uint32 numThreads,
                                    vx_uint32 numWorkItems,
                                    vx_size sizeWorkItem,
                                    vx_threadpool_f worker,
                                    void *arg);

vx_bool ownIssueThreadpool(vx_threadpool_t *pool, vx_value_set_t workitems[], uint32_t numWorkItems);

vx_bool ownCompleteThreadpool(vx_threadpool_t *pool, vx_bool blocking);

#ifdef __cplusplus
}
#endif

#endif
