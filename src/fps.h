#ifndef __FPS_H__
#define __FPS_H__

void fps_init();
void fps_tick(const struct SI_condat *gamepad);
void fps_draw(display_context_t* disp);
uint32_t fps_get_tick_delta_useconds();

#endif
