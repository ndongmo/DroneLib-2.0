#include "stream/MediaSender.h"
#include "utils/Logger.h"
#include "utils/Config.h"
#include "utils/Helper.h"
#include "utils/Constants.h"

#define HTTP_PROTOCOL_NAME "http"

using namespace utils;

namespace stream {

MediaSender::MediaSender(NetSender &sender) : 
    m_audioSender(sender), m_videoSender(sender) {
	m_name = "MediaSender";
}

int MediaSender::writePacket(AVPacket *packet) {
    std::lock_guard<std::mutex> guard(m_mutex);

    if(m_http_client) {
        avio_write(m_http_client, packet->data, packet->size);
        avio_flush(m_http_client);
        return 1;
    } else {
        return av_interleaved_write_frame(m_ofmt_ctx, packet);
    }
}

int MediaSender::startHTTP() {
    int ret;

    if ((ret = avio_accept(m_ofmt_ctx->pb, &m_http_client)) < 0) {
        logE << m_name << ": failed to accept http client -> " << av_err2str(ret) << std::endl;
        return -1;
    }
    
    while ((ret = avio_handshake(m_http_client)) > 0);

    if (ret < 0) {
        logE << m_name << ": failed the first hand shake with the http client -> " << av_err2str(ret) << std::endl;
        return -1;
    }
        
    if ((ret = av_opt_set_int(m_http_client, "reply_code", 200, AV_OPT_SEARCH_CHILDREN)) < 0) {
        logE << m_name << ": failed to set the reply_code -> " << av_err2str(ret) << std::endl;
        return -1;
    }
 
    while ((ret = avio_handshake(m_http_client)) > 0);
 
    if (ret < 0) {
        logE << m_name << ": failed the second hand shake with the http client -> " << av_err2str(ret) << std::endl;
        return -1;
    }
    
    return 1;
}

int MediaSender::abortCallback(void *arg) {
    MediaSender *ms = static_cast<MediaSender*>(arg);
    
    return ms->isCancelRequested() ? 1 : 0;
}

int MediaSender::begin() {	
    int ret;

    if (!isActive()) {
        return -1;
    }

	avdevice_register_all();
	postproc_version(); // required solving linking libpostproc issue

	if(Config::getInt(STREAM_MODE) < STREAM_MODE_RAW || Config::getInt(STREAM_MODE) > STREAM_MODE_FILE) {
		Config::setInt(STREAM_MODE, STREAM_MODE_RAW);
	}
    
    if(Config::getInt(STREAM_MODE) == STREAM_MODE_FILE) {
		Config::setString(STREAM_OUT_FILE_ADDRESS, Helper::generateStreamUrl());

		std::string out_filename = Config::getString(STREAM_OUT_FILE_ADDRESS);
		std::string out_format = Config::getString(STREAM_OUTPUT_FORMAT);

		if ((ret = avformat_alloc_output_context2(&m_ofmt_ctx, NULL, out_format.c_str(), out_filename.c_str())) < 0) {
			logE << m_name << ": cannot open output file '" << out_filename << "' with format '" << 
				out_format << "' -> " << av_err2str(ret) << std::endl;
			return -1;
		}
	}

    if(m_videoSender.isActive()) {
        m_videoSender.setOutputFormatCtx(this, m_ofmt_ctx);

        if(m_videoSender.begin() == -1) {
            return -1;
        }
    }
	if(m_audioSender.isActive()) {
        m_audioSender.setOutputFormatCtx(this, m_ofmt_ctx);

        if(m_audioSender.begin() == -1) {
            return -1;
        }
    }

    if(m_ofmt_ctx) {
		av_dump_format(m_ofmt_ctx, 0, Config::getString(STREAM_OUT_FILE_ADDRESS).c_str(), 1);		
	}
	
	return 1;
}

int MediaSender::end() {
    m_running = false;

	if(m_process.joinable()) {
		m_process.join();
	}

	bool result = !Config::getInt(CAMERA_ACTIVE) || m_videoSender.end() != -1;
	result = result && (!Config::getInt(MICRO_ACTIVE) || m_audioSender.end() != -1);

	if(m_ofmt_ctx != NULL) {
        if(m_ofmt_ctx) {
            av_write_trailer(m_ofmt_ctx);
        }
		if (!(m_ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
			avio_closep(&m_ofmt_ctx->pb);
		}
        if(m_http_client) {
            avio_flush(m_http_client);
            avio_close(m_http_client);
            m_http_client = NULL;
        }
		avformat_free_context(m_ofmt_ctx);
		m_ofmt_ctx = NULL;
	}

	return result ? 1 : -1;
}

void MediaSender::start() {
    m_running = true;
	m_process = std::thread([this]{run(); });
}

void MediaSender::run() {
    if (m_ofmt_ctx) {
        int ret;

        if (!(m_ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
			AVDictionary *options = NULL;
            std::string out_filename = Config::getString(STREAM_OUT_FILE_ADDRESS);
            AVIOInterruptCB  callback = { .callback = &MediaSender::abortCallback, .opaque = this };

			av_dict_set(&options, "listen", "1", 0);

			if ((ret = avio_open2(&m_ofmt_ctx->pb, out_filename.c_str(), AVIO_FLAG_WRITE, &callback, &options)) < 0) {
				logE << m_name << ": could not open server address '" << out_filename << "' -> " << av_err2str(ret) << std::endl;
                m_running = false;
				return;
			}
            if(Config::getString(STREAM_PROTOCOL) == HTTP_PROTOCOL_NAME) {
                startHTTP();
            }
		}
        if ((ret = avformat_write_header(m_ofmt_ctx, NULL)) < 0) {
			logE << m_name << ": failed to write output file header -> " << av_err2str(ret) << std::endl;
			return;
		}
    }
    
	if (m_videoSender.isActive()) {
        m_videoSender.start();
        logI << m_name << ": " << m_videoSender.getName() << " was started!" << std::endl;
    }
	if (m_audioSender.isActive()) {
        m_audioSender.start();
        logI << m_name << ": " << m_audioSender.getName() << " was started!" << std::endl;
    }
}

void MediaSender::stop() {
    m_running = false;

    if(m_process.joinable()) {
		m_process.join();
	}

	if (m_videoSender.isActive()) {
        m_videoSender.stop();
        logI << m_name << ": " << m_videoSender.getName() << " was stopped!" << std::endl;
    }
	if (m_audioSender.isActive()) {
        m_audioSender.stop();
        logI << m_name << ": " << m_audioSender.getName() << " was stopped!" << std::endl;
    }
}

bool MediaSender::isCancelRequested() const {
	return !m_running;
}

bool MediaSender::isRunning() {
	return m_running && (m_videoSender.isRunning() || m_audioSender.isRunning());
}

bool MediaSender::isActive() const {
	return m_videoSender.isActive() || m_audioSender.isActive();
}

} // namespace stream