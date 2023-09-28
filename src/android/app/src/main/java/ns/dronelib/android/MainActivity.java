package ns.dronelib.android;

import androidx.appcompat.app.AppCompatActivity;
import androidx.annotation.VisibleForTesting;

import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback, IMessageListener {

    private StreamMediaPlayer m_videoPlayer;
    private StreamMediaPlayer m_audioPlayer;
    private boolean m_isPaused;

    SurfaceView m_surfaceView;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        m_videoPlayer.setDisplay(holder);

        if(!m_isPaused) {
            m_videoPlayer.init();
            m_audioPlayer.init();
        } else {
            m_videoPlayer.start();
            m_audioPlayer.start();
        }

        m_isPaused = false;
    }

    @Override
    protected void onPause() {
        super.onPause();
        if(!m_isPaused) {
            m_videoPlayer.pause();
            m_audioPlayer.pause();
            m_isPaused = true;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if(m_videoPlayer != null) {
            m_videoPlayer.stop();
            m_videoPlayer.release();
        }
        if(m_audioPlayer != null) {
            m_audioPlayer.stop();
            m_audioPlayer.release();
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    @Override
    public void showMessage(String msg) {

    }

    /**
     * Update the error message if needed update the battery life and fps value.
     *
     */
    public void update(String msg, int batLife, int fps) {

    }

    /**
     * A native method to start the client controller service.
     */
    public native boolean startFromJNI();

    /**
     * A native method to stop the client controller service.
     */
    public native void stopFromJNI();

    /**
     * A native method for sending a given event to the controller.
     *
     * @param eventID the event to send
     */
    public native void updateEventFromJNI(int eventID);

    /**
     * A native method to get the last received video frame.
     *
     * @return the received video frame
     */
    public native byte[] updateVideoFromJNI();

    /**
     * A native method to get the last received audio frame.
     *
     * @return the received audio frame
     */
    public native byte[] updateAudioFromJNI();
}
