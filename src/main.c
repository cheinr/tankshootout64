#include <stdio.h>
#include <stdlib.h>

#include <libdragon.h>

#include "tank.h"

static volatile uint32_t animcounter = 0;


void update_counter( int ovfl )
{
    animcounter++;
}

const uint32_t DOUBLE_BUFFERING = 2;

int main(void) {
  display_context_t disp;

  int resolution_x = 320;
  int resolution_y = 240;

  /* Initialize subsystems */
  display_init(RESOLUTION_320x240,
           DEPTH_16_BPP,
           DOUBLE_BUFFERING,
           GAMMA_NONE,
           ANTIALIAS_RESAMPLE_FETCH_ALWAYS);
  dfs_init(DFS_DEFAULT_LOCATION);
  rdp_init();
  controller_init();
  timer_init();

  // Allows debugf statements to show up in emulator logs
  debug_init_isviewer();

  tank_t* tank1 = tank_init(20, 30);
  tank_t* tank2 = tank_init(120, 30);
  tank_t* tank3 = tank_init(20, 130);
  tank_t* tank4 = tank_init(120, 130);

  /* Kick off animation update timer to fire thirty times a second */
  new_timer(TIMER_TICKS(1000000 / 30), TF_CONTINUOUS, update_counter);

  debugf("Starting main loop!\n");
  
  /* Main loop test */
  while (1) {

    while(!(disp = display_lock()));


    tank_tick(tank1, animcounter);
    tank_tick(tank2, animcounter);
    tank_tick(tank3, animcounter);
    tank_tick(tank4, animcounter);

    // =============================
    // Step 1: Clear the Screen
    // =============================

    // Assure RDP is ready for new commands
    rdp_sync(SYNC_PIPE);
    // Remove any clipping windows
    rdp_set_default_clipping();
    // Attach RDP to display
    rdp_attach_display(disp);
    // Clear the screen with a solid color
    rdp_enable_primitive_fill();
    rdp_set_primitive_color(graphics_make_color(50, 168, 82, 255));
    rdp_draw_filled_rectangle(0, 0, resolution_x, resolution_y);

    // =============================
    // Step 2: Draw The Tank
    // =============================

    // Assure RDP is ready for new commands
    rdp_sync(SYNC_PIPE);

    // Enable sprite display instead of solid color fill
    rdp_enable_texture_copy();

    tank_draw(tank1);
    tank_draw(tank2);
    tank_draw(tank3);
    tank_draw(tank4);

    rdp_detach_display();

    display_show(disp);
  }
}
