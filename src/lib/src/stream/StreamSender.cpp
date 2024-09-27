#include "stream/StreamSender.h"
#include "stream/MediaSender.h"
#include "net/NetSender.h"
#include "utils/Logger.h"
#include "utils/Config.h"
#include "utils/Helper.h"
#include "utils/Constants.h"

using namespace utils;

namespace stream {

StreamSender::StreamSender(NetSender &sender) : m_sender(sender) {
	
}

int StreamSender::begin() {
	m_frameIndex = 0;
	
	avdevice_register_all();
	
	if(openInputStream() == -1) {
		return -1;
	}

	if(Config::getInt(STREAM_MODE) != STREAM_MODE_RAW) {
		if(openOutputStream() == -1) {
			return -1;
		}

		if(initFilters() == -1) {
			return -1;
		}
	}

	return 1;
}

int StreamSender::end() {
	m_running = false;

	if(m_process.joinable()) {
		m_process.join();
	}

	if(m_packet != NULL) {
		av_packet_free(&m_packet);
		m_packet = NULL;
	}
	if(m_frame != NULL) {
		av_frame_free(&m_frame);
		m_frame = NULL;
	}
	if(m_options != NULL) {
		av_dict_free(&m_options);
		m_options = NULL;
	}
	if(m_decoder_ctx != NULL) {
		avcodec_free_context(&m_decoder_ctx);
		m_decoder_ctx = NULL;
		m_decoder = NULL;
	}
	if(m_encoder_ctx != NULL) {
		avcodec_free_context(&m_encoder_ctx);
		m_encoder_ctx = NULL;
		m_encoder = NULL;
	}
	if(m_filter_ctx.filter_graph != NULL) {
		avfilter_graph_free(&m_filter_ctx.filter_graph);
		av_packet_free(&m_filter_ctx.enc_pkt);
		av_frame_free(&m_filter_ctx.filtered_frame);
		m_filter_ctx.filter_graph = NULL;
		m_filter_ctx.buffersink_ctx = NULL;
		m_filter_ctx.buffersrc_ctx = NULL;
		m_filter_ctx.enc_pkt = NULL;
		m_filter_ctx.filtered_frame = NULL;
	}
	if(m_ifmt_ctx != NULL) {
		avformat_close_input(&m_ifmt_ctx);
		m_ifmt_ctx = NULL;
		m_istream = NULL;
	}
	if(m_ofmt_ctx != NULL) {
		m_ofmt_ctx = NULL;
		m_ostream = NULL;
		m_media_sender = NULL;
	}
	if(m_sws_ctx != NULL) {
		sws_freeContext(m_sws_ctx);
		m_sws_ctx = NULL;
	}
	if(m_swr_ctx != NULL) {
		swr_free(&m_swr_ctx);
		m_swr_ctx = NULL;
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
			logE << m_name << ": cannot read captured frame -> " << av_err2str(ret) << std::endl;
			continue;
		}
		if(m_filter_ctx.filter_graph) {  
            if ((ret = avcodec_send_packet(m_decoder_ctx, m_packet)) < 0) {
				logE << m_name << ": decoding failed -> " << av_err2str(ret) << std::endl;
				m_running = false;
                break;
            }
 
            while (ret >= 0) {
                ret = avcodec_receive_frame(m_decoder_ctx, m_frame);
                if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
                    break;
				} else if (ret < 0) {
					logE << m_name << ": failed to receive a frame from the decoder -> " << av_err2str(ret) << std::endl;
                    m_running = false;
					break;
				}

                m_frame->pts = m_frame->best_effort_timestamp;

                if ((ret = filterEncodeFrame(m_frame)) < 0) {
                    m_running = false;
					break;
				}
            }
		} else {
			sendPacket(m_packet);
		}
		
		av_packet_unref(m_packet);
	}

	if (m_filter_ctx.filter_graph) {  
		flushBuffers();
	}
}

void StreamSender::sendPacket(AVPacket *packet) {	
	StreamFragment sf;
	sf.frameNumber = ++m_frameIndex;
	sf.frameSize = packet->size;
	sf.fragmentSize = Config::getInt(NET_FRAGMENT_SIZE) - NET_FRAME_HEADER - STREAM_FRAME_HEADER;

	int fragmentPerFrame = (int)std::ceil(packet->size / (float)sf.fragmentSize);
	int total = (int)(fragmentPerFrame * sf.fragmentSize);

	for(int i = 0; i < fragmentPerFrame; i++) {
		sf.fragmentNumber = i;
		sf.fragmentData = packet->data + (sf.fragmentSize * i);

		if(i == fragmentPerFrame - 1 && total > packet->size) {
			sf.fragmentSize = sf.fragmentSize - (total - packet->size);
		}
		
		m_sender.sendFrame(m_streamID, NS_FRAME_TYPE_DATA, sf);
#ifndef __arm__
		std::this_thread::sleep_for(std::chrono::microseconds(10));
#endif
	}
}

