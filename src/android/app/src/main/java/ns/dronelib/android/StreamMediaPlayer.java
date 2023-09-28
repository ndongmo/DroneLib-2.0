package ns.dronelib.android;

import android.media.MediaPlayer;

public class StreamMediaPlayer extends MediaPlayer {
    private StreamDataSource m_dataSource;

    public StreamMediaPlayer(IMessageListener listener) {
        super();
        m_dataSource = new StreamDataSource(listener);
    }
    public void init() {
        this.setDataSource(m_dataSource);
        this.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
            @Override
            public void onPrepared(MediaPlayer mp) {
                mp.start();
            }
        });
        this.prepareAsync();
    }
}
