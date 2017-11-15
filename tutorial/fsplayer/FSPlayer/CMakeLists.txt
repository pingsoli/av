cmake_minimum_required( VERSION 3.4.1 )
project( main )
set( CMAKE_VERBOSE_MAKEFILE ON )

# 设置 std=c++11
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

# Add debug 
#set( CMAKE_BUILD_TYPE Debug )
set( CMAKE_BUILE_TYPE RelWithDebInfo )

# Set ffmpeg and SDL2 include directory
set( FFMPEG_INCLUDE_DIR /home/pingsoli/ffmpeg_build/include )
set( FFMPEG_LIB_DIR /home/pingsoli/ffmpeg_build/lib )
set( SDL_INCLUDE_DIR /home/pingsoli/ffmpeg_build/include/SDL2 )
set( SDL_LIB_DIR /home/pingsoli/ffmpeg_build/lib )

# set variable
set( INC_DIR ${FFMPEG_INCLUDE_DIR} ${SDL_INCLUDE_DIR} )
set( BIN_DIR ../bin ) 
set( LIB_DIR ${FFMPEG_LIB_DIR} ${SDL_LIB_DIR} ) 

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
