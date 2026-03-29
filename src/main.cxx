#include "cpu.h"
#include "renderer.h"
#include "utils.h"
#include <SDL3/SDL_main.h> // Required for cross-platform main entry

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  CPU cpu{};
  if (!cpu.loadROM("rom/IBM Logo.ch8")) {
    return 1;
  }
  Renderer renderer{};

  bool running{true};

  SDL_Event event;
  auto handleEvent = [&running](const SDL_Event &event) {
    if (event.type == SDL_EVENT_QUIT) {
      running = false;
    }
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
      running = false;
    }
  };

  while (running) {
    while (SDL_PollEvent(&event)) {
      handleEvent(event);
    }
    cpu.cycle();
    renderer.Render(cpu.getDisplay());
    SDL_Delay(static_cast<Uint32>(1000.0 / FREQUENCY));
  }

  SDL_Quit();

  return 0;
}