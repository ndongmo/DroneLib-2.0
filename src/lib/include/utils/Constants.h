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

/* Controller config variables */
#define CTRL_ADDRESS 	        "ctrl_address"
#define CTRL_PORT_RCV 		    "ctrl_port_rcv"
#define CTRL_PORT_SEND 		    "ctrl_port_send"

/* Controller config variables */
#define NET_FRAGMENT_SIZE 		    "net_fragment_size"
#define NET_FRAGMENT_NUMBER		    "net_fragment_number"
#define STREAM_FRAGMENT_SIZE 		"stream_fragment_size"
#define STREAM_FRAGMENT_PER_FRAME   "stream_fragment_per_frame"

/* Video source config variables */
#define VIDEO_CODEC         "video_codec"
#define VIDEO_FORMAT        "video_format"
#define VIDEO_BIT_RATE      "video_bit_rate"
#define VIDEO_FPS           "video_fps" // Frame per second
#define VIDEO_WIDTH         "video_width"
#define VIDEO_HEIGHT        "video_height"
#define VIDEO_DEVICE         "video_device"

/* Video source config vairiables default values */
#define VIDEO_CODEC_DEFAULT         13 // https://ffmpeg.org/doxygen/3.2/avcodec_8h_source.html
#define VIDEO_FORMAT_DEFAULT        1 // https://ffmpeg.org/doxygen/trunk/pixfmt_8h_source.html
#define VIDEO_BIT_RATE_DEFAULT      400000
#define VIDEO_WIDTH_DEFAULT         640
#define VIDEO_HEIGHT_DEFAULT        480
#define VIDEO_FPS_DEFAULT           60
#define VIDEO_DEVICE_DEFAULT        "/dev/video0"

/* Video destination config variables */
#define VIDEO_DST_FORMAT    "video_dst_format"
#define VIDEO_DST_WIDTH     "video_dst_width"
#define VIDEO_DST_HEIGHT    "video_dst_height"

/* Video destination config vairiables default values */
#define VIDEO_DST_FORMAT_DEFAULT    3 // https://ffmpeg.org/doxygen/trunk/pixfmt_8h_source.html
#define VIDEO_DST_WIDTH_DEFAULT     640
#define VIDEO_DST_HEIGHT_DEFAULT    480

/* Audio source config variables */
#define AUDIO_CODEC         "audio_codec"
#define AUDIO_FORMAT        "audio_format"
#define AUDIO_SAMPLE        "audio_sample" 
#define AUDIO_CHANNELS      "audio_channels" 
#define AUDIO_BIT_RATE      "audio_bit_rate" 
#define AUDIO_DEVICE        "audio_device"
#define AUDIO_INPUT_FORMAT  "audio_input_format"

/* Video source config vairiables default values */
#define AUDIO_CODEC_DEFAULT         13 // https://ffmpeg.org/doxygen/3.2/avcodec_8h_source.html
#define AUDIO_FORMAT_DEFAULT        1 // https://ffmpeg.org/doxygen/trunk/pixfmt_8h_source.html
#define AUDIO_SAMPLE_DEFAULT        44100
#define AUDIO_CHANNELS_DEFAULT      2
#define AUDIO_BIT_RATE_DEFAULT      1536
#define AUDIO_DEVICE_DEFAULT        "hw:0"
#define AUDIO_INPUT_FORMAT_DEFAULT  "alsa"

// NS_FRAME_TYPE
#define NS_FRAME_TYPE_QUIT              0
#define NS_FRAME_TYPE_ACK               1
#define NS_FRAME_TYPE_DATA              2
#define NS_FRAME_TYPE_DATA_LOW_LATENCY  3
#define NS_FRAME_TYPE_DATA_WITH_ACK     4
#define NS_FRAME_TYPE_MAX               5

// Command IDs
#define NS_ID_EMPTY             0   // empty cmd id
#define NS_ID_PING              1   // ping cmd id
#define NS_ID_PONG              2   // pong cmd id
#define NS_ID_MAX               3   // max cmd id
#define NS_ID_NAV               4   // navigation cmd id
#define NS_ID_CAMERA            5   // navigation cmd id
#define NS_ID_STREAM_AUDIO      6   // audio stream id
#define NS_ID_STREAM_VIDEO      7   // video stream id

// Command frequencies in millisecond
#define NS_FREQ_PING            50                          // ping cmd frequency
#define NS_FREQ_NAV             DRONE_WHEEL_MOVE_LAPS / 2   // navigation cmd frequency
#define NS_FREQ_CAMERA          50                          // camera rotation frequency

// Error codes
#define ERROR_NET_SEND          1   // error sending data through network
#define ERROR_NET_RECEIVE       2   // error reading received data through network
#define ERROR_NET_DISCOVERY     3   // error discovering network
#define ERROR_NET_INIT          4   // error initializing network
#define ERROR_STREAM_INIT       5   // error initializing stream

// Controller commands
#define CMD_CTRL_QUIT 1
#define CMD_CTRL_DISCOVER 2