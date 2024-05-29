package ns.dronelib.android;

import android.net.Uri;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.media3.common.C;
import androidx.media3.common.util.UnstableApi;
import androidx.media3.datasource.BaseDataSource;
import androidx.media3.datasource.DataSpec;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

@UnstableApi public class CustomVideoDataSource extends BaseDataSource {
    private IMessageListener m_listener;
    private int m_length = 0;
    private int m_position = 0;
    private byte[] m_data = null;

    public CustomVideoDataSource(IMessageListener listener, byte[] data) {
        super(true);
        m_listener = listener;
        m_data = data;
    }

    @Override
    public long open(DataSpec dataSpec) throws IOException {
        Log.d("VideoDataSource", "open ");
        m_position = (int) dataSpec.position;
        Log.d("VideoDataSource", "open position=" + m_position);
        return C.LENGTH_UNSET;
    }

    @Nullable
    @Override
    public Uri getUri() {
        return Uri.EMPTY;
    }

    @Override
    public synchronized void close() throws IOException {
        Log.d("VideoDataSource", "close ");
    }

    @Override
    public synchronized int read(@NonNull byte[] buffer, int offset, int size) throws IOException {
        if(m_length == 0) {
            //m_data = getVideoFromJNI();
            m_length = m_data != null ? m_data.length : 0;
            m_position = 0;
            if(m_data != null && m_length > 0) {
                //Arrays.fill(m_data, (byte) 1 );
                Log.d("VideoDataSource", "read -> data:" + m_length + " pos:" + m_position + " size: " + size + " offset:" + offset + " buffer:" + buffer.length);
            }
        }
        else if (m_position == m_length) {
            Log.d("VideoDataSource", "read -> end");
            return C.RESULT_END_OF_INPUT;
        }
        if(m_length > m_position && m_data != null) {
            if (m_position + size > m_length) {
                size = m_length - m_position;
            }
            System.arraycopy(m_data, m_position, buffer, offset, size);
            m_position += size;
            return size;
        }
        else {
            return 0;
        }
    }
}
