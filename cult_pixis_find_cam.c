/*********************************************************************************************/
/*********************************************************************************************/
//Name: cult_pixis_find_cam.c
//Content: test to get name of CCD attached to computer
//Input: none
//Output: error/success message, and camera name if retrieved
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
	 
	 printf("PVCAM successfully initialized.\n")
	 
 }//else
	 
 /*get name of camera, takes parameters referring to cameras starting from 0, which is the first camera by definition*/
 rs_bool name_error = pl_cam_get_name(0, cam_name);

 /*error checking*/
 if(name_error == TRUE){
	 
	 printf("Camera name error: << %i >>.\n ", pl_error_code() );
	  
 }//if
 else
 {
	 
	 printf("Camera found, name: << %s >>. Go on to next test. \n", cam_name);
	 
 }//else
 
 /*un-initialize the PVCAM library and free all resources used by it*/
 pl_pvcam_uninit();
	 
 return EXIT_SUCCESS;
 
}//main

/*********************************************************************************************/
/*********************************************************************************************/

