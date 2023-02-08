#include "stream/VideoReceiver.h"
#include "utils/Logger.h"
#include "utils/Config.h"
#include "utils/Constants.h"

extern "C" {
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

using namespace utils;

namespace stream {

VideoReceiver::VideoReceiver() {
	m_name = "VideoReceiverService";
}

int VideoReceiver::end() {
	if(StreamReceiver::end() == -1) {
		return -1;
	}

	if(m_decoder.codecCtx != NULL) {
		avcodec_free_context(&m_decoder.codecCtx);
		m_decoder.codecCtx = NULL;
	}
	if(m_decoder.convertCtx != NULL) {
		sws_freeContext(m_decoder.convertCtx);
		m_decoder.convertCtx = NULL;
	}
	if(m_decoder.frame != NULL) {
		av_frame_free(&m_decoder.frame);
		m_decoder.frame = NULL;
	}
	if(m_decoder.frameScl != NULL) {
		av_frame_free(&m_decoder.frameScl);
		m_decoder.frameScl = NULL;
	}
	if(m_decoder.avPacket != NULL) {
		av_packet_free(&m_decoder.avPacket);
		m_decoder.avPacket = NULL;
	}
	if(m_decoder.swr_ctx != NULL) {
		swr_free(&m_decoder.swr_ctx);
		m_decoder.swr_ctx = NULL;
	}
	if(m_decoder.filter_graph != NULL) {
		avfilter_graph_free(&m_decoder.filter_graph);
		m_decoder.filter_graph = NULL;
	}

	return 1;
}

} // namespace stream