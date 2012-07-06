/*
 * Copyright (C) 2011. Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <libcpu/memoryprotection.h>
#include <rtems/libmmu.h>
#include <rtems/score/isr.h>

/**
 *  @brief Initialize the memory protection manager and the hardware.
 */
rtems_status_code rtems_memory_management_initialize( )
{
  rtems_status_code status;
  //ISR_Level         level;
  //_ISR_Disable( level );
    status = _CPU_Memory_management_Initialize();
  //_ISR_Enable( level );
  return status;
}

/**
 *  Verify that @a perms is valid for the underlying architecture
 */
rtems_status_code rtems_memory_management_verify_permission(
    rtems_memory_management_permission perms
) {
  rtems_status_code status;
  ISR_Level         level;
  _ISR_Disable( level );
    status = _CPU_Memory_management_Verify_permission(perms);
  _ISR_Enable( level );
  return status;
}

/**
 *  Verify that @a size is valid for the underlying architecture
 */
rtems_status_code rtems_memory_management_verify_size(
    size_t size
) {
  rtems_status_code status;
  ISR_Level         level;
  _ISR_Disable( level );
    status = _CPU_Memory_management_Verify_size(size);
  _ISR_Enable( level );
  return status;
}

/**
 * @brief Install the memory protection entry to the enforcement mechanism.
 */
rtems_status_code rtems_memory_management_install_entry(
  rtems_memory_management_entry *mpe
) {
  rtems_status_code status;
  ISR_Level         level;
  _ISR_Disable( level );
    status = _CPU_Memory_management_Install_MPE(mpe);
    if ( status == RTEMS_SUCCESSFUL ) {
      mpe->installed = true;
    }
  _ISR_Enable( level );
  return status;
}

/**
 * @brief Uninstall the memory protection entry from the enforcement mechanism.
 */
rtems_status_code rtems_memory_management_uninstall_entry(
  rtems_memory_management_entry *mpe
) {
  rtems_status_code status;
  ISR_Level         level;
  _ISR_Disable( level );
  status = _CPU_Memory_management_Uninstall_MPE(mpe);
  if ( status == RTEMS_SUCCESSFUL ) {
    mpe->installed = false;
  }
  _ISR_Enable( level );
  return status;
}

rtems_status_code rtems_memory_management_set_write
(
  rtems_memory_management_entry *mpe
) {
  rtems_status_code status;
  ISR_Level         level;
  _ISR_Disable( level );
  status ==  _CPU_Memory_management_Set_write(mpe);
  if ( status == RTEMS_SUCCESSFUL ) {
    /* To edit the new permission opaque type
 *     with enabled write permission */
  }

  _ISR_Enable( level );
  return status;
}
