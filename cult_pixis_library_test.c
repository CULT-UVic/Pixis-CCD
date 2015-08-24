/*********************************************************************************************/
/*********************************************************************************************/
//Name: cult_pixis_library_test.c
//Content: test to determine whether the PVCAM library will initialize on a computer
//Input: none
//Output: error/success report on console
//File History: created file, 8/20/2015, Levente Buzas (lbuzas@uvic.ca)
//Version: 1.0
/*********************************************************************************************/
/*********************************************************************************************/

/*C libraries*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*PVCAM libraries*/
#include "master.h"
#include "pvcam.h"

/*********************************************************************************************/
/*********************************************************************************************/

int main(void)
{

 /*camera name */
 char cam_name[CAM_NAME_LEN]; 
 
 /*camera handle */
 int16 hCam; 
 
 /*initialize PVCAM library, before this runs only error checking function "pl_error_code" will work */
 rs_bool init_error = pl_pvcam_init();
 
 /*error checking*/
 if(init_error == TRUE)
 {
	 
	 printf("Library initialization error: << %i >>.\n ", pl_error_code() );
	  
 }//if
 else
 {
	 
	 printf("PVCAM successfully initialized. Go on to next test.\n")
	 
 }//else
	 
 /*un-initialize the PVCAM library and free all resources used by it*/
 pl_pvcam_uninit();
	 
 return EXIT_SUCCESS;
 
}//main

/*********************************************************************************************/
/*********************************************************************************************/