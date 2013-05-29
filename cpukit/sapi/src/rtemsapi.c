/**
 * @file
 *
 * @brief Initializes the RTEMS API
 *
 * @ingroup ClassicRTEMS
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#define RTEMS_API_INIT

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/rtemsapi.h>

#include <rtems/rtems/intr.h>
#include <rtems/rtems/barrier.h>
#include <rtems/rtems/clock.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/dpmem.h>
#include <rtems/rtems/event.h>
#include <rtems/rtems/message.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/mp.h>
#endif
#include <rtems/rtems/part.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/rtems/region.h>
#include <rtems/rtems/sem.h>
#include <rtems/rtems/signal.h>
#include <rtems/rtems/timer.h>

Objects_Information *_RTEMS_Objects[ OBJECTS_RTEMS_CLASSES_LAST + 1 ];

void _RTEMS_API_Initialize(void)
{
  /*
   * Install our API Object Management Table and initialize the
   * various managers.
   */
  _Objects_Information_table[OBJECTS_CLASSIC_API] = _RTEMS_Objects;

  #if defined(RTEMS_MULTIPROCESSING)
    _Multiprocessing_Manager_initialization();
  #endif

  _RTEMS_tasks_Manager_initialization();
  _Timer_Manager_initialization();
  _Signal_Manager_initialization();
  _Event_Manager_initialization();
  _Message_queue_Manager_initialization();
  _Semaphore_Manager_initialization();
  _Partition_Manager_initialization();
  _Region_Manager_initialization();
  _Dual_ported_memory_Manager_initialization();
  _Rate_monotonic_Manager_initialization();
  _Barrier_Manager_initialization();
}
