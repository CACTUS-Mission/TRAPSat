/*******************************************************************************
** File: vc0706_device.c
**
** Purpose:
**   This is a source file for the VC0706 application
**
**   int VC0706_takePics(void) "VC0706 Camera capture Loop"
**
**
*******************************************************************************/
/*
** VC0706 Master Header
*/
#include "vc0706.h"
#include "vc0706_child.h"

/*
** Parallel Pins
*/
//int PARALLEL_PIN_BUS[6] = {31, 32, 33, 34, 35, 36};
int PARALLEL_PIN_BUS[6] = {36, 35, 34, 33, 32, 31};

/*
** Parallel functions
*/
void setupParallelPhotoCount(void);
void updatePhotoCount(uint8 pic_count);

/*
** External References
*/
extern vc0706_hk_tlm_t VC0706_HkTelemetryPkt;
extern struct led_t led;
extern struct mux_t mux;
extern struct Camera_t cam;

extern char num_reboots[3];

/*
** VC0706 take Pictures Loop
*/
int VC0706_takePics(void)
{
	int32 hk_packet_succes = 0;

    /*
    ** Path that pictures should be stored in
    **
    ** NOTE: if path is greater than 16 chars, imageName[] in vc0706_core.h will need to be enlarged accordingly.
    */
    char path[OS_MAX_PATH_LEN];
    memset(path, '\0', sizeof(path));

    char file_name[15];
    memset(file_name, '\0', sizeof(file_name));


    /*
    ** get Num reboots
    */
    //OS_printf("VC0706: Attempting to setNumReboots()...\n");
    //setNumReboots();

    /*
    ** Attempt to initialize LED
    */
    led_init(&led, (int)LED_PIN);

    /*
    ** Initialize MUX
    */
    if(mux_init(&mux, (int)MUX_SEL_PIN) == -1)
    {
        OS_printf("MUX initialization error.\n");
        return -1;
    }

    /*
    ** Attempt to initalize Camera
    */
    if(init(&cam) == -1) // Error
    {
        OS_printf("Camera initialization error.\n");
        return -1;
    }

    /*
    ** Initialize the Parallel Pins
    */
    setupParallelPhotoCount();




    /*
    ** infinite Camera loop
    ** w/ no delay
    */
    unsigned int num_pics_stored = 1;
    for ( ;; ) // NOTE: we will need to add flash and MUX implementation. Easy, but should be broken into separate headers.
    {

       /*
  g++ vc0706_test.cpp -o vc0706_test -lwiringPig++ vc0706_test.cpp -o vc0706_test -lwiringPi      ** Switch Cameras -- Has not been tested with hardware yet
        */
        if( mux_switch(&mux) == -1)
		{
            OS_printf("vc0706::mux_switch() failed.\n");
		} // Only fails if mux_state != 0|1

    	//OS_printf("Switched camera. MUX State: %d\n", mux.mux_state);

        /*
        ** Get camera version, another way to check that the camera is working properly. Also necessary for initialization.
        **
        ** NOTE: Not sure if this should be done every loop iteration. It is a good way to check on the Camera, but maybe wasteful of time.
        */
	    if ((getVersion(&cam)) == -1)
        {
            OS_printf( "Failed communication to Camera.\n"); // NOTE: vc0706_core::checkReply() does CVE logging.
            // return -1; // should never stop the task, just keep trying.
            continue; // loop start over
	    }

        /*
        ** Set Path for the new image
	    **
	    ** Format:
	    ** /ram/images/<num_reboots>_<camera 0 or 1>_<num_pics_stored>.jpg
        */
		//OS_printf("VC0706: Calling sprintf()...\n");
        
        int ret = 0;
        ret = snprintf(file_name, sizeof(file_name), "%.3s_%d_%.4u.jpg", num_reboots, mux.mux_state, num_pics_stored); // cFS /exe relative path
        if(ret < 0)
        {
            OS_printf("sprintf err: %s\n", strerror(ret));
            continue;
        }

        ret = snprintf(path, sizeof(path), "/ram/images/%s", file_name); // cFS /exe relative path
        if(ret < 0)
        {
            OS_printf("sprintf err: %s\n", strerror(ret));
            continue;
        }

    	/*
        ** Actually take the picture
        */
    	//OS_printf("VC0706: Calling takePicture(&cam, \"%s\")...\n", path);
        char* pic_file_name = takePicture(&cam, path);
        if(pic_file_name != (char *)NULL)
		{
		    //OS_printf("Debug: Camera took picture. Stored at: %s\n", pic_file_name);

		    /*
		    ** Put Image name on telem packet
		    */
		    if( (hk_packet_succes = snprintf(VC0706_HkTelemetryPkt.vc0706_filename, 15, "%.*s", 15, (char *)pic_file_name+12)) < 0 ) // only use the filename, not path.
		    {
		    	OS_printf("VC0706: ERROR: HK sprintf ret [%d] filename [%.*s]\n", hk_packet_succes, 15, (char * )&pic_file_name[12]);
		    	// continue
		    }
		    else
		    {
			  //OS_printf("VC0706: Wrote Picture Filename to HK Packet. Sent: '%.*s'\n", 15, (char * )&pic_file_name[12], hk_packet_succes);
			    VC0706_SendTimFileName(file_name); 
			}
		    //OS_printf("VC0706: VC0706_HkTelemetryPkt.vc0706_filename: '%s'\n", VC0706_HkTelemetryPkt.vc0706_filename);

			/*
			** update number of pics taken on the parallel pins
			*/            
			updatePhotoCount((uint8) num_pics_stored);

		    /*
		    ** incriment num pics for filename
		    */
		    num_pics_stored++;
		}
		else 
		{
			VC0706_SendTimFileName("error.txt"); // contains: "image failed to be taken."
		}

    } /* Infinite Camera capture Loop End Here */

    return(0);
}


/*
** initializes parallel pins on the parallel line designated for photo count [pins 31-36]
*/
void setupParallelPhotoCount(void)
{
	int i;
	for(i=0; i<6; i++)
	{
		pinMode(PARALLEL_PIN_BUS[i], OUTPUT);
	}

    // initialize to 0
    updatePhotoCount((uint8)0);
}

/*
** updatePhotoCount() - writes the specified int to 6 bits on out parallel line
** 						this function assumes the designated ouputs are already 
*/
void updatePhotoCount(uint8 pic_count)
{
	int gpio_pin;
	int i;
 	for(i=0; i < 6; i++)
    {
        gpio_pin = PARALLEL_PIN_BUS[i];

        if ( pic_count & (1 << i) )
        {
            digitalWrite(gpio_pin, HIGH);
        }
        else
        {
            digitalWrite(gpio_pin, LOW);
        }
    }
}

