/*!
* \file Constants.h
* \brief Define drone constants.
* \author Ndongmo Silatsa
* \date 12-05-2022
* \version 2.0
*/

#pragma once

/* Drone config variables */
#define DRONE_PORT_RCV_DEFAULT 		    4440
#define DRONE_PORT_SEND_DEFAULT	        4441
#define DRONE_PORT_VIDEO_DEFAULT	    4442
#define DRONE_PORT_AUDIO_DEFAULT	    4443

/* Fragment default config */
#define NET_FRAGMENT_SIZE_DEFAULT		1024
#define NET_FRAGMENT_NUMBER_DEFAULT	    128

#define MOTOR_MOVE_DELAY                100000 // Delay of a motor move in micro second 1000000 = 1s