#include <libdragon.h>
#include <assert.h>

uint64_t frameCount = 0;
uint64_t lastFrameTimeUSeconds = 0;
uint32_t frameTimeUSecondsSamples[10];
uint32_t numFrameTimeSamples = 0;

uint32_t cUpPressed = 0;
uint32_t drawFPS = 0;

void fps_init() {
  uint64_t timerTicks = timer_ticks();
  uint64_t frameTimeUSeconds = TIMER_MICROS_LL(timerTicks);

  lastFrameTimeUSeconds = frameTimeUSeconds;
}


void fps_tick(const struct SI_condat *gamepad) {
  frameCount++;
  uint64_t timerTicks = timer_ticks();
  uint64_t frameTimeUSeconds = TIMER_MICROS_LL(timerTicks);

  if (numFrameTimeSamples >= 10) {
    for (int i = 0; i < 10 - 1; i++) {
      frameTimeUSecondsSamples[i] = frameTimeUSecondsSamples[i+1];
    }
    numFrameTimeSamples -= 1;
  }

  frameTimeUSecondsSamples[numFrameTimeSamples++] = frameTimeUSeconds - lastFrameTimeUSeconds;

  lastFrameTimeUSeconds = frameTimeUSeconds;

  if (!cUpPressed && gamepad->C_up) {
    drawFPS = !drawFPS;
  }

  cUpPressed = gamepad->C_up;
}

uint32_t fps_get_tick_delta_useconds() {
  assert(numFrameTimeSamples > 0);
  return frameTimeUSecondsSamples[numFrameTimeSamples - 1];
}

static float calculate_average_fps() {
  uint64_t totalSampleTime = 0;
  for (int i = 0; i < numFrameTimeSamples; i++) {
    totalSampleTime += frameTimeUSecondsSamples[i];
  }

  uint32_t averageFrameTimeUSeconds = (totalSampleTime / numFrameTimeSamples);

  float averageFrameTimeSeconds = averageFrameTimeUSeconds / 1000000.0;
  return 1.0 / averageFrameTimeSeconds;
}

void fps_draw(display_context_t* disp) {

  if (!drawFPS) {
    return;
  }

  float averageFPS = calculate_average_fps();

  char fpsDisplay[12];
  sprintf(fpsDisplay, "FPS: %05.2f", averageFPS);

  // Hack to get software text working on gliden64
  graphics_draw_box(*disp, 0, 1, 90, 10, graphics_make_color(0, 0, 0, 0));

  graphics_draw_text(*disp, 5, 2, fpsDisplay);
}
