/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is called from init_exec and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#define CONFIGURE_INIT
#include "system.h"
#include <stdio.h>
#include <stdlib.h>
#include <rtems/libmm.h>

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code status;
  uint32_t access;
  size_t  blocksize;
  int i;
  unsigned char* a1;
  unsigned char* a2;
  void * alut_search_addr1;
  void * alut_search_addr2;
  rtems_memory_management_entry *mpe;
  
  alut_search_addr1 = (char*) 0xffffffff;
  alut_search_addr2 = (char*) 0x00100000;
  
  rtems_memory_management_region_descriptor r1 = { 
    .name = "Valid Entry-1",
    .base = 0x00100000,
    .size = 0x200000
  };
  rtems_memory_management_region_descriptor r2 = {
    .name = "Valid Entry-2",
    .base = 0x00400000,
    .size = 0x100000
  };

  puts( "\n\n*** LIBMM LOW LEVEL (BSP) TEST BEGINS ***\n" );
  puts( "initialize the memory protect manager\n");

  rtems_memory_management_install_alut();

  status = rtems_memory_management_initialize ( );

  if(status != RTEMS_SUCCESSFUL)
  {
    printf("Failed:initialize the memory protect manager failed; status = %d\n",status);
    exit(0);
  }
  printf("ALUT created\n");
    
  printf("Test 1 : Adding valid Entry-1 into ALUT\n");
  status = rtems_memory_management_create_entry(r1, &mpe); 
  if(status == RTEMS_SUCCESSFUL){
    printf("Passed : Entry Added\n");
  }
  else{
    printf("Failed : Entry addition failed, status = %d\n",status);
  }
  
  printf("Test 2 : Installing Entry-1 into HW\n");
  status = rtems_memory_management_install_entry(mpe);

  if(status == RTEMS_SUCCESSFUL){
     printf("Passed : Installing Entry-1 into HW = %d\n",status);
   }   
   else{
     printf("Failed : to Install Entry-1 into HW = %d\n",status);
   }

  printf("Test 3 : Set Read only for installed Entry-1 permissions \n");
  status = rtems_memory_management_set_read_only(mpe); 

  if(status == RTEMS_SUCCESSFUL){
     printf("Passed : Set Read only permissions = %d\n",status);
   }
   else{
     printf("Failed : to Set Read only permissions = %d\n",status);
   }

  printf("Test 4 : Adding another valid Entry-2\n");

  status = rtems_memory_management_create_entry(r2, &mpe); 
  if(status == RTEMS_SUCCESSFUL){
    printf("Passed : Entry-2 successfully added, status = %d\n",status);
  }
  else{
    printf("Failed : Entry adding failed, status = %d\n",status);
  }
  printf("Test 5 : Installing Entry-2 into HW\n");
  status = rtems_memory_management_install_entry(mpe);

  if(status == RTEMS_SUCCESSFUL){
     printf("Passed : Installing Entry into HW = %d\n",status);
   }
   else{
     printf("Failed : to Install Entry into HW = %d\n",status);
   }
  status = rtems_memory_management_install_entry(mpe);

  printf("Test 6 : Set Write permission for installed Entry-2 \n");
  status = rtems_memory_management_set_write(mpe);

  if(status == RTEMS_SUCCESSFUL){
     printf("Passed : Set Write permissions = %d\n",status);
   }
   else{
     printf("Failed : to Set Write permissions = %d\n",status);
   }

  char a;
  a1 = alut_search_addr1;
  printf("Checking MMU exception 1:Read from Unmapped block\n"); 
  a = *a1++; 
  
  a1 = alut_search_addr1 + 0x2000;
  printf("Checking MMU exception 2: Write to Unmapped block\n");  
  //*a1++ = 0xCC;

  // this one isn't an exception.
  a2 = r1.base;
  printf("Checking MMU exception 3: Read from readonly block\n"); 
    a = *a2++;
  
  printf("Checking MMU exception 4: Write to readonly block  \n");
   *a2++ = 0xCC;

  printf(  "\n\n*** LIBMM LOW LEVEL (BSP) TEST ENDS ***\n" );
  exit( 0 ); 
}

