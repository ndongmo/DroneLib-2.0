package ns.dronelib.android;

import android.os.Bundle;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class ConfigActivity extends AppCompatActivity {

    private EditText etDroneAddress;
    private EditText etDronePort;
    private EditText etVideoFps;
    private EditText etVideoWidth;
    private EditText etVideoHeight;
    private EditText etAudioChannels;
    private EditText etAudioSample;
    private EditText etAudioSamples;
    private CheckBox chbCamera;
    private CheckBox chbMicro;
    private CheckBox chbServos;
    private CheckBox chbMotors;
    private CheckBox chbLeds;
    private CheckBox chbBuzzer;
    private CheckBox chbBattery;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_config);

        etDroneAddress = findViewById(R.id.et_drone_address);
        etDronePort = findViewById(R.id.et_drone_port);
        etVideoFps = findViewById(R.id.et_video_fps);
        etVideoWidth = findViewById(R.id.et_video_width);
        etVideoHeight = findViewById(R.id.et_video_height);
        etAudioChannels = findViewById(R.id.et_audio_channels);
        etAudioSample = findViewById(R.id.et_audio_sample);
        etAudioSamples = findViewById(R.id.et_audio_samples);
        chbCamera = findViewById(R.id.chb_camera_active);
        chbMicro = findViewById(R.id.chb_micro_active);
        chbServos = findViewById(R.id.chb_servos_active);
        chbMotors = findViewById(R.id.chb_motors_active);
        chbLeds = findViewById(R.id.chb_leds_active);
        chbBuzzer = findViewById(R.id.chb_buzzer_active);
        chbBattery = findViewById(R.id.chb_battery_active);
        ImageButton btSave = findViewById(R.id.bt_save);
        ImageButton btReset = findViewById(R.id.bt_reset);

        btSave.setOnClickListener(view -> {
            saveConfig();
        });

        btReset.setOnClickListener(view -> {
            etDroneAddress.setText(JNIHelper.DRONE_ADDRESS_DEFAULT);
            etDronePort.setText(JNIHelper.DRONE_PORT_DEFAULT);
            etVideoFps.setText(JNIHelper.VIDEO_FPS_DEFAULT);
            etVideoWidth.setText(JNIHelper.VIDEO_WIDTH_DEFAULT);
            etVideoHeight.setText(JNIHelper.VIDEO_HEIGHT_DEFAULT);
            etAudioChannels.setText(JNIHelper.AUDIO_CHANNELS_DEFAULT);
            etAudioSample.setText(JNIHelper.AUDIO_SAMPLE_DEFAULT);
            etAudioSamples.setText(JNIHelper.AUDIO_NB_SAMPLES_DEFAULT);

            chbCamera.setChecked(JNIHelper.CAMERA_ACTIVE_DEFAULT);
            chbMicro.setChecked(JNIHelper.MICRO_ACTIVE_DEFAULT);
            chbServos.setChecked(JNIHelper.SERVOS_ACTIVE_DEFAULT);
            chbMotors.setChecked(JNIHelper.MOTORS_ACTIVE_DEFAULT);
            chbLeds.setChecked(JNIHelper.LEDS_ACTIVE_DEFAULT);
            chbBuzzer.setChecked(JNIHelper.BUZZER_ACTIVE_DEFAULT);
            chbBattery.setChecked(JNIHelper.BATTERY_ACTIVE_DEFAULT);

            saveConfig();
        });

        etDroneAddress.setText(JNIHelper.getString(JNIHelper.DRONE_ADDRESS));
        etDronePort.setText(String.valueOf(JNIHelper.getInt(JNIHelper.DRONE_PORT)));
        etVideoFps.setText(String.valueOf(JNIHelper.getInt(JNIHelper.VIDEO_FPS)));
        etVideoWidth.setText(String.valueOf(JNIHelper.getInt(JNIHelper.VIDEO_WIDTH)));
        etVideoHeight.setText(String.valueOf(JNIHelper.getInt(JNIHelper.VIDEO_HEIGHT)));
        etAudioChannels.setText(String.valueOf(JNIHelper.getInt(JNIHelper.AUDIO_CHANNELS)));
        etAudioSample.setText(String.valueOf(JNIHelper.getInt(JNIHelper.AUDIO_SAMPLE)));
        etAudioSamples.setText(String.valueOf(JNIHelper.getInt(JNIHelper.AUDIO_NB_SAMPLES)));

        chbCamera.setChecked(JNIHelper.getInt(JNIHelper.CAMERA_ACTIVE) == 1);
        chbMicro.setChecked(JNIHelper.getInt(JNIHelper.MICRO_ACTIVE) == 1);
        chbServos.setChecked(JNIHelper.getInt(JNIHelper.SERVOS_ACTIVE) == 1);
        chbMotors.setChecked(JNIHelper.getInt(JNIHelper.MOTORS_ACTIVE) == 1);
        chbLeds.setChecked(JNIHelper.getInt(JNIHelper.LEDS_ACTIVE) == 1);
        chbBuzzer.setChecked(JNIHelper.getInt(JNIHelper.BUZZER_ACTIVE) == 1);
        chbBattery.setChecked(JNIHelper.getInt(JNIHelper.BATTERY_ACTIVE) == 1);
    }

    /**
     * Save the current config.
     */
    private void saveConfig() {
        JNIHelper.setString(JNIHelper.DRONE_ADDRESS, etDroneAddress.getText().toString());
        JNIHelper.setInt(JNIHelper.DRONE_PORT, Integer.parseInt(etDronePort.getText().toString()));
        JNIHelper.setInt(JNIHelper.VIDEO_FPS, Integer.parseInt(etVideoFps.getText().toString()));
        JNIHelper.setInt(JNIHelper.VIDEO_WIDTH, Integer.parseInt(etVideoWidth.getText().toString()));
        JNIHelper.setInt(JNIHelper.VIDEO_HEIGHT, Integer.parseInt(etVideoHeight.getText().toString()));
        JNIHelper.setInt(JNIHelper.AUDIO_CHANNELS, Integer.parseInt(etAudioChannels.getText().toString()));
        JNIHelper.setInt(JNIHelper.AUDIO_SAMPLE, Integer.parseInt(etAudioSample.getText().toString()));
        JNIHelper.setInt(JNIHelper.AUDIO_NB_SAMPLES, Integer.parseInt(etAudioSamples.getText().toString()));

        JNIHelper.setInt(JNIHelper.CAMERA_ACTIVE, (chbCamera.isChecked() ? 1 : 0));
        JNIHelper.setInt(JNIHelper.MICRO_ACTIVE, (chbMicro.isChecked() ? 1 : 0));
        JNIHelper.setInt(JNIHelper.SERVOS_ACTIVE, (chbServos.isChecked() ? 1 : 0));
        JNIHelper.setInt(JNIHelper.MOTORS_ACTIVE, (chbMotors.isChecked() ? 1 : 0));
        JNIHelper.setInt(JNIHelper.LEDS_ACTIVE, (chbLeds.isChecked() ? 1 : 0));
        JNIHelper.setInt(JNIHelper.BUZZER_ACTIVE, (chbBuzzer.isChecked() ? 1 : 0));
        JNIHelper.setInt(JNIHelper.BATTERY_ACTIVE, (chbBattery.isChecked() ? 1 : 0));

        JNIHelper.saveConfig();

        Toast.makeText(this, R.string.save_success, Toast.LENGTH_SHORT).show();
    }
}