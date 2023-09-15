#include "stream/StreamSender.h"
#include "net/NetSender.h"
#include "utils/Logger.h"
#include "utils/Config.h"
#include "utils/Constants.h"

using namespace utils;

namespace stream {

StreamSender::StreamSender(NetSender &sender) : m_sender(sender) {
	
}

int StreamSender::begin() {
	int ret;
	const AVInputFormat *format = NULL;

	m_frameIndex = 0;
	avdevice_register_all();

	if(!m_formatname.empty()) {
		format = av_find_input_format(m_formatname.c_str());
	}
	
	if ((ret = avformat_open_input(&m_ifmt_ctx, m_filename.c_str(), format, &m_options)) < 0) {
        logE << "StreamSender: cannot capture input file '" << m_filename << "' -> " << av_err2str(ret) << std::endl;
		return -1;
    }
 
    if ((ret = avformat_find_stream_info(m_ifmt_ctx, NULL)) < 0) {
		logE << "StreamSender: cannot find stream information -> " << av_err2str(ret) << std::endl;
		return -1;
    }

    if ((ret = av_find_best_stream(m_ifmt_ctx, m_mediaType, -1, -1, &m_codec, 0)) < 0) {
		logE << "StreamSender: cannot find this stream in the input file -> " << av_err2str(ret) << std::endl;
		return -1;
    }

	if (!(m_packet = av_packet_alloc())) {
		logE << "StreamSender: cannot allocate packet memory -> " << std::endl;
		return -1;
	}
	
	return 1;
}

int StreamSender::end() {
	m_running = false;

	if(m_process.joinable()) {
		m_process.join();
	}

	if(m_ifmt_ctx != NULL) {
		avformat_close_input(&m_ifmt_ctx);
		m_ifmt_ctx = NULL;
	}

	if(m_packet != NULL) {
		av_packet_free(&m_packet);
		m_packet = NULL;
	}

	if(m_options != NULL) {
		av_dict_free(&m_options);
		m_options = NULL;
	}

	return 1;
}

void StreamSender::start() {
	m_running = true;
	m_process = std::thread([this]{run(); });
}

void StreamSender::stop() {
	m_running = false;
	
	if(m_process.joinable()) {
		m_process.join();
	}
}

void StreamSender::run() {
	int ret;

	while (m_running)
	{
		if ((ret = av_read_frame(m_ifmt_ctx, m_packet)) < 0) {
			logE << "StreamSender: cannot read captured frame -> " << av_err2str(ret) << std::endl;
			continue;
		}
		sendPacket();
		av_packet_unref(m_packet);
	}
}

void StreamSender::sendPacket() {	
	StreamFragment sf;
	sf.frameNumber = ++m_frameIndex;
	sf.frameSize = m_packet->size;
	sf.fragmentSize = Config::getInt(NET_FRAGMENT_SIZE) - NET_FRAME_HEADER - STREAM_FRAME_HEADER;

	int fragmentPerFrame = (int)std::ceil(m_packet->size / (float)sf.fragmentSize);
	int total = (int)(fragmentPerFrame * sf.fragmentSize);

	for(int i = 0; i < fragmentPerFrame; i++) {
		sf.fragmentNumber = i;
		sf.fragmentData = m_packet->data + (sf.fragmentSize * i);

		if(i == fragmentPerFrame - 1 && total > m_packet->size) {
			sf.fragmentSize = sf.fragmentSize - (total - m_packet->size);
		}
		
		m_sender.sendFrame(m_streamID, NS_FRAME_TYPE_DATA, sf);
#ifndef __arm__
		std::this_thread::sleep_for(std::chrono::microseconds(10));
#endif
	}
}

} // namespace net