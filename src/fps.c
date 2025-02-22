#include <libdragon.h>

uint32_t frameCount = 0;
uint32_t lastFrameTimeUSeconds = 0;
uint32_t frameTimeUSecondsSamples[10];
uint32_t numFrameTimeSamples = 0;

void fps_tick() {
  frameCount++;
  uint32_t timerTicks = timer_ticks();
  uint64_t frameTimeUSeconds = TIMER_MICROS_LL(timerTicks);

  if (numFrameTimeSamples >= 10) {
    for (int i = 0; i < 10 - 1; i++) {
      frameTimeUSecondsSamples[i] = frameTimeUSecondsSamples[i+1];
    }
    numFrameTimeSamples -= 1;
  }

  frameTimeUSecondsSamples[numFrameTimeSamples++] = frameTimeUSeconds - lastFrameTimeUSeconds;

  lastFrameTimeUSeconds = frameTimeUSeconds;
}


static float calculate_average_fps() {
  uint32_t totalSampleTime = 0;
  for (int i = 0; i < numFrameTimeSamples; i++) {
    totalSampleTime += frameTimeUSecondsSamples[i];
  }

  uint32_t averageFrameTimeUSeconds = (totalSampleTime / numFrameTimeSamples);

  float averageFrameTimeSeconds = averageFrameTimeUSeconds / 1000000.0;
  return 1.0 / averageFrameTimeSeconds;
}

void fps_draw(display_context_t* disp) {
  float averageFPS = calculate_average_fps();

  char fpsDisplay[12];
  sprintf(fpsDisplay, "FPS: %05.2f", averageFPS);

  // Hack to get software text working on gliden64
  graphics_draw_box(*disp, 0, 1, 90, 10, graphics_make_color(0, 0, 0, 0));

  graphics_draw_text(*disp, 5, 2, fpsDisplay);
}
