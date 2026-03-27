#pragma once

#include "constants.h"
#include <SDL3/SDL.h>
#include <array>

class Renderer {
public:
  Renderer();
  Renderer(const Renderer &) = delete;
  Renderer(Renderer &&) = delete;
  Renderer &operator=(const Renderer &) = delete;
  Renderer &operator=(Renderer &&) = delete;
  ~Renderer();
  void Render(
      const std::array<Byte, DISPLAY_HEIGHT * DISPLAY_WIDTH> &display) noexcept;

private:
  SDL_Window *window{};
  SDL_Renderer *renderer{};
};