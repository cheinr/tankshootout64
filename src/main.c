#include <stdio.h>
#include <stdlib.h>

#include <libdragon.h>

#include "tank.h"
#include "physics.h"

static volatile uint32_t animcounter = 0;

uint32_t entityIdCounter = 0;

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

  // world coordinates happen to be the same as screen coordinates
  physics_scene_init(resolution_x, resolution_y);

  // Allows debugf statements to show up in emulator logs
  debug_init_isviewer();

  tank_t* tank1 = tank_init(entityIdCounter++, 20, 30);
  tank_t* tank2 = tank_init(entityIdCounter++, 120, 30);
  tank_t* tank3 = tank_init(entityIdCounter++, 20, 130);
  tank_t* tank4 = tank_init(entityIdCounter++, 120, 130);

  physics_scene_add_entity(&tank1->physicsEntity);
  physics_scene_add_entity(&tank2->physicsEntity);
  physics_scene_add_entity(&tank3->physicsEntity);
  physics_scene_add_entity(&tank4->physicsEntity);

  /* Kick off animation update timer to fire thirty times a second */
  new_timer(TIMER_TICKS(1000000 / 30), TF_CONTINUOUS, update_counter);

  debugf("Starting main loop!\n");
  
  /* Main loop test */
  while (1) {

    controller_scan();
    const SI_controllers_state_t controllers = get_keys_pressed();

    tank_tick(tank1, animcounter, &controllers.c[0]);
    tank_tick(tank2, animcounter, &controllers.c[1]);
    tank_tick(tank3, animcounter, &controllers.c[2]);
    tank_tick(tank4, animcounter, &controllers.c[3]);

    physics_scene_tick();

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
