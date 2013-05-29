/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_REALVIEW_PBX_A9_BSP_H
#define LIBBSP_ARM_REALVIEW_PBX_A9_BSP_H

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#ifndef ASM

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_ARM_MMU_CLIENT_DOMAIN 15U

#define BSP_ARM_MMU_READ_ONLY \
  ((BSP_ARM_MMU_CLIENT_DOMAIN << ARM_MMU_SECT_DOMAIN_SHIFT) \
    | ARM_MMU_SECT_AP_0 \
    | ARM_MMU_SECT_AP_2 \
    | ARM_MMU_SECT_DEFAULT)

#define BSP_ARM_MMU_READ_ONLY_CACHED \
  (BSP_ARM_MMU_READ_ONLY | ARM_MMU_SECT_C | ARM_MMU_SECT_B)

#define BSP_ARM_MMU_READ_WRITE \
  ((BSP_ARM_MMU_CLIENT_DOMAIN << ARM_MMU_SECT_DOMAIN_SHIFT) \
    | ARM_MMU_SECT_AP_0 \
    | ARM_MMU_SECT_DEFAULT)

#define BSP_ARM_MMU_READ_WRITE_CACHED \
  (BSP_ARM_MMU_READ_WRITE | ARM_MMU_SECT_C | ARM_MMU_SECT_B)

#define BSP_ARM_MMU_READ_WRITE_DATA \
  BSP_ARM_MMU_READ_WRITE_CACHED

#define BSP_ARM_MMU_READ_ONLY_DATA \
  BSP_ARM_MMU_READ_ONLY_CACHED

#define BSP_ARM_MMU_CODE BSP_ARM_MMU_READ_ONLY_CACHED

#define BSP_ARM_A9MPCORE_PT_BASE 0x1f000600

typedef enum {
  BSP_ARM_A9MPCORE_FATAL_CLOCK_IRQ_INSTALL,
  BSP_ARM_A9MPCORE_FATAL_CLOCK_IRQ_REMOVE
} rvpbxa9_fatal_code;

void rvpbxa9_fatal(rvpbxa9_fatal_code code) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBBSP_ARM_REALVIEW_PBX_A9_BSP_H */
