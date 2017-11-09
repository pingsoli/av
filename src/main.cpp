extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}
#include "CWindow.h"

int decode(AVCodecContext *avctx, AVFrame *frame,
  int *got_frame, AVPacket *pkt)
{
  int ret;
  *got_frame = 0;
  if (pkt) {
    ret = avcodec_send_packet(avctx, pkt);
    if (ret < 0)
      return ret == AVERROR_EOF ? 0 : ret;
  }
  
  ret = avcodec_receive_frame(avctx, frame);
  if (ret < 0 && ret != AVERROR(EAGAIN)
    && ret != AVERROR_EOF)
    return ret;
  if (ret >= 0)
    *got_frame = 1;
  return 0;
}

void check(int ret, const char *message) 
{
  if (ret < 0) {
    fprintf(stderr, "Error(invalid return): %s\n", message);
    exit(-1);
  }
}

void check_nullptr(void *ptr, const char *message)
{
  if (ptr == NULL) {
    fprintf(stderr, "Error(null pointer): %s\n", message);
    exit(-1);
  }
}

int main(int argc, char *argv[])
{
  const char      *filename = NULL;
  AVFormatContext *pFormatCtx = NULL;
  AVCodecContext  *pCodecCtx = NULL;
  AVCodec         *pCodec = NULL;
  AVCodecParameters *pCodecPar = NULL;
  int             i, vsi = -1; // Video Stream Index
  AVFrame         *pFrame, *pFrameYUV;
  AVPacket        *pkt;
  uint8_t         *buffer;
  int             ret, got_pic;
  struct SwsContext *img_convert_ctx;
  int             sw, sh;

  if (argc != 2) {
    fprintf(stderr, "Usage: <exec file> <movie file>\n");
    return -1;
  }
  filename = argv[1];

  av_register_all();
  ret = avformat_open_input(&pFormatCtx, filename, NULL, NULL);
  check(ret, "avformat_open_input failed");

  ret = avformat_find_stream_info(pFormatCtx, NULL);
  check(ret, "avformat_find_stream_info failed");

  // Print stream info
  av_dump_format(pFormatCtx, 0, NULL, 0);
  
  // Find video stream index
  vsi = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);  
  check(vsi, "cannot find video stream");

  pCodecPar = pFormatCtx->streams[vsi]->codecpar;
  pCodec = avcodec_find_decoder(pCodecPar->codec_id);
  check_nullptr(pCodec, "avcodc_find_decoder failed");

  pCodecCtx = avcodec_alloc_context3(pCodec);
  avcodec_parameters_to_context(pCodecCtx, pCodecPar);
  ret = avcodec_open2(pCodecCtx, pCodec, NULL);
  check(ret, "avcodec_open2 failed");
  
  pFrame = av_frame_alloc();
  pFrameYUV = av_frame_alloc();
  long buffer_size = 0;
  buffer_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
    pCodecCtx->width, pCodecCtx->height, 1);

  buffer = (uint8_t *) av_malloc(buffer_size);
  av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, buffer,
      AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);

  pkt = (AVPacket *) av_malloc(sizeof(AVPacket));
  // Pixel format conversion, convert original pix_fmt to YUV420p
  img_convert_ctx = sws_getContext(
      pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
      pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,
      SWS_BICUBIC, NULL, NULL, NULL);

  sw = pCodecCtx->width;
  sh = pCodecCtx->height;
  CWindow window(sw, sh);

  while (av_read_frame(pFormatCtx, pkt) >= 0) {
    if (pkt->stream_index == vsi) {
      ret = decode(pCodecCtx, pFrame, &got_pic, pkt);
      check(ret, "decode failed");

      if (got_pic) {
        sws_scale(img_convert_ctx, (const unsigned char *const *)pFrame->data,
            pFrame->linesize, 0, pCodecCtx->height,
            pFrameYUV->data, pFrameYUV->linesize);

      window.refresh(
        {pFrameYUV->data[0], pFrameYUV->data[1], pFrameYUV->data[2]},
        {static_cast<size_t>(pFrameYUV->linesize[0]), 
         static_cast<size_t>(pFrameYUV->linesize[1]), 
         static_cast<size_t>(pFrameYUV->linesize[2])});
      }
    }
  }

  // Flush last frame into screen
  while (1) {
    ret = decode(pCodecCtx, pFrame, &got_pic, pkt);
    if (ret < 0) break;
    if (!got_pic) break;

    window.refresh(
        {pFrameYUV->data[0], pFrameYUV->data[1], pFrameYUV->data[2]},
        {static_cast<size_t>(pFrameYUV->linesize[0]), 
         static_cast<size_t>(pFrameYUV->linesize[1]), 
         static_cast<size_t>(pFrameYUV->linesize[2])});
  }

  av_packet_unref(pkt);
  sws_freeContext(img_convert_ctx);
  SDL_Quit();
  av_frame_free(&pFrame);
  av_frame_free(&pFrameYUV);
  avcodec_close(pCodecCtx);
  avformat_close_input(&pFormatCtx);

  return 0;
}
