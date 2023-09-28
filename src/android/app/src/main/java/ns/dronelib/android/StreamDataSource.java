package ns.dronelib.android;

import android.media.MediaDataSource;

import java.io.IOException;

public class StreamDataSource extends MediaDataSource {
    private IMessageListener m_listener;

    public StreamDataSource(IMessageListener listener) {
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
}
