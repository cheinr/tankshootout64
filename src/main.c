#include <stdio.h>
#include <stdlib.h>

#include <libdragon.h>

#include "tank.h"
#include "physics.h"
#include "fps.h"

uint32_t entityIdCounter = 0;

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

  // world coordinates happen to be the same as screen coordinates
  physics_scene_init(resolution_x, resolution_y);

  // Allows debugf statements to show up in emulator logs
  debug_init_isviewer();

  tank_t* tank1 = tank_init(entityIdCounter++, 32, 32, 45);
  tank_t* tank2 = tank_init(entityIdCounter++, resolution_x - 32, 32, 135);
  tank_t* tank3 = tank_init(entityIdCounter++, 32, resolution_y - 32, 315);
  tank_t* tank4 = tank_init(entityIdCounter++, resolution_x - 32, resolution_y - 32, 225);

  physics_scene_add_entity(&tank1->physicsEntity);
  physics_scene_add_entity(&tank2->physicsEntity);
  physics_scene_add_entity(&tank3->physicsEntity);
  physics_scene_add_entity(&tank4->physicsEntity);

  debugf("Starting main loop!\n");

  /* Main loop test */
  while (1) {

    controller_scan();
    const SI_controllers_state_t controllers = get_keys_pressed();

    fps_tick(&controllers.c[0]);

    uint32_t timeDeltaUSeconds = fps_get_tick_delta_useconds();

    tank_tick(tank1, &controllers.c[0]);
    tank_tick(tank2, &controllers.c[1]);
    tank_tick(tank3, &controllers.c[2]);
    tank_tick(tank4, &controllers.c[3]);

    physics_scene_tick(timeDeltaUSeconds);

    while(!(disp = display_lock()));

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

    // Enable sprite display instead of solid color fill
    rdp_enable_texture_copy();

    tank_draw_body(tank1);
    tank_draw_body(tank2);
    tank_draw_body(tank3);
    tank_draw_body(tank4);

    tank_draw_barrel(tank1);
    tank_draw_barrel(tank2);
    tank_draw_barrel(tank3);
    tank_draw_barrel(tank4);

    rdp_detach_display();

    fps_draw(&disp);

    display_show(disp);
  }
}
