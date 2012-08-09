#ifndef _RTEMS_MMUSUPPORT_H
#define _RTEMS_MMUSUPPORT_H
/*
 * COPYRIGHT (c) 1989-2011.
 * On-Line Applications Research Corporation (OAR).
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <libcpu/arm-cp15.h>
#include <rtems/rtems/status.h>
#include "mmu_support.h"

/* Find a valid page table to map a region for it 
 * FIXME: currently a page table may map up to 1M
 * byte of memory */
static uint32_t search_empty_PT(uint32_t base)
{

  /*  TODO: distinguish between lvl1/lvl2 PT search
 *  according to the base address */

  mmu_lvl2_t  *lvl2_ptr = (mmu_lvl2_t *) _ttbl2_base;

  for ( ; lvl2_ptr < ( lvl2_index + 256 ); lvl2_ptr++ )
    if ( *lvl2_ptr == 0x0 )
      return lvl2_ptr;

  return 0;
}
/* @brief Allocate a page table for one region 
 * FIXME: Specify maximum number of page table/regions here
 * and allocate memory for them 
 */
static mmu_lvl2_t *arm_PT_Allocate(void)
{
  /* allocate lvl2 page table, 1KB size */
  mmu_lvl2_t *lvl2_PT_base = (mmu_lvl2_t *) malloc( 1024 ); 

  if ( lvl2_PT_base == NULL )
    return 0;

  return lvl2_PT_base;
}

/* initialize page table with fault entries */
static inline rtems_status_code arm_PT_Initialize(arm_bsp_mm_mpe *cpu_mpe)
{ 
  pt_type type = cpu_mpe->type;
  uint32_t  paddr,base_paddr;
  uint32_t *PTEptr;

  if ( type == ARM_MMU_LVL2_4KB_PT ) {

    base_paddr = (cpu_mpe->vAddress & 0xfff00000); /* 1:1 Mapping */

    mmu_lvl2_t *PTEptr = (mmu_lvl2_t *)cpu_mpe->ptAdress;

    for (i = 0; i < 256; i++) {

      paddr = base_paddr + ( i << 12 ); /* i = 4K pages */
      PTEptr[i] = MMU_SET_LVL2_PTE_4KB(paddr,
                                       ARM_MMU_AP_NOPR,
                                       1,
                                       0);
    }
  }
  return RTEMS_SUCCESSFUL;
}

/* Activating a region means that installing the corresponding lvl2 page table base address
 *  * at lvl1 pagetable, that is, the system is fully installed to generate exceptions and 
 *   * apply AP/CB when accessing that region according to its attributes. */
static inline rtems_status_code arm_PT_Activate(arm_bsp_mm_mpe *);

/* Add PTEs to a page table. Number of PTEs is determined according to region's size
 *  * Every page table controls one region, Installing a region means that allocating 
 *   * PTEs at CPU page tables, but not necessary activated */
static inline rtems_status_code arm_PT_Install(arm_bsp_mm_mpe *);

/* Deleting lvl2 page table base address from lvl1 page table */
static inline rtems_status_code arm_PT_UnInstall(arm_bsp_mm_mpe *);

/* Deleting a page table corresponds to delete a region */
static inline rtems_status_code arm_PT_Delete(arm_bsp_mm_mpe *);

/* Changing Page table attributes to new attributes */
static inline rtems_status_code arm_PT_Change_Attr(arm_bsp_mm_mpe *,uint32_t AP, uint32_t CB);

/* Verify that size must is multiple of page size */
inline rtems_status_code _CPU_Memory_management_Verify_size(uint32_t size)
{
  if ( (size % MMU_SECT_SIZE) != 0)
    return RTEMS_INVALID_SIZE;

  return RTEMS_SUCCESSFUL;
}