int StreamSender::openInputStream() {
	int ret;
	const AVInputFormat *format = NULL;
	
	if(!m_formatname.empty()) {
		format = av_find_input_format(m_formatname.c_str());
	}
	
	if ((ret = avformat_open_input(&m_ifmt_ctx, m_filename.c_str(), format, &m_options)) < 0) {
        logE << m_name << ": cannot capture input file '" << m_filename << "' -> " << av_err2str(ret) << std::endl;
		return -1;
    }
 
    if ((ret = avformat_find_stream_info(m_ifmt_ctx, NULL)) < 0) {
		logE << m_name << ": cannot find stream information -> " << av_err2str(ret) << std::endl;
		return -1;
    }

    if ((ret = av_find_best_stream(m_ifmt_ctx, m_mediaType, -1, -1, &m_decoder, 0)) < 0) {
		logE << m_name << ": cannot find this stream in the input file -> " << av_err2str(ret) << std::endl;
		return -1;
    }

	m_istream = m_ifmt_ctx->streams[ret];

	if (!(m_packet = av_packet_alloc())) {
		logE << m_name << ": cannot allocate packet memory" << std::endl;
		return -1;
	}

	if (!(m_decoder_ctx = avcodec_alloc_context3(m_decoder))) {
		logE << m_name << ": failed to allocate the decoder context for stream #" << m_istream->index << std::endl;
		return -1;
	}

	if ((ret = avcodec_parameters_to_context(m_decoder_ctx, m_istream->codecpar)) < 0) {
		logE << m_name << ": failed to copy decoder params to the decoder context for stream #" << 
			m_istream->index << " -> " << av_err2str(ret) << std::endl;
		return -1;
	}

	m_decoder_ctx->pkt_timebase = m_istream->time_base;
	initDecoderCtx();

	if (Config::getInt(STREAM_MODE) != STREAM_MODE_RAW) {		
		if ((ret = avcodec_open2(m_decoder_ctx, m_decoder, NULL)) < 0) {
			logE << m_name << ": failed to open the decoder context for stream #" << m_istream->index << std::endl;
            return -1;
		}

		if (!(m_frame = av_frame_alloc())) {
			logE << m_name << ": cannot allocate frame memory" << std::endl;
			return -1;
		}
	}
	
	return 1;
}

int StreamSender::openOutputStream() {
	int ret;

	if(m_ofmt_ctx) {	
        if (!(m_ostream = avformat_new_stream(m_ofmt_ctx, NULL))) {
            logE << m_name << ": failed to allocate the output stream" << std::endl;
            return -1;
        }
	}

	const char* encoder_config = (m_mediaType == AVMEDIA_TYPE_VIDEO) ? VIDEO_ENCODER : AUDIO_ENCODER;
	std::string encoder_name = Config::getString(encoder_config);

	if (!encoder_name.empty()) {
		if (!(m_encoder = avcodec_find_encoder_by_name(encoder_name.c_str()))) {
			logW << m_name << ": could not find encoder " << encoder_name << std::endl;
		}			
	}
	if (!m_encoder && m_ofmt_ctx) {
		AVCodecID codecID = (m_mediaType == AVMEDIA_TYPE_VIDEO) ? 
			m_ofmt_ctx->oformat->video_codec : m_ofmt_ctx->oformat->audio_codec;
		encoder_name = avcodec_get_name(codecID);

		if (!(m_encoder = avcodec_find_encoder(codecID))) {
			logW << m_name << ": could not find encoder from output format codec " << encoder_name << std::endl;
		}
	}
	if (!m_encoder) {
		encoder_name = avcodec_get_name(m_decoder_ctx->codec_id);
		if (!(m_encoder = avcodec_find_encoder(m_decoder_ctx->codec_id))) {
			logW << m_name << ": could not find encoder from decoder codec " << encoder_name << std::endl;
			return -1;
		}	
	}

	if (!(m_encoder_ctx = avcodec_alloc_context3(m_encoder))) {
		logE << m_name << ": failed to allocate the encoder context" << std::endl;
        return -1;
	}

	initEncoderCtx();

	if (m_ofmt_ctx && (m_ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)) {
        m_encoder_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}
	
	if ((ret = avcodec_open2(m_encoder_ctx, m_encoder, NULL)) < 0) {
		logE << m_name << ": failed to open the encoder context for encoder " << 
			m_encoder->name << " -> " << av_err2str(ret) << std::endl;
		return -1;
	}
	if(m_ostream) {
		if ((ret = avcodec_parameters_from_context(m_ostream->codecpar, m_encoder_ctx)) < 0) {
			logE << m_name << ": failed to copy encoder parameters to output stream #" << 
				m_ostream->index << " -> " << av_err2str(ret) << std::endl;
		}
		m_ostream->time_base = m_encoder_ctx->time_base;
	}

	return 1;
}

