#include <stdio.h>
#include <stdlib.h>

#include <libdragon.h>

#include "game.h"
#include "tank.h"
#include "physics.h"
#include "fps.h"
#include "ui.h"
#include "string.h"

const uint32_t DOUBLE_BUFFERING = 2;

const char COLOR_BLUE[] = "blue";
const char COLOR_GREEN[] = "green";
const char COLOR_RED[] = "red";
const char COLOR_YELLOW[] = "yellow";

volatile static int viArrived = 0;
void on_vi_arrived() {
  viArrived = 1;
}

sprite_t* bgSprite;

void load_bg_sprite() {
  int fp = dfs_open("/tilegrid32.sprite");
  bgSprite = malloc(dfs_size(fp));
  dfs_read(bgSprite, 1, dfs_size(fp), fp);
  dfs_close(fp);
}

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

  load_bg_sprite();

  // world coordinates happen to be the same as screen coordinates
  physics_scene_init(resolution_x, resolution_y);

  // Allows debugf statements to show up in emulator logs
  debug_init_isviewer();

  game_t* game = game_init();

  tank_t* tanks[4];
  memset(tanks, 0, sizeof(tanks));

  debugf("Starting main loop!\n");

  register_VI_handler(on_vi_arrived);

  /* Main loop test */
  while (1) {

    while(!viArrived) { }
    viArrived = 0;

    if (game->state == INITIALIZING) {

      physics_scene_free();
      physics_scene_init(resolution_x, resolution_y);

      for (int i = 0; i < 4; i++) {
        if (tanks[i] != NULL) {
          tank_free(tanks[i]);
        }
      }

      tanks[0] = tank_init(32, 32, 45, COLOR_BLUE);
      tanks[1] = tank_init(resolution_x - 32, 32, 135, COLOR_RED);
      tanks[2] = tank_init(32, resolution_y - 32, 315, COLOR_GREEN);
      tanks[3] = tank_init(resolution_x - 32, resolution_y - 32, 225, COLOR_YELLOW);

      // TODO - Move into tank.c
      physics_scene_add_entity(&tanks[0]->physicsEntity);
      physics_scene_add_entity(&tanks[1]->physicsEntity);
      physics_scene_add_entity(&tanks[2]->physicsEntity);
      physics_scene_add_entity(&tanks[3]->physicsEntity);

      // init fps last so timing doesn't account for all the loading
      // we've just done
      fps_init();

      ui_init();
      game_start();
    }

    controller_scan();
    const SI_controllers_state_t controllers = get_keys_pressed();

    fps_tick(&controllers.c[0]);

    uint32_t timeDeltaUSeconds = fps_get_tick_delta_useconds();

    game_tick(timeDeltaUSeconds, tanks);

    int controllersPresent = get_controllers_present();

    if (game->state == RUNNING || game->state == ENDED) {
      tank_tick(tanks[0],
                controllersPresent & CONTROLLER_1_INSERTED,
                &controllers.c[0],
                timeDeltaUSeconds);
      tank_tick(tanks[1],
                controllersPresent & CONTROLLER_2_INSERTED,
                &controllers.c[1],
                timeDeltaUSeconds);
      tank_tick(tanks[2],
                controllersPresent & CONTROLLER_3_INSERTED,
                &controllers.c[2],
                timeDeltaUSeconds);
      tank_tick(tanks[3],
                controllersPresent & CONTROLLER_4_INSERTED,
                &controllers.c[3],
                timeDeltaUSeconds);

      physics_scene_tick(timeDeltaUSeconds);

    }

    ui_tick(game, timeDeltaUSeconds);

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

    // Draw the background
    rdp_enable_texture_copy();
    rdp_load_texture(0, 0, MIRROR_DISABLED, bgSprite);
    rdp_sync(SYNC_PIPE);
    rdp_draw_textured_rectangle(0, 0, 0, resolution_x, resolution_y, MIRROR_DISABLED);

    // =============================
    // Step 2: Draw The Tank
    // =============================

    tank_draw_body(tanks[0]);
    tank_draw_body(tanks[1]);
    tank_draw_body(tanks[2]);
    tank_draw_body(tanks[3]);

    tank_draw_barrel(tanks[0]);
    tank_draw_barrel(tanks[1]);
    tank_draw_barrel(tanks[2]);
    tank_draw_barrel(tanks[3]);

    ui_draw();

    rdp_detach_display();

    fps_draw(&disp);

    display_show(disp);
  }
}