/* Verify that size must is multiple of page size */
inline rtems_status_code _CPU_Memory_management_Initialize(uint32_t size)
{
  mmu_lvl1_t *lvl1_base;

  /* flush the cache and TLB */
  arm_cp15_cache_invalidate();
  arm_cp15_tlb_invalidate();

  /* set manage mode access for all domains */
  arm_cp15_set_domain_access_control(0xffffffff);

  lvl1_base = (mmu_lvl1_t *) _ttbl_base;

  /* set up the trans table */
  mmu_set_map_inval(lvl1_base);
  arm_cp15_set_translation_table_base(lvl1_base);

  /* fill level 1 pagetable with no protection slots, Cache through attributes
   * and 1:1 address mapping
   */
  int i;
  uint32_t paddr = 0x00000000;
  for (i = 0; i < (0x4000 / 4); i++) {
    paddr = i; /* i = 1MB page size */
    lvl1_base[i] = MMU_SET_LVL1_SECT((paddr << 20),
                                     ARM_MMU_AP_NOPR,
                                     0,
                                     1,
                                     0);
    }
  
  /* flush the cache and TLB */
  arm_cp15_cache_invalidate();
  arm_cp15_tlb_invalidate();

  /*  I & D caches turned on */
  arm_cp15_set_control(MMU_CTRL_DEFAULT |
                       MMU_CTRL_D_CACHE_EN |
                       MMU_CTRL_I_CACHE_EN |
                       MMU_CTRL_ALIGN_FAULT_EN |
                       MMU_CTRL_LITTLE_ENDIAN |
                       MMU_CTRL_MMU_EN);
  return RTEMS_SUCCESSFUL;
}

/* @brief Installing @mpe allocates new arm_bsp_mm_mpe for it
 * and set its value for then allocate a new lvl2 page table
 * and activate it */
rtems_status_code _CPU_Memory_management_Install_MPE(
  rtems_memory_management_entry *mpe
)
{
  arm_bsp_mm_mpe *cpu_mpe;
  mmu_lvl1_t     *lvl1_pt;
  int             sectionsNumber; /* 1MB sections */
  size_t          size; /* per Byte */
  uint32_t        vAddress;
  int             PTEIndex;
  uint32_t        paddr;

  lvl1_pt = (mmu_lvl1_t *) _ttbl_base; 
  PTEIndex = ((mpe->region.base & 0xfff00000) >> 20);
  paddr = (mpe->region.base & 0xfff00000);  
  size = mpe->region.size;

  cpu_mpe = (arm_bsp_mm_mpe *) malloc(sizeof(arm_bsp_mm_mpe));

  if ( cpu_mpe == NULL )
    return RTEMS_NO_MEMORY;

  sectionsNumber = (size / MMU_SECT_SIZE);
 
  /* flush the cache and TLB */
  arm_cp15_cache_invalidate();
  arm_cp15_tlb_invalidate(); 

  /*  I & D caches turned off */
  arm_cp15_set_control(MMU_CTRL_DEFAULT |
                       MMU_CTRL_D_CACHE_DES |
                       MMU_CTRL_I_CACHE_DES |
                       MMU_CTRL_ALIGN_FAULT_EN |
                       MMU_CTRL_LITTLE_ENDIAN |
                       MMU_CTRL_MMU_DES);

  /* Set AP for this region to NO ACCESS */

  int i;
  for ( i = 0; i < numPages; i++) {
    uint32_t paddr_base = paddr;
    paddr_base = (i<<20) + paddr;

    lvl1_pt[PTEIndex++] = MMU_SET_LVL1_SECT(paddr_base,
                                            ARM_MMU_AP_NO_ACCESS,
                                            0,
                                            1,
                                            0);
  }
  cpu_mpe->vAddress = mpe->region.base;
  /* for level 1 page table ptAddress is the same as ptlvl1Adress */
  cpu_mpe->ptAddress = lvl1_pt;
  cpu_mpe->ptlvl1Adress = lvl1_pt;
  cpu_mpe->pagesNumber = sectionsNumber;
  cpu_mpe->type = LVL1_PT; /* Default value now */
  cpu_mpe->AP   = ARM_MMU_AP_NO_ACCESS; /* Default when installing entry */
  cpu_mpe->CB   = ARM_MMU_WT; /* Default */
  /* TODO: Domain may be defined as read only, write.. and any page may
   * be attached to it */  
  cpu_mpe->domain = 0; 

  /* install a pointer to high-level API to bsp_mm_mpe */
  mpe->cpu_mpe = cpu_mpe;
  
  return RTEMS_SUCCESSFUL;
}
