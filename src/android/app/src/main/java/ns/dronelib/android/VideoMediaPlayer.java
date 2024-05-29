package ns.dronelib.android;

import android.media.AudioAttributes;
import android.media.MediaPlayer;

public class VideoMediaPlayer extends MediaPlayer {
    private VideoDataSource m_dataSource;
    private boolean m_init;

    public VideoMediaPlayer(IMessageListener listener) {
        super();
        m_dataSource = new VideoDataSource(listener);
    }

    public void init() {
        //this.setDataSource(m_dataSource);
        this.setOnPreparedListener(mp -> mp.start());
        this.prepareAsync();
        m_init = true;
    }

    public boolean isInitialized() {
        return m_init;
    }
}
