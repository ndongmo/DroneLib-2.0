package ns.dronelib.android;

import androidx.annotation.OptIn;
import androidx.appcompat.app.AppCompatActivity;
import androidx.media3.common.MediaItem;
import androidx.media3.common.PlaybackException;
import androidx.media3.common.Player;
import androidx.media3.common.util.UnstableApi;
import androidx.media3.datasource.HttpDataSource;
import androidx.media3.datasource.rtmp.RtmpDataSource;
import androidx.media3.exoplayer.ExoPlayer;
import androidx.media3.exoplayer.source.DefaultMediaSourceFactory;
import androidx.media3.ui.PlayerView;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageButton;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;

public class MainActivity extends AppCompatActivity implements IMessageListener {

    private final static int MESSAGE_UPDATE_DELAY = 1000;
    private final static int NATIVE_MESSAGE = 1;
    private final static int JAVA_MESSAGE = 2;
    private final static int APP_RUNNING_STATE = 2;
    private final static String TAG = "MainActivity";
    private TextView tvFps, tvBattery, tvMessage;
    private ImageButton btLeft, btRight, btUp, btDown;
    private ImageButton btX, btY, btA, btB;
    private ImageButton btStart, btStop, btConfig;
    private ExoPlayer m_videoPlayer;
    private boolean m_started, m_running;
    private int m_state = 0;
    private int m_eventID = -1;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    /**
     * Update fps, battery and message on screen.
     */
    private final Handler m_updateHandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(Message msg) {
            if(msg.obj != null && !msg.obj.toString().isEmpty()) {
                tvMessage.setText(msg.obj.toString());
            }
            if(msg.what == NATIVE_MESSAGE) {
                tvBattery.setText(MainActivity.this.getString(R.string.battery, msg.arg1));
                if(m_eventID != -1) {
                    pressEventFromJNI(m_eventID);
                }
                if(msg.arg2 == 0) {
                    stop();
                }
                else if(!m_running && getServerStateFromJNI() == APP_RUNNING_STATE) {
                    new Handler(Looper.getMainLooper()).post(() -> {
                        run();
                    });

                }
            }
        }
    };
    private final Handler m_updater = new Handler();
    private final Runnable m_updateProcess = new Runnable() {
        @Override
        public void run() {
            update(getMessageFromJNI(), getBatteryFromJNI(), getStateFromJNI() ? 1 : 0);
            if(m_started) {
                m_updater.postDelayed(this, MESSAGE_UPDATE_DELAY);
            }
        }
    };

    @OptIn(markerClass = UnstableApi.class)
    @SuppressLint("ClickableViewAccessibility")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_main);

        initFromJNI(this.getFilesDir().getAbsolutePath());
        this.requestPermissions(new String[] {Manifest.permission.INTERNET}, 1);

        m_videoPlayer = new ExoPlayer.Builder(this).build();
        m_videoPlayer.addListener(new PlayerListener());
        m_videoPlayer.setPlayWhenReady(false);
        m_videoPlayer.prepare();

        PlayerView playerView = findViewById(R.id.video_view);
        playerView.setPlayer(m_videoPlayer);
        playerView.setUseController(false);

        tvFps = findViewById(R.id.txt_fps);
        tvBattery = findViewById(R.id.txt_bat);
        tvMessage = findViewById(R.id.txt_error);
        btLeft = findViewById(R.id.bt_left);
        btRight = findViewById(R.id.bt_right);
        btUp = findViewById(R.id.bt_up);
        btDown = findViewById(R.id.bt_down);
        btX = findViewById(R.id.bt_x);
        btY = findViewById(R.id.bt_y);
        btA = findViewById(R.id.bt_a);
        btB = findViewById(R.id.bt_b);
        btStart = findViewById(R.id.bt_start);
        btStop = findViewById(R.id.bt_stop);
        btConfig = findViewById(R.id.bt_config);

        tvFps.setText(this.getString(R.string.fps, JNIHelper.getInt(JNIHelper.VIDEO_FPS)));
        tvBattery.setText(this.getString(R.string.battery, 0));

        btLeft.setEnabled(false);
        btRight.setEnabled(false);
        btUp.setEnabled(false);
        btDown.setEnabled(false);
        btX.setEnabled(false);
        btY.setEnabled(false);
        btA.setEnabled(false);
        btB.setEnabled(false);
        btStop.setEnabled(false);

        btLeft.setOnTouchListener(new TouchListener(JNIHelper.GO_LEFT));
        btRight.setOnTouchListener(new TouchListener(JNIHelper.GO_RIGHT));
        btUp.setOnTouchListener(new TouchListener(JNIHelper.GO_UP));
        btDown.setOnTouchListener(new TouchListener(JNIHelper.GO_DOWN));
        btX.setOnTouchListener(new TouchListener(JNIHelper.CAM_LEFT));
        btY.setOnTouchListener(new TouchListener(JNIHelper.CAM_UP));
        btA.setOnTouchListener(new TouchListener(JNIHelper.CAM_RIGHT));
        btB.setOnTouchListener(new TouchListener(JNIHelper.CAM_DOWN));

        btStart.setOnClickListener(view -> {
            start();
        });

        btStop.setOnClickListener(view -> {
            stop();
        });

        btConfig.setOnClickListener(view -> {
            startActivity(new Intent(getApplicationContext(),ConfigActivity.class));
        });
    }

    @Override
    protected void onPause() {
        super.onPause();
        stop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stop();
        if(m_videoPlayer != null) {
            m_videoPlayer.stop();
            m_videoPlayer.release();
        }
    }

    @Override
    public void showMessage(String msg) {
        m_updateHandler.obtainMessage(JAVA_MESSAGE, msg).sendToTarget();
    }

    /**
     * Start the streaming process.
     */
    public void start() {
        if(!m_started && startFromJNI()) {
            m_started = true;
            m_updater.postDelayed(m_updateProcess, MESSAGE_UPDATE_DELAY);
        }
    }

    /**
     * Start the streaming process.
     */
    private void run() {
        if(m_started && !m_running) {
            boolean motors_active = JNIHelper.getInt(JNIHelper.MOTORS_ACTIVE) == 1;
            boolean servos_active = JNIHelper.getInt(JNIHelper.SERVOS_ACTIVE) == 1;

            m_running = true;
            btLeft.setEnabled(motors_active);
            btRight.setEnabled(motors_active);
            btUp.setEnabled(motors_active);
            btDown.setEnabled(motors_active);
            btX.setEnabled(servos_active);
            btY.setEnabled(servos_active);
            btA.setEnabled(servos_active);
            btB.setEnabled(servos_active);
            btStop.setEnabled(true);
            btStart.setEnabled(false);
            btConfig.setEnabled(false);

            tvMessage.setText("");
            tvFps.setText(this.getString(R.string.fps, JNIHelper.getInt(JNIHelper.VIDEO_FPS)));

            if(!m_videoPlayer.isPlaying()) {
                Uri uri = Uri.parse(JNIHelper.getString(JNIHelper.STREAM_OUT_FILE_ADDRESS));
                if(uri != null && uri.toString().startsWith("rtmp://")) {
                    m_videoPlayer.setMediaSource(new DefaultMediaSourceFactory(new RtmpDataSource.Factory())
                            .createMediaSource(new MediaItem.Builder().setUri(uri).build()));
                    m_videoPlayer.setPlayWhenReady(true);
                    m_videoPlayer.prepare();
                    Log.d(TAG, "Uri: " + uri);
                } else {
                    Log.e(TAG, "Client received wrong uri: " + uri);
                }
            }
        }
    }

    /**
     * Stop the streaming process.
     */
    public void stop() {
        if(m_started) {
            stopFromJNI();

            btLeft.setEnabled(false);
            btRight.setEnabled(false);
            btUp.setEnabled(false);
            btDown.setEnabled(false);
            btX.setEnabled(false);
            btY.setEnabled(false);
            btA.setEnabled(false);
            btB.setEnabled(false);
            btStop.setEnabled(false);
            btStart.setEnabled(true);
            btConfig.setEnabled(true);
            tvMessage.setText("");

            m_started = false;
            m_running = false;

            m_videoPlayer.stop();
            m_updater.removeCallbacks(m_updateProcess);
        }
    }

    /**
     * Update the error message if needed update the battery life and fps value.
     *
     */
    public void update(String msg, int batLife, int state) {
        if(m_started) {
            m_updateHandler.obtainMessage(NATIVE_MESSAGE, batLife, state, msg).sendToTarget();
        }
    }

    /**
     * A native method to initialize the environment and config.
     *
     * @param fileDir the app file directory
     */
    public native void initFromJNI(String fileDir);

    /**
     * A native method to start the client controller service.
     */
    public native boolean startFromJNI();

    /**
     * A native method to stop the client controller service.
     */
    public native void stopFromJNI();

    /**
     * A native method to retrieve the battery level from the client controller service.
     */
    public native int getBatteryFromJNI();

    /**
     * A native method to retrieve the current debug message from the client controller service.
     */
    public native String getMessageFromJNI();

    /**
     * A native method to retrieve the running state of the client controller service.
     */
    public native boolean getStateFromJNI();

    /**
     * A native method to retrieve the current state of the client controller service.
     */
    public native int getServerStateFromJNI();

    /**
     * A native method for sending a pressed event to the controller.
     *
     * @param eventID the event to send
     */
    public native void pressEventFromJNI(int eventID);

    /**
     * A native method for sending a released event to the controller.
     *
     * @param eventID the event to send
     */
    public native void releaseEventFromJNI(int eventID);

    class TouchListener implements View.OnTouchListener {
        private final int m_currentEventID;
        public TouchListener(int eventID) {
            m_currentEventID = eventID;
        }
        @Override
        public boolean onTouch(View view, MotionEvent ev) {
            if(ev.getAction() == MotionEvent.ACTION_DOWN) {
                m_eventID = m_currentEventID;
                pressEventFromJNI(m_currentEventID);
            }
            else if(ev.getAction() == MotionEvent.ACTION_UP) {
                m_eventID = -1;
                releaseEventFromJNI(m_currentEventID);
            }
            return true;
        }
    }

    class PlayerListener implements Player.Listener {
        @Override
        public void onIsPlayingChanged(boolean isPlaying) {
            Log.d(TAG, "onIsPlayingChanged isPlaying: " + isPlaying);
        }
        @Override
        public void onPlayerError(PlaybackException error) {
            Throwable cause = error.getCause();
            if (cause instanceof HttpDataSource.HttpDataSourceException) {
                HttpDataSource.HttpDataSourceException httpError = (HttpDataSource.HttpDataSourceException) cause;
                if (httpError instanceof HttpDataSource.InvalidResponseCodeException) {
                    HttpDataSource.InvalidResponseCodeException httpException = (HttpDataSource.InvalidResponseCodeException) httpError;
                    Log.e(TAG, "onPlayerError: responseCode:" + httpException.responseCode +
                            " responseMessage: " + httpException.responseMessage);
                }
            }
            Log.e(TAG, "onPlayerError: " + error.getMessage(), cause);
        }
    }
}
