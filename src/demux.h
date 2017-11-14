#pragma once

extern "C" {
  #include "libavformat/avformat.h"
  #include "libavutil/avutil.h"
}
#include <vector>

class Demuxer 
{
public:
  Demuxer(AVFormatContext *avfc);
  // Print demuxing info
  void print();

private:
  std::vector<int> video_stream_index;
  std::vector<int> audio_stream_index;
  std::vector<int> subtitle_stream_index;
};
