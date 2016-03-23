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


/*
** External References
*/
extern vc0706_hk_tlm_t VC0706_HkTelemetryPkt;
extern struct led_t led;
extern struct mux_t mux;
extern struct Camera_t cam;

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
    char * path = malloc(OS_MAX_PATH_LEN);

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
    ** infinite Camera loop
    ** w/ no delay
    */
    unsigned int num_pics_stored = 0;;
    for ( ;; ) // NOTE: we will need to add flash and MUX implementation. Easy, but should be broken into separate headers.
    {
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
		unsigned int num_reboots = 0; // initialized to undefined
        //num_reboots = getNumReboots(); // not writen yet -- ask Keegan.
        int ret = sprintf(path, "/ram/images/%.3u_%d_%.4u.jpg", num_reboots, mux.mux_state, num_pics_stored); // cFS /exe relative path
    	if(ret < 0)
    	{
    	    OS_printf("sprintf err: %s\n", strerror(ret));
    	    continue;
		}

        /*
        ** Switch Cameras -- Has not been tested with hardware yet
        */
        if( mux_switch(&mux) == -1)
		{
            OS_printf("vc0706::mux_switch() failed.\n");
		} // Not sure what failure situation should be, maybe don't care? Should never fail anyway. Only fails if mux_state != 0|1

    	/*
        ** Actually takes the picture
        */
    	OS_printf("VC0706: Calling takePicture(&cam, \"%s\")...\n", path);
        char* pic_file_name = takePicture(&cam, path);
        if(pic_file_name != (char *)NULL)
		{
		    OS_printf("Debug: Camera took picture. Stored at: %s\n", pic_file_name);

		    /*
		    ** Put Image name on telem packet
		    */
		    if( (hk_packet_succes = snprintf(VC0706_HkTelemetryPkt.vc0706_filename, 15, "%.*s", 15, (char *)pic_file_name+12)) < 0 ) // only use the filename, not path.
		    {
		    	OS_printf("VC0706: ERROR: Failed to write Picture Filename to HK Packet. Attempted to send: '%.*s'\n\tstrncpy returned: '%d'\n", 15, (char * )&pic_file_name[12], hk_packet_succes);
		    	// continue
		    }
		    else
		    {
			OS_printf("VC0706: Wrote Picture Filename to HK Packet. Sent: '%.*s'\n", 15, (char * )&pic_file_name[12], hk_packet_succes);
		    }
		    OS_printf("VC0706: VC0706_HkTelemetryPkt.vc0706_filename: '%s'\n", VC0706_HkTelemetryPkt.vc0706_filename);

		    /*
		    ** incriment num pics for filename
		    */
		    num_pics_stored++;
		} // else continue

    } /* Infinite Camera capture Loop End Here */

    return(0);
}
