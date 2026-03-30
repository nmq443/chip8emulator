#include "cpu.h"
#include "renderer.h"
#include "utils.h"
#include <SDL3/SDL_main.h> // Required for cross-platform main entry

/*
CHIP-8 pad    ->   PC keyboard
1  2  3  C         1  2  3  4
4  5  6  D         Q  W  E  R
7  8  9  E         A  S  D  F
A  0  B  F         Z  X  C  V
*/

static constexpr std::array<SDL_Scancode, 16> KEYMAP = {
    SDL_SCANCODE_X, // 0
    SDL_SCANCODE_1, // 1
    SDL_SCANCODE_2, // 2
    SDL_SCANCODE_3, // 3
    SDL_SCANCODE_Q, // 4
    SDL_SCANCODE_W, // 5
    SDL_SCANCODE_E, // 6
    SDL_SCANCODE_A, // 7
    SDL_SCANCODE_S, // 8
    SDL_SCANCODE_D, // 9
    SDL_SCANCODE_Z, // A
    SDL_SCANCODE_C, // B
    SDL_SCANCODE_4, // C
    SDL_SCANCODE_R, // D
    SDL_SCANCODE_F, // E
    SDL_SCANCODE_V, // F
};

void updateKeys(CPU &cpu) {
  const bool *keys{SDL_GetKeyboardState(nullptr)};
  for (std::size_t i{}; i < NUM_KEYS; ++i) {
    cpu.setKey(i, keys[KEYMAP[i]]);
  }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  CPU cpu{};
  // std::filesystem::path romFilepath{"rom/2-ibm-logo.ch8"};
  //  std::filesystem::path romFilepath{"rom/3-corax+.ch8"};
  //  std::filesystem::path romFilepath{"rom/4-flags.ch8"};
  //  std::filesystem::path romFilepath{"rom/5-quirks.ch8"};
  std::filesystem::path romFilepath{"rom/6-keypad.ch8"};
  // std::filesystem::path romFilepath{"rom/7-beep.ch8"};
  // std::filesystem::path romFilepath{"rom/8-scrolling.ch8"};
  if (!cpu.loadROM(romFilepath)) {
    return 1;
  }
  Renderer renderer{};

  bool running{true};

  SDL_Event event;
  auto handleEvent = [&running, &cpu](const SDL_Event &event) {
    switch (event.type) {
    case SDL_EVENT_QUIT:
      running = false;
      break;
    }
  };

  while (running) {
    while (SDL_PollEvent(&event)) {
      handleEvent(event);
    }
    updateKeys(cpu);
    cpu.cycle();
    renderer.Render(cpu.getDisplay());
    cpu.updateTimers();
    SDL_Delay(static_cast<Uint32>(1000.0 / FREQUENCY));
  }

  SDL_Quit();

  return 0;
}