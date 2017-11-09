#include "CWindow.h"
#include <string>
#include <stdexcept>

CWindow::CWindow(int width, int height)
  : width_{width}, height_{height}
{
  // SDL Init and alloc memory
  init();
}

template <typename T>
void check_nullptr(T&& t, const std::string &msg) 
{
  if (t == nullptr) {
    throw std::runtime_error{msg};
  }
}

void CWindow::init()
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    throw std::runtime_error{"SDL Initialization"};
  }

  window_ = SDL_CreateWindow("av", SDL_WINDOWPOS_UNDEFINED, 
      SDL_WINDOWPOS_UNDEFINED, width_, height_,
      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  check_nullptr(window_, "SDL create window failed");

  renderer_ = SDL_CreateRenderer(window_, -1, 
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  check_nullptr(renderer_, "SDL create renderer failed");

  texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_YV12,
      SDL_TEXTUREACCESS_STREAMING, width_, height_);
  check_nullptr(texture_, "SDL Create texture failed");
}

CWindow::~CWindow()
{
  SDL_DestroyTexture(texture_);
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
  SDL_Quit();
}

void CWindow::refresh(std::array<uint8_t*, 3> planes, 
                          std::array<size_t, 3> pitches)
{
  SDL_UpdateYUVTexture(texture_, nullptr, 
      planes[0], pitches[0],
      planes[1], pitches[1],
      planes[2], pitches[2]);

  SDL_RenderClear(renderer_);
  SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
  SDL_RenderPresent(renderer_);
}
