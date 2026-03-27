#include "renderer.h"
#include "constants.h"
#include "utils.h"

Renderer::Renderer() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Error initializing SDL: %s", SDL_GetError());
    return;
  }
  window = SDL_CreateWindow("Chip 8 emulator", DISPLAY_WIDTH * DISPLAY_SCALE,
                            DISPLAY_HEIGHT * DISPLAY_SCALE, 0);
  if (!window) {
    SDL_Log("Error creating window: %s", SDL_GetError());
    return;
  }

  renderer = SDL_CreateRenderer(window, NULL);
  if (!renderer) {
    SDL_Log("Error creating renderer: %s", SDL_GetError());
    return;
  }
}

void Renderer::Render(
    const std::array<Byte, DISPLAY_HEIGHT * DISPLAY_WIDTH> &display) noexcept {
  if (!window || !renderer) {
    return;
  }
  // Background off -> render black
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  // On pixel -> white
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  for (std::size_t r{}; r < DISPLAY_HEIGHT; ++r) {
    for (std::size_t c{}; c < DISPLAY_WIDTH; ++c) {
      std::size_t idx{convert2DIndicesTo1DIndex(r, c, DISPLAY_WIDTH)};
      Byte pixel = display[idx];
      if (pixel) {
        SDL_FRect rect;
        rect.x = static_cast<float>(c * DISPLAY_SCALE);
        rect.y = static_cast<float>(r * DISPLAY_SCALE);
        rect.w = static_cast<float>(DISPLAY_SCALE);
        rect.h = static_cast<float>(DISPLAY_SCALE);
        SDL_RenderFillRect(renderer, &rect);
      }
    }
  }

  SDL_RenderPresent(renderer);
}

Renderer::~Renderer() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}