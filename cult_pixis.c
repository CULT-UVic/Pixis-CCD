/*********************************************************************************************/
/*********************************************************************************************/
//Name: cult_pixis.c
//Content: image acquisition application for the PI PIXIS 2048F CCD using PVCAM
//Input: none. all parameters hard-coded
//Image acquisition parameters: full frame, 1 image, 10 seconds exposure, 1 binning in x & y
//Output: single .fits file containing exposure, with one keyword in header 
//File History: created file, 6/2/2015, Levente Buzas (lbuzas@uvic.ca)
//Required: PVCAM and NASA FITSIO installed on computer
//			FITSIO: http://heasarc.gsfc.nasa.gov/fitsio/
//			PVCAM: http://www.qimaging.com/support/downloads/legacy-PVCAM.php
//      	or     http://www.roperscientific.de/pvcam.html
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

/*NASA FITSUTIL libraries*/
#include "fitsio.h" 

/*********************************************************************************************/
/*********************************************************************************************/

/*symbolic constants*/
#define START_PIXEL_SERIAL_REGISTER					(0)					
#define END_PIXEL_SERIAL_REGISTER					(2047)
#define SERIAL_BINNING 								(1)

#define START_PIXEL_PARALLEL_REGISTER				(0)
#define END_PIXEL_PARALLEL_REGISTER					(2047)
#define PARALLEL_BINNING 							(1)

#define EXPOSURE_TOTAL								(1)					//number of images to be taken
#define REGION_TOTAL								(1)					//total number of Regions of Interest (ROIs)
#define EXPOSURE_TIME_SECONDS						(10)
#define NUMBER_OF_FRAMES							(1)					//number of frames to be collected as part of the each exposures?

/*********************************************************************************************/
/*********************************************************************************************/

/*prototype for data acquisition function*/
static rs_bool acquire(int16 hCam);

/*prototype for wrapper for FITSIO routines*/
static rs_bool compile_fits(uns16* pixel_stream, uns32 size);

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
	 
	 printf("Camera initialized, ready to accepts commands.\n");
	 
 }//else 
	 
 /* collect data. pass in handle for camera to be used for collection*/
 rs_bool acqusition_error = acquire(hCam);
 
 /*error checking*/
 if(acqusition_error == TRUE){
	 
	 printf("Data could not be collected.\n");
	 	 
 }//if
 else
 {
	 
	 printf("Data collection successful.\n");
	 
 }//else 
 
 /*close current camera, collection is done */
 pl_cam_close(hCam);
 
 /*un-initialize the PVCAM library and free all resources used by it*/
 pl_pvcam_uninit();
 
 return EXIT_SUCCESS;
 
}//main

/*********************************************************************************************/
/*********************************************************************************************/

