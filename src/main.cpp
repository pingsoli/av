#include <iostream>
#include <cstdio>
#include <SDL.h>

int main(int argc, char *argv[])
{
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
