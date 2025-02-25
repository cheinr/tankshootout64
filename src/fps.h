#ifndef __FPS_H__
#define __FPS_H__

void fps_tick();
void fps_draw(display_context_t* disp);
uint32_t fps_get_tick_delta_useconds();

#endif
