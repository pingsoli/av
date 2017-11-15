extern "C" {

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

}

#include <SDL.h>
#include <SDL_thread.h>

#include <iostream>
#include <string>

#include "PacketQueue.h"
#include "Audio.h"
#include "Media.h"
#include "VideoDisplay.h"
using namespace std;

bool quit = false;
char *filename;

int main(int argc, char* argv[])
{
	av_register_all();

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

  if (argc != 2) {
    std::cout << "Usage: <execute file> <movie file> " << std::endl;
    exit(-1);
  }
  filename = argv[1];

	MediaState media(filename);

	if (media.openInput())
		SDL_CreateThread(decode_thread, "", &media); // 创建解码线程，读取packet到队列中缓存

	media.audio->audio_play(); // create audio thread

	media.video->video_play(&media); // create video thread

	AVStream *audio_stream = media.pFormatCtx->streams[media.audio->stream_index];
	AVStream *video_stream = media.pFormatCtx->streams[media.video->stream_index];

	double audio_duration = audio_stream->duration * av_q2d(audio_stream->time_base);
	double video_duration = video_stream->duration * av_q2d(video_stream->time_base);

	cout << "audio duration: " << audio_duration << endl;
	cout << "video duration: " << video_duration << endl;

	SDL_Event event;
	while (true) // SDL event loop
	{
		SDL_WaitEvent(&event);
		switch (event.type)
		{
		case FF_QUIT_EVENT:
		case SDL_QUIT:
			quit = 1;
			SDL_Quit();

			return 0;
			break;

		case FF_REFRESH_EVENT:
			video_refresh_timer(&media);
			break;

		default:
			break;
		}
	}

	getchar();
	return 0;
}
