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
#define DRONE_PORT_STREAM_DEFAULT	    4442 // video and audio mixed

/* Fragment default config */
#define NET_FRAGMENT_SIZE_DEFAULT		1400 // Ethernet/IPv4 limit is 1500 bytes, so 1400 bytes is a safe value
#define NET_FRAGMENT_NUMBER_DEFAULT	    256

#define DISCOVERY_LAPS 500  // time laps for led blinking and buzzer playing on discovering mode
