cmake_minimum_required( VERSION 3.4.1 )
project( main )
set( CMAKE_VERBOSE_MAKEFILE ON )

# 设置 std=c++14
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

# set variable
set( INC_DIR ../include /home/pingsoli/ffmpeg_build/include )
set( BIN_DIR ../bin )
set( LIB_DIR /home/pingsoli/ffmpeg_build/lib )

# 递归查找当期文件夹下及子目录下所有 cpp 文件
file( GLOB_RECURSE SRC "./*.cpp" )

# set executable file output path
set( EXECUTABLE_OUTPUT_PATH ${BIN_DIR} )

# include header files directory
include_directories( ${INC_DIR} )

# add link directories
link_directories( ${LIB_DIR} )

# generate executable file
add_executable( main ${SRC} )

# load inner static/shared library
target_link_libraries( main
  -lSDL2main -lSDL2 -lavformat -lavcodec -lavutil -lswscale -lfdk-aac
  -lmp3lame -lx264 -lopus -lass -lfreetype -lswresample
  -ltheoraenc -ltheora -ltheoradec -lvorbis -lvorbisenc -lvdpau
  -lm -ldl -pthread -lz -lva -lX11 -lva-x11 -lva-drm -lstdc++ )

add_custom_target( run ../bin/main ../bin/test.mp4 )
