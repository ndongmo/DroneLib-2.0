package ns.dronelib.android;

/**
 * JNI class helper which provides method to get or set c++ values.
 */
public class JNIHelper {
    public static final String DRONE_ADDRESS = "drone_address";
    public static final String DRONE_PORT = "drone_port_discovery";
    public static final String VIDEO_FPS = "video_fps";
    public static final String VIDEO_WIDTH = "video_width";
    public static final String VIDEO_HEIGHT = "video_height";
    public static final String AUDIO_CHANNELS = "audio_channels";
    public static final String AUDIO_SAMPLE = "audio_sample";
    public static final String AUDIO_NB_SAMPLES = "audio_nb_samples";
    public static final String CAMERA_ACTIVE = "camera_active";
    public static final String MICRO_ACTIVE = "micro_active";
    public static final String SERVOS_ACTIVE = "servos_active";
    public static final String MOTORS_ACTIVE = "motors_active";
    public static final String LEDS_ACTIVE = "leds_active";
    public static final String BATTERY_ACTIVE = "battery_active";
    public static final String BUZZER_ACTIVE = "buzzer_active";

    public static final String DRONE_ADDRESS_DEFAULT = "192.168.0.241";
    public static final int DRONE_PORT_DEFAULT = 4444;
    public static final int VIDEO_FPS_DEFAULT = 30;
    public static final int VIDEO_WIDTH_DEFAULT = 640;
    public static final int VIDEO_HEIGHT_DEFAULT = 480;
    public static final int AUDIO_CHANNELS_DEFAULT = 2;
    public static final int AUDIO_SAMPLE_DEFAULT = 44100;
    public static final int AUDIO_NB_SAMPLES_DEFAULT = 512;
    public static final boolean CAMERA_ACTIVE_DEFAULT = true;
    public static final boolean MICRO_ACTIVE_DEFAULT = true;
    public static final boolean SERVOS_ACTIVE_DEFAULT = true;
    public static final boolean MOTORS_ACTIVE_DEFAULT = true;
    public static final boolean LEDS_ACTIVE_DEFAULT = true;
    public static final boolean BATTERY_ACTIVE_DEFAULT = true;
    public static final boolean BUZZER_ACTIVE_DEFAULT = true;

    public static final int GO_SPEED_2 = 5; // A button
    public static final int BUZZ = 11; // B button
    public static final int QUIT = 12; // Quit button
    public static final int DISCOVER = 10; // Discover button
    public static final int GO_UP = 0; // Move UP
    public static final int GO_DOWN = 1; // Move DOWN
    public static final int GO_LEFT = 2; // Move LEFT
    public static final int GO_RIGHT = 3; // Move RIGHT
    public static final int CAM_UP = 6; // Camera look UP
    public static final int CAM_DOWN = 7; // Camera look DOWN
    public static final int CAM_LEFT = 8; // Camera look LEFT
    public static final int CAM_RIGHT = 9; // Camera RIGHT

    /**
     * Get config integer value related to the given key name.
     * @param key
     * @return integer value
     */
    public static native int getInt(final String key);

    /**
     * Get config string value related to the given key name.
     * @param key
     * @return string value
     */
    public static native String getString(final String key);

    /**
     * Set config integer value related to the given key name with the given value.
     * @param key
     * @param value
     */
    public static native void setInt(final String key, final int value);

    /**
     * Set config string value related to the given key name with the given value.
     * @param key
     * @param value
     */
    public static native void setString(final String key, final String value);

    /**
     * Save the current config values.
     */
    public static native void saveConfig();
}
