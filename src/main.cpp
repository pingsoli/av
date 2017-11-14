#include "CWindow.h"
#include "demux.h"
#include <iostream>

int main(int argc, char *argv[])
{
  const char      *filename = NULL;
  AVFormatContext *pFormatCtx = NULL;

  if (argc != 2) {
    std::cout << "Usage: <execute file> <video file>" << std::endl;
    return -1;
  }
  filename = argv[1];

  av_register_all();
  avformat_open_input(&pFormatCtx, filename, NULL, NULL);

  Demuxer demuxer(pFormatCtx);



  return 0;
}
