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
#define DRONE_IPV4_ADDRESS_DEFAULT 	    "192.168.0.242"
#define DRONE_WHEEL_MOVE_LAPS           100 // Time laps of wheel move in millisecond

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
#define NS_FRAME_TYPE_QUIT              0
#define NS_FRAME_TYPE_ACK               1
#define NS_FRAME_TYPE_DATA              2
#define NS_FRAME_TYPE_DATA_LOW_LATENCY  3
#define NS_FRAME_TYPE_DATA_WITH_ACK     4
#define NS_FRAME_TYPE_MAX               5

// Command IDs
#define NS_ID_EMPTY     0   // empty cmd id
#define NS_ID_PING      1   // ping cmd id
#define NS_ID_PONG      2   // pong cmd id
#define NS_ID_MAX       3   // max cmd id
#define NS_ID_NAV       4   // navigation cmd id

// Command frequencies in millisecond
#define NS_FREQ_PING  50                        // ping cmd frequency
#define NS_FREQ_NAV   DRONE_WHEEL_MOVE_LAPS / 2 // navigation cmd frequency

// NS_STREAM
#define NS_STREAM_ACK 1 // ack stream data by default

// Error codes
#define ERROR_NET_SEND          1   // error sending data through network
#define ERROR_NET_RECEIVE       2   // error reading received data through network
#define ERROR_NET_DISCOVERY     3   // error discovering network
#define ERROR_NET_INIT          4   // error initializing network

// Controller commands
#define CMD_CTRL_QUIT 1
#define CMD_CTRL_DISCOVER 2