package ns.dronelib.android;

import androidx.annotation.NonNull;
import androidx.annotation.OptIn;
import androidx.appcompat.app.AppCompatActivity;
import androidx.media3.common.AudioAttributes;
import androidx.media3.common.C;
import androidx.media3.common.MediaItem;
import androidx.media3.common.MimeTypes;
import androidx.media3.common.PlaybackException;
import androidx.media3.common.Player;
import androidx.media3.common.util.UnstableApi;
import androidx.media3.datasource.ByteArrayDataSource;
import androidx.media3.datasource.DataSource;
import androidx.media3.datasource.DefaultHttpDataSource;
import androidx.media3.datasource.HttpDataSource;
import androidx.media3.datasource.UdpDataSource;
import androidx.media3.datasource.rtmp.RtmpDataSource;
import androidx.media3.exoplayer.DefaultLoadControl;
import androidx.media3.exoplayer.ExoPlayer;
import androidx.media3.exoplayer.dash.DashMediaSource;
import androidx.media3.exoplayer.hls.HlsMediaSource;
import androidx.media3.exoplayer.rtsp.RtspMediaSource;
import androidx.media3.exoplayer.smoothstreaming.SsMediaSource;
import androidx.media3.exoplayer.source.DefaultMediaSourceFactory;
import androidx.media3.exoplayer.source.MediaSource;
import androidx.media3.exoplayer.source.ProgressiveMediaSource;
import androidx.media3.exoplayer.upstream.DefaultAllocator;
import androidx.media3.ui.PlayerView;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.ImageButton;
import android.widget.TextView;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

public class MainActivity extends AppCompatActivity implements IMessageListener {

    private final static int MESSAGE_UPDATE_DELAY = 1000;
    private final static int NATIVE_MESSAGE = 1;
    private final static int JAVA_MESSAGE = 2;
    private final static String TAG = "MainActivity";
    private TextView tvFps, tvBattery, tvMessage;
    private ImageButton btLeft, btRight, btUp, btDown;
    private ImageButton btX, btY, btA, btB;
    private ImageButton btStart, btStop, btConfig;
    private ExoPlayer m_videoPlayer;
    private boolean m_isRunning;
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
            }
        }
    };
    private final Handler m_updater = new Handler();
    private final Runnable m_updateProcess = new Runnable() {
        @Override
        public void run() {
            update(getMessageFromJNI(), getBatteryFromJNI(), getStateFromJNI() ? 1 : 0);
            if(m_isRunning) {
                m_updater.postDelayed(this, MESSAGE_UPDATE_DELAY);
            }
        }
    };

    @OptIn(markerClass = UnstableApi.class)
    @SuppressLint("ClickableViewAccessibility")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initFromJNI();
        this.requestPermissions(new String[] {Manifest.permission.READ_EXTERNAL_STORAGE}, 1);
        this.requestPermissions(new String[] {Manifest.permission.INTERNET}, 1);

        m_videoPlayer = new ExoPlayer.Builder(this).build();
        m_videoPlayer.setMediaSource(new DefaultMediaSourceFactory(new RtmpDataSource.Factory())
                .createMediaSource(new MediaItem.Builder().setUri(Uri.parse("rtmp://192.168.0.240:4444/stream")).build()));
        m_videoPlayer.addListener(new PlayerListener());
        m_videoPlayer.setPlayWhenReady(true);
        m_videoPlayer.prepare();

        PlayerView playerView = findViewById(R.id.video_view);
        playerView.setPlayer(m_videoPlayer);
        //playerView.setUseController(false);

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
        if(!m_isRunning && startFromJNI()) {
            boolean motors_active = JNIHelper.getInt(JNIHelper.MOTORS_ACTIVE) == 1;
            boolean servos_active = JNIHelper.getInt(JNIHelper.SERVOS_ACTIVE) == 1;

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

            tvFps.setText(this.getString(R.string.fps, JNIHelper.getInt(JNIHelper.VIDEO_FPS)));
            tvMessage.setText("");
            m_isRunning = true;
            m_updater.postDelayed(m_updateProcess, MESSAGE_UPDATE_DELAY);

            if(!m_videoPlayer.isPlaying()) {
                m_videoPlayer.setPlayWhenReady(true);
                m_videoPlayer.prepare();
            }
        }
    }

    /**
     * Stop the streaming process.
     */
    public void stop() {
        if(m_isRunning) {
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

            m_isRunning = false;

            m_videoPlayer.stop();
            m_videoPlayer.release();
        }
    }

    /**
     * Update the error message if needed update the battery life and fps value.
     *
     */
    public void update(String msg, int batLife, int state) {
        if(m_isRunning) {
            m_updateHandler.obtainMessage(NATIVE_MESSAGE, batLife, state, msg).sendToTarget();
        }
    }

    /**
     * A native method to initialize the environment and config.
     */
    public native void initFromJNI();

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

    @UnstableApi public class VideoDataSourceFactory implements DataSource.Factory {
        public ByteArrayDataSource mydatasource;
        private CustomVideoDataSource customDatasource;
        public VideoDataSource datasource = new VideoDataSource(MainActivity.this);
        public VideoDataSourceFactory() {
            File file = new File("/sdcard/Movies/output.mp4");
            int size = (int) file.length();
            byte[] data = new byte[size];
            try {
                BufferedInputStream buf = new BufferedInputStream(new FileInputStream(file));
                buf.read(data, 0, data.length);
                buf.close();
            } catch (FileNotFoundException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            mydatasource = new ByteArrayDataSource(data);
            customDatasource = new CustomVideoDataSource(MainActivity.this, data);
        }

        @NonNull
        @Override
        public DataSource createDataSource() {
            return datasource;
        }
    }
}