rs_bool acquire(int16 hCam)
{

 /*this function could take arguments to define ROI, binning, exposure time. currently    */
 /*it is hard-coded to read out the entire chip, with a binning of 1, and exposure of 10s.*/

 rs_bool return_value = TRUE;
 
 printf("Starting test data collection. Full frame readout, no binning, 10 second integration.\n");
 
 /*define ROI - currently full chip.*/
 rgn_type region = {START_PIXEL_SERIAL_REGISTER, 
					END_PIXEL_SERIAL_REGISTER,
					SERIAL_BINNING, 
					START_PIXEL_PARALLEL_REGISTER, 
					END_PIXEL_PARALLEL_REGISTER, 
					PARALLEL_BINNING};
 
 /*data collection parameters */
 uns32 size;								//will be filled by the number of bytes in the pixel stream
 uns16 *frame;								//pointer to the pixel stream
 int16 status;								//state of current exposure is reported in this variable
 uns32 not_needed;			 				//reports exactly how many bytes of data came in so far during exposure. divide by two to get pixels?
 uns16 numberframes = NUMBER_OF_FRAMES;		//number of frames to be taken

 /*initialize data collection functions*/
 return_value = pl_exp_init_seq();
 
 /*error checking*/
 if( return_value == TRUE)
 {
	printf("Data collection functions did not initialize correctly: << %i >>.\n ", pl_error_code() );
	 
 }//if
 else
 {
	 
	 printf("Data collection functions initialized. Setting readout parameters.\n")
	 
 }//else
 
 /*prepare the camera to perform a readout.*/
 return_value = pl_exp_setup_seq(hCam, EXPOSURE_TOTAL, REGION_TOTAL, &region, TIMED_MODE, EXPOSURE_TIME_SECONDS, &size);
 
 if( return_value == TRUE)
 {
	printf("Exposure setup error: << %i >>.\n ", pl_error_code() );
	 
 }//if
 else
 {
	 
	 printf("Exposure parameters are set successfully.\n");
	 
 }//else
	 
 assert(size != 0);

 /*allocate memory buffer for data collection, at least the size of the pixel stream.   */
 /*"size" is the number of bytes in the pixel stream. this memory must be page locked on*/
 /*virtual memory systems?*/
 frame = (uns16*)malloc(size);
 
 printf( "Collecting %i frames.\n", numberframes );

 while( numberframes > 0 ) 
 {

	/*starts one sequence, and returns immediately. a sequence may be one or more exposures, in this case it is "EXPOSURE_TOTAL"*/	 
	 return_value = pl_exp_start_seq(hCam, frame);
	 
		if( return_value == TRUE)
		{
			printf("Error starting the exposure: << %i >>.\n ", pl_error_code() );
	 
		}//if
		else
		{
	 
			printf("Exposure #%i started.\n", numberframes);
	 
		}//else
	 
	 /*check progress of collection by calling "pl_exp_check_status". state of current exposure is returned in variable "status" */
	 while( pl_exp_check_status( hCam, &status, &not_needed ) && (status != READOUT_COMPLETE && status != READOUT_FAILED) );
		
		/*check error codes */
		if( status == READOUT_FAILED ) 
		{
			
			printf( "Data collection error: <<%i>>.\n", pl_error_code() );
			
			return_value =  FALSE;
			
			break;
			
		}//if
		
	 /* frame now contains valid data. line below would print center three points to correlate image and real world coordinates - is that really needed?*/
	 //printf( "Center Three Points: %i, %i, %i\n",  frame[size/sizeof(uns16)/2 - 1],  frame[size/sizeof(uns16)/2], frame[size/sizeof(uns16)/2 + 1] );
	 
	 numberframes--;
	 
	 printf( "Remaining frames: %i.\n", numberframes );
	 
 }//while
 
 
 /*finish the sequence. instead of "0" as third parameter, the standard image bufer "hbuf" can be used.*/
 /*in that case, this function decodes pixel stream and places it into the buffer. this is not required*/
 /*for single region single exposure testing such as this one.*/
 return_value = pl_exp_finish_seq(hCam, frame, 0);
 
		/*error checking*/
		if( setup_error == TRUE)
		{
			printf("After exposure clean-up failed: << %i >>.\n ", pl_error_code() );
	 
		}//if
		else
		{
	 
			printf("Clean-up after exposures done.");
	 
		}//else

 /*make a .fits file from pixel stream with one keyword in header. never tested!*/			
 compile_fits(frame, size);
			
 /*un-initialize the sequence. data collection with parameters above after this can not take place.*/
 pl_exp_uninit_seq();
 
 /*free the memory allocated to store the given frame?*/
 free(frame);
 
 return return_value;
 
}///acquire

/*********************************************************************************************/
/*********************************************************************************************/

rs_bool compile_fits(uns16* pixel_stream, uns32 size)
{	
	/*this function will hopefully compile the pixel stream to a .fits image. it have never been tested*/
	/*Example from: https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node17.html */
	/*Programming guidelines: https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node19.html */
	/*Quick guide: https://heasarc.gsfc.nasa.gov/docs/software/fitsio/quick/quick.html */
	/*Note: NASA FITSIO C library routines needed.*/
	
	/* pointer to the FITS file; defined in "fitsio.h" */
	fitsfile *fptr;    
   	
    int status, ii, jj;
    long  fpixel = 1, naxis = 2, nelements, exposure;
    long naxes[2] = { END_PIXEL_SERIAL_REGISTER+1, END_PIXEL_PARALLEL_REGISTER+1 };   /* image is 2048 pixels wide by 2048 rows */
    short array[END_PIXEL_SERIAL_REGISTER+1][END_PIXEL_PARALLEL_REGISTER+1];
	
	/* initialize status before calling fitsio routines */
    status = 0;         
	
	/* create new file */
    fits_create_file(&fptr, "testfile.fits", &status);   

    /* Create the primary array image (16-bit short integer pixels */
    fits_create_img(fptr, SHORT_IMG, naxis, naxes, &status);

    /* Write a keyword; must pass the ADDRESS of the value */
    exposure = EXPOSURE_TIME_SECONDS;
    fits_update_key(fptr, TLONG, "EXPOSURE", &exposure, "Total Exposure Time", &status);

    /* Initialize the values in the image with a linear ramp function. what is a linear ramp function???*/
    for (jj = 0; jj < naxes[1]; jj++)
	{
        for (ii = 0; ii < naxes[0]; ii++)
		{
            array[jj][ii] = ii + jj;
			
		}//for	
	}//for

	/* number of pixels to write */
    nelements = naxes[0] * naxes[1];         

    /* Write the array of integers to the image */
    fits_write_img(fptr, TSHORT, fpixel, nelements, array[0], &status);

	/* close the file */
    fits_close_file(fptr, &status);            

	/* print out any error messages */
    fits_report_error(stderr, status);  
	
    return status;
	
}//compile_fits

/*********************************************************************************************/
/*********************************************************************************************/
