/*!
* \file Constants.h
* \brief Define needed constants.
* \author Ndongmo Silatsa
* \date 25-04-2022
* \version 2.0
*/

#pragma once

#define MAX_FPS                 60      // camera rotation increment value
#define CAMERA_ROTATION_ANGLE   5       // camera rotation increment value
#define MOTORS_MOVE_LAPS        100     // Time laps of wheel move in millisecond
#define SERVOS_MOVE_LAPS        25      // Time laps of servo rotation in millisecond
#define SERVOS_MOVE_APL         2       // Servo rotation value per laps
#define BATTERY_LAPS            1000    // Battery update time laps in millisecond
#define BATTERY_VOLT_MAX        8.4f    // Battery maximum voltage
#define BUZZER_LAPS             500     // Buzzer time laps in millisecond

/* Different stream modes */
#define STREAM_MODE_RAW 0       // Raw packets are sent through the current udp connection
#define STREAM_MODE_ENCODED 1   // Encoded packets are sent through the current udp connection
#define STREAM_MODE_FILE 2      // Encoded packets are saved as file or sent through a new streaming address

/* Drone config default values */
#define DRONE_PORT_DISCOVERY_DEFAULT 	4445
#define DRONE_IPV4_ADDRESS_DEFAULT 	    "192.168.0.241"

/* Drone config variables */
#define DRONE_ADDRESS 		    "drone_address"
#define DRONE_PORT_DISCOVERY 	"drone_port_discovery"
#define DRONE_PORT_RCV 		    "drone_port_rcv"
#define DRONE_PORT_SEND	        "drone_port_send"
#define DRONE_PORT_STREAM	    "drone_port_stream"

/* Client config variables */
#define CLIENT_ADDRESS 	        "client_address"
#define CLIENT_PORT_RCV 	    "client_port_rcv"
#define CLIENT_PORT_SEND 	    "client_port_send"

/* Client config variables */
#define NET_FRAGMENT_SIZE 		    "net_fragment_size"
#define NET_FRAGMENT_NUMBER		    "net_fragment_number"
#define STREAM_FRAGMENT_SIZE 		"stream_fragment_size"
#define STREAM_FRAGMENT_PER_FRAME   "stream_fragment_per_frame"

/* Client default config values */
#define CLIENT_PORT_RCV_DEFAULT 		    4450
#define CLIENT_PORT_SEND_DEFAULT 		    4451

/* Stream config variables */
#define STREAM_MODE             "stream_mode"
#define STREAM_PROTOCOL         "stream_protocol"
#define STREAM_OUTPUT_FORMAT    "stream_output_format"
#define STREAM_OUT_FILE_ADDRESS "stream_out_file_address"

/* Stream default config values */
#define STREAM_MODE_DEFAULT             STREAM_MODE_RAW
#define STREAM_PROTOCOL_DEFAULT         "udp"
#define STREAM_OUTPUT_FORMAT_DEFAULT    "mpegts"

/* Video source config variables */
#define VIDEO_ENCODER       "video_encoder"
#define VIDEO_PIX_FORMAT    "video_pix_format"
#define VIDEO_BIT_RATE      "video_bit_rate"
#define VIDEO_FPS           "video_fps" // Frame per second
#define VIDEO_WIDTH         "video_width"
#define VIDEO_HEIGHT        "video_height"
#define VIDEO_DEVICE        "video_device"
#define VIDEO_INPUT_FORMAT  "video_input_format"

/* Video source config vairiables default values */
#define VIDEO_ENCODER_DEFAULT       ""
#define VIDEO_PIX_FORMAT_DEFAULT    ""          // https://ffmpeg.org/doxygen/trunk/pixfmt_8h_source.html
#define VIDEO_BIT_RATE_DEFAULT      400000
#define VIDEO_WIDTH_DEFAULT         640
#define VIDEO_HEIGHT_DEFAULT        480
#define VIDEO_FPS_DEFAULT           30
#ifdef _WIN32 
/* Cmd to list capture devices on Window: ffmpeg -list_devices true -f dshow -i any */
#define VIDEO_DEVICE_DEFAULT        "video=EasyCamera"
#define VIDEO_INPUT_FORMAT_DEFAULT  "dshow"
#else
#define VIDEO_DEVICE_DEFAULT        "/dev/video0"
#define VIDEO_INPUT_FORMAT_DEFAULT  "v4l2"
#endif

/* Audio source config variables */
#define AUDIO_ENCODER       "audio_encoder"
#define AUDIO_SAMPLE        "audio_sample" 
#define AUDIO_NB_SAMPLES    "audio_nb_samples" 
#define AUDIO_CHANNELS      "audio_channels" 
#define AUDIO_BIT_RATE      "audio_bit_rate" 
#define AUDIO_DEVICE        "audio_device"
#define AUDIO_INPUT_FORMAT  "audio_input_format"
#define AUDIO_SAMPLE_FORMAT "audio_sample_format" 

/* Video source config vairiables default values */
#define AUDIO_ENCODER_DEFAULT       ""
#define AUDIO_SAMPLE_DEFAULT        44100
#define AUDIO_NB_SAMPLES_DEFAULT    512 // number of audio samples per channel
#define AUDIO_CHANNELS_DEFAULT      2
#define AUDIO_BIT_RATE_DEFAULT      1536
#define AUDIO_DEVICE_DEFAULT        "hw:0"
#define AUDIO_INPUT_FORMAT_DEFAULT  "alsa"
#define AUDIO_SAMPLE_FORMAT_DEFAULT -1 // https://ffmpeg.org/doxygen/trunk/group__lavu__sampfmts.html

/* Resource activation config variables */
#define CAMERA_ACTIVE               "camera_active"
#define MICRO_ACTIVE                "micro_active"
#define SERVOS_ACTIVE               "servos_active"
#define MOTORS_ACTIVE               "motors_active"
#define LEDS_ACTIVE                 "leds_active"
#define BATTERY_ACTIVE              "battery_active"
#define BUZZER_ACTIVE               "buzzer_active"

/* Resource activation config variables default values */
#define CAMERA_ACTIVE_DEFAULT       1
#define MICRO_ACTIVE_DEFAULT        1
#define SERVOS_ACTIVE_DEFAULT       1 
#define MOTORS_ACTIVE_DEFAULT       1
#define LEDS_ACTIVE_DEFAULT         1
#define BATTERY_ACTIVE_DEFAULT      1
#define BUZZER_ACTIVE_DEFAULT       1

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
#define NS_ID_BATTERY_LIFE      8   // battery life id
#define NS_ID_BUZZER            9   // buzzer play id

// Command frequencies in millisecond
#define NS_FREQ_PING            50                     // ping cmd frequency
#define NS_FREQ_NAV             MOTORS_MOVE_LAPS / 2   // navigation cmd frequency
#define NS_FREQ_CAMERA          SERVOS_MOVE_LAPS / 2   // camera rotation frequency

// Error codes
#define ERROR_NET_SEND          1   // error sending data through network
#define ERROR_NET_RECEIVE       2   // error reading received data through network
#define ERROR_NET_DISCOVERY     3   // error discovering network
#define ERROR_NET_INIT          4   // error initializing network
#define ERROR_STREAM_INIT       5   // error initializing stream

// Controller commands
#define CMD_CLIENT_QUIT 1
#define CMD_CLIENT_DISCOVER 2