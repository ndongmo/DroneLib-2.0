/*!
* \file Constants.h
* \brief Define needed constants.
* \author Ndongmo Silatsa
* \date 25-04-2022
* \version 2.0
*/

#pragma once

/* Drone config variables */
#define DRONE_PORT_DISCOVERY_DEFAULT 	4444
#define DRONE_IPV4_ADDRESS_DEFAULT 	    "192.168.2.1"

/* Drone config variables */
#define DRONE_ADDRESS 		    "drone_address"
#define DRONE_PORT_DISCOVERY 	"drone_port_discovery"
#define DRONE_PORT_RCV 		    "drone_port_rcv"
#define DRONE_PORT_SEND	        "drone_port_send"
#define DRONE_PORT_VIDEO 	    "drone_port_video"
#define DRONE_PORT_AUDIO	    "drone_port_audio"

/* Controller config variables */
#define CTRL_PORT_RCV 		    "ctrl_port_rcv"
#define CTRL_PORT_SEND 		    "ctrl_port_send"
#define CTRL_PORT_VIDEO 	    "ctrl_port_video"
#define CTRL_PORT_AUDIO		    "ctrl_port_audio"
#define CTRL_FPS	            "ctrl_fps"
#define CTRL_SCREEN_WIDTH	    "ctrl_screen_width"
#define CTRL_SCREEN_HEIGHT	    "ctrl_screen_height"

/* Controller config variables */
#define NET_FRAGMENT_SIZE 		    "net_fragment_size"
#define NET_FRAGMENT_NUMBER		    "net_fragment_number"
#define STREAM_FRAGMENT_ACKED 		"stream_fragment_acked"
#define STREAM_FRAGMENT_SIZE 		"stream_fragment_size"
#define STREAM_FRAGMENT_PER_FRAME   "stream_fragment_per_frame"

// NS_FRAME_TYPE
#define NS_FRAME_TYPE_UNINITIALIZED 0
#define NS_FRAME_TYPE_ACK 1
#define NS_FRAME_TYPE_DATA 2
#define NS_FRAME_TYPE_DATA_LOW_LATENCY 3
#define NS_FRAME_TYPE_DATA_WITH_ACK 4
#define NS_FRAME_TYPE_MAX 5

// NS_MANAGER_INTERNAL_BUFFER_ID
#define NS_MANAGER_INTERNAL_BUFFER_ID_PING 0
#define NS_MANAGER_INTERNAL_BUFFER_ID_PONG 1
#define NS_MANAGER_INTERNAL_BUFFER_ID_MAX 3

// NS_STREAM
#define NS_STREAM_ACK 1 // ack stream data by default