#!/bin/bash

FFMPEG_PATH=/home/pingsoli/ffmpeg_build

gcc -g tutorial01.c -o test   -I$FFMPEG_PATH/include -L$FFMPEG_PAT$FFMPEG_PATH/lib   \
  -lSDL2main -lSDL2 -lavformat -lavcodec -lavutil -lswscale -lfdk-aac \
  -lmp3lame -lx264 -lopus -lass -lfreetype -lswresample  \
  -ltheoraenc -ltheora -ltheoradec -lvorbis -lvorbisenc -lvdpau   \
  -lm -ldl -pthread -lz -lva -lX11 -lva-x11 -lva-drm -lstdc++
