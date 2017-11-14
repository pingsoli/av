#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

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

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
  FILE *fp;
  char filename[256];
  int y;

  // Open file 
  sprintf(filename, "frame%d.ppm", iFrame);
  fp = fopen(filename, "wb");
  check_nullptr(fp, "fopen failed");

  // Write header
  fprintf(fp, "P6\n%d %d\n255\n", width, height);

  // Write pixel data
  for (y = 0; y < height; y++) {
    fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, fp);
  }

  // Close file
  fclose(fp);
}

int main(int argc, char *argv[])
{
  const char      *filename = NULL;
  AVFormatContext *pFormatCtx = NULL;
  AVCodecContext  *pCodecCtx = NULL;
  AVCodec         *pCodec = NULL;
  AVCodecParameters *pCodecPar = NULL;
  int             i, vsi = -1; // Video Stream Index
  int             frameNum = 0;
  AVFrame         *pFrame, *pFrameRGB;
  AVPacket        *pkt;
  uint8_t         *buffer;
  int             ret, gotPicture;
  struct SwsContext *img_convert_ctx;

  if (argc != 2) {
    fprintf(stderr, "Usage: <exec file> <movie file>\n");
    return -1;
  }
  filename = argv[1];

  // Register all formats and codecs
  av_register_all();

  // Open video file
  ret = avformat_open_input(&pFormatCtx, filename, NULL, NULL);
  check(ret, "avformat_open_input failed");

  // Retrieve stream information 
  ret = avformat_find_stream_info(pFormatCtx, NULL);
  check(ret, "avformat_find_stream_info failed");

  // Print stream info
  av_dump_format(pFormatCtx, 0, NULL, 0);
  
  // Find video stream index
  vsi = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);  
  check(vsi, "cannot find video stream");

  // Find the decoder for the video stream
  pCodecPar = pFormatCtx->streams[vsi]->codecpar;
  pCodec = avcodec_find_decoder(pCodecPar->codec_id);
  check_nullptr(pCodec, "avcodc_find_decoder failed");

  // Open codec
  pCodecCtx = avcodec_alloc_context3(pCodec);
  avcodec_parameters_to_context(pCodecCtx, pCodecPar);
  ret = avcodec_open2(pCodecCtx, pCodec, NULL);
  check(ret, "avcodec_open2 failed");
 
  // Allocate an AVFrame structure
  pFrame = av_frame_alloc();
  pFrameRGB = av_frame_alloc();

  long buffer_size = 0;
  buffer_size = av_image_get_buffer_size(AV_PIX_FMT_RGB24,
    pCodecCtx->width, pCodecCtx->height, 1);

  buffer = (uint8_t *) av_malloc(buffer_size);
  av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer,
      AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);

  pkt = (AVPacket *) av_malloc(sizeof(AVPacket));

  // Pixel format conversion, convert original pix_fmt to YUV420p
  img_convert_ctx = sws_getContext(
      pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
      pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24,
      SWS_BILINEAR, NULL, NULL, NULL); 
 
  // Read frames and save first five frames to disk
  while (av_read_frame(pFormatCtx, pkt) >= 0) {
    // Is this a packet from the video stream ?
    if (pkt->stream_index == vsi) {
      // Decode video frame
      ret = decode(pCodecCtx, pFrame, &gotPicture, pkt);
      check(ret, "decode failed");

      // Did we get a video frame?
      if (gotPicture) {
        // Convert the image from its native format to RGB
        sws_scale(img_convert_ctx, (uint8_t const* const *)pFrame->data,
            pFrame->linesize, 0, pCodecCtx->height,
            pFrameRGB->data, pFrameRGB->linesize);
        
        if (++frameNum <= 5) 
          SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, frameNum);
      }
    }
  }

  av_packet_unref(pkt);
  sws_freeContext(img_convert_ctx);
  av_frame_free(&pFrameRGB);
  avcodec_close(pCodecCtx);
  avformat_close_input(&pFormatCtx);

  return 0;
}
