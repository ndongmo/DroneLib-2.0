package ns.dronelib.android;

import android.media.MediaPlayer;

public class AudioMediaPlayer extends MediaPlayer {
    private AudioDataSource m_dataSource;

    public AudioMediaPlayer(IMessageListener listener) {
        super();
        m_dataSource = new AudioDataSource(listener);
    }
    public void init() {
        this.setDataSource(m_dataSource);
        this.setOnPreparedListener(mp -> mp.start());
        this.prepareAsync();
    }
}
