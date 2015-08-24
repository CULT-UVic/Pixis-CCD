/*********************************************************************************************/
/*********************************************************************************************/
//Name: cult_pixis_init_cam.c
//Content: test initializing the CCD & gathering the resources for data future data collection
//Input: none
//Output: error/success message and camera name if retrieved
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
	 
	 printf("Camera found: << %s >>.\n", cam_name);
	 
 }//else
 
 /*reserves and initializes camera hardware. provides sole camera access to current user, until "pl_pvcam_uninit" is called*/
 rs_bool camera_error = pl_cam_open(cam_name, &hCam, OPEN_EXCLUSIVE);

 /*error checking*/
 if(camera_error == TRUE){
	 
	 printf("Camera initialization error: << %i >>.\n ", pl_error_code() );
	 	 
 }//if
 else
 {
	 
	 printf("Camera initialized, ready to accepts commands. You can try collecting data now.\n");
	 
 }//else 
	 
 /*close current camera*/
 pl_cam_close(hCam);
 
 /*un-initialize the PVCAM library and free all resources used by it*/
 pl_pvcam_uninit();
	 
 return EXIT_SUCCESS;
 
}//main

/*********************************************************************************************/
/*********************************************************************************************/