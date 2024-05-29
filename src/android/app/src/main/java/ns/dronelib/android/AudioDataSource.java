package ns.dronelib.android;

import android.media.MediaDataSource;

import java.io.IOException;

public class AudioDataSource extends MediaDataSource {
    private IMessageListener m_listener;

    public AudioDataSource(IMessageListener listener) {
        m_listener = listener;
    }

    @Override
    public synchronized int readAt(long position, byte[] buffer, int offset, int size) throws IOException {
        return 0;
    }

    @Override
    public synchronized long getSize() throws IOException {
        return 0;
    }

    @Override
    public synchronized void close() throws IOException {

    }

    /**
     * A native method to get the last received audio frame.
     *
     * @return the received audio frame
     */
    public native byte[] getAudioFromJNI();
}