int StreamSender::initFilters() {
	int ret;
	const char *filter_spec;

	m_filter_ctx.filter_graph = avfilter_graph_alloc();

	if (!m_filter_ctx.filter_graph) {
        logE << m_name << ": failed to allocate filter graph memory" << std::endl;
        return -1;
    }

	if (m_istream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		filter_spec = "null"; /* passthrough (dummy) filter for video */
	else
		filter_spec = "anull"; /* passthrough (dummy) filter for audio */

	if (initFilterCtx() < 0) {
		return -1;
	}

	AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();

	if (!outputs || !inputs) {
        logE << m_name << ": failed to allocate filter input/output memory" << std::endl;
        return -1;
    }

	/* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = m_filter_ctx.buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;
 
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = m_filter_ctx.buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;
 
    if (!outputs->name || !inputs->name) {
        logE << m_name << ": failed to allocate filter input/output name" << std::endl;
        ret = -1;
    } else if ((ret = avfilter_graph_parse_ptr(m_filter_ctx.filter_graph, filter_spec,
        &inputs, &outputs, NULL)) < 0) {
		logE << m_name << ": failed to parse the filter graph -> " << av_err2str(ret) << std::endl;
        ret = -1;
	} else if ((ret = avfilter_graph_config(m_filter_ctx.filter_graph, NULL)) < 0) {
		logE << m_name << ": failed to configure the filter graph -> " << av_err2str(ret) << std::endl;
        ret = -1;
	} else {
		ret = 1;
	}

    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

	if(ret < 0) {
		return -1;
	}

	if (!(m_filter_ctx.enc_pkt = av_packet_alloc())) {
		logE << m_name << ": cannot allocate filter packet memory" << std::endl;
		return -1;
	}

	if (!(m_filter_ctx.filtered_frame = av_frame_alloc())) {
		logE << m_name << ": cannot allocate filter frame memory" << std::endl;
		return -1;
	}
 
    return 1;
}

int StreamSender::filterEncodeFrame(AVFrame *frame) {
    int ret;
 
    /* push the decoded frame into the filtergraph */
    if ((ret = av_buffersrc_add_frame_flags(m_filter_ctx.buffersrc_ctx, frame, 0)) < 0) {
		logE << m_name << ": error while feeding the filtergraph -> " << av_err2str(ret) << std::endl;
		return ret;
    }
 
    /* pull filtered frames from the filtergraph */
    while (1) {
        if ((ret = av_buffersink_get_frame(m_filter_ctx.buffersink_ctx, m_filter_ctx.filtered_frame)) < 0) {
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) { ret = 0; }
            break;
        }

        m_filter_ctx.filtered_frame->time_base = av_buffersink_get_time_base(m_filter_ctx.buffersink_ctx);
        m_filter_ctx.filtered_frame->pict_type = AV_PICTURE_TYPE_NONE;
        ret = encodeFrame(false);
        av_frame_unref(m_filter_ctx.filtered_frame);

        if (ret < 0) {
            break;
		}
    }
 
    return ret;
}

int StreamSender::encodeFrame(bool flush) {
	int ret;
    AVFrame *filt_frame = flush ? NULL : m_filter_ctx.filtered_frame;
    AVPacket *enc_pkt = m_filter_ctx.enc_pkt;
 
    av_packet_unref(enc_pkt);

    if (filt_frame && filt_frame->pts != AV_NOPTS_VALUE) {
        filt_frame->pts = av_rescale_q(filt_frame->pts, filt_frame->time_base, m_encoder_ctx->time_base);
	}
  
    if ((ret = avcodec_send_frame(m_encoder_ctx, filt_frame)) < 0) {
		logE << m_name << ": failed to send filter frame -> " << av_err2str(ret) << std::endl;
        return ret;
	}

    while (ret >= 0) {
        ret = avcodec_receive_packet(m_encoder_ctx, enc_pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return 0;
		}

		if(m_ofmt_ctx && m_ostream && m_media_sender) {
			/* prepare packet for muxing */
			enc_pkt->stream_index = m_ostream->index;
			av_packet_rescale_ts(enc_pkt, m_encoder_ctx->time_base,
				m_ofmt_ctx->streams[m_ostream->index]->time_base);
	
			/* mux encoded frame */
			ret = m_media_sender->writePacket(enc_pkt);
		} else {
			sendPacket(enc_pkt);
			ret = 1;
		}
    }

	return ret;
}

void StreamSender::flushBuffers() {
	int ret;

	/* flush decoder */
	if ((ret = avcodec_send_packet(m_decoder_ctx, NULL)) < 0) {
		logE << m_name << ": failed to flush decoder -> " << av_err2str(ret) << std::endl;
		return;
	}

	while (ret >= 0) {
		ret = avcodec_receive_frame(m_decoder_ctx, m_frame);
		if (ret == AVERROR_EOF) {
			break;
		} else if (ret < 0) {
			return;
		}

		m_frame->pts = m_frame->best_effort_timestamp;

		if ((ret = filterEncodeFrame(m_frame)) < 0) {
			return;
		}
	}

	/* flush filter */
	if ((ret = filterEncodeFrame(NULL)) < 0) {
		return;
	}

	/* flush encoder */
	if (m_encoder_ctx->codec->capabilities & AV_CODEC_CAP_DELAY) {
		encodeFrame(true);
	}
}

} // namespace net