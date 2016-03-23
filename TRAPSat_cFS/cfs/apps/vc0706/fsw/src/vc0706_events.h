/************************************************************************
** File:
**    vc0706_events.h
**
** Purpose:
**  Define VC0706 App Events IDs
**
** Notes:
**
**
*************************************************************************/
#ifndef _vc0706_events_h_
#define _vc0706_events_h_

/*
** Main Task Event IDs
*/

#define VC0706_RESERVED_EID              0
#define VC0706_STARTUP_INF_EID           1
#define VC0706_COMMAND_ERR_EID           2
#define VC0706_COMMANDNOP_INF_EID        3
#define VC0706_COMMANDRST_INF_EID        4
#define VC0706_INVALID_MSGID_ERR_EID     5
#define VC0706_LEN_ERR_EID               6 // used for checking ingress message length, and reply from camera length

#define VC0706_REPLY_ERR_EID			 9
// Will need commands for switch camera
// possibly take specific picture or
// just stop taking picture from specific
// camera, something to discuss.


/*
** Child Task Event IDs
*/
#define VC0706_CHILD_INIT_ERR_EID	7
#define VC0706_CHILD_INIT_EID		8

#endif /* _vc0706_events_h_ */

/************************/
/*  End of File Comment */
/************************/
