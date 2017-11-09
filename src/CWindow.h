#pragma once 

#include <SDL2/SDL.h>
#include <array>

class CWindow 
{
private:
  SDL_Window   *window_;
  SDL_Renderer *renderer_;
  SDL_Texture  *texture_;

  int width_, height_;

  void init();

public:
  CWindow(int width, int height);
  ~CWindow();

  void refresh(std::array<uint8_t*, 3> planes, 
                  std::array<size_t, 3> pitches);
};
