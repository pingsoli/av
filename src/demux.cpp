#include "demux.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>

Demuxer::Demuxer(AVFormatContext *avfc)
{
  int ret = avformat_find_stream_info(avfc, NULL);
  if (ret < 0) {
    fprintf(stderr, "avformat_find_stream_info failed\n");
  }
  
  // Get all streams
  for (int i = 0; i < avfc->nb_streams; ++i) {
    AVMediaType type = avfc->streams[i]->codecpar->codec_type;
    if (type == AVMEDIA_TYPE_VIDEO) {
      video_stream_index.push_back(i);
    } else if (type == AVMEDIA_TYPE_AUDIO) {
      audio_stream_index.push_back(i);
    } else if (type == AVMEDIA_TYPE_SUBTITLE) {
      subtitle_stream_index.push_back(i);
    }
  }
}

void Demuxer::print()
{
  std::stringstream video_ss;
  std::stringstream audio_ss;
  std::stringstream subtitle_ss;

  // Copy vector<int> to output stringstream
  std::copy(video_stream_index.begin(), video_stream_index.end(),
      std::ostream_iterator<int>(video_ss, " "));
  std::copy(audio_stream_index.begin(), audio_stream_index.end(),
      std::ostream_iterator<int>(audio_ss, " "));
  std::copy(subtitle_stream_index.begin(), subtitle_stream_index.end(), 
      std::ostream_iterator<int>(subtitle_ss, " "));

  std::cout << "Video Stream Index: " << video_ss.str() << std::endl
    << "Audio Stream Index: " << audio_ss.str() << std::endl
    << "Subtitle Stream Index: " << subtitle_ss.str() << std::endl;
}
