#include <stdio.h>
#include <stdlib.h>

#include <libdragon.h>

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
  
  // Get the file handle for our sprite
  int fp = dfs_open("/redtank98.sprite");
  // Allocate a buffer for our file
  sprite_t *redtank = malloc(dfs_size(fp));
  // Load the sprite into the buffer
  dfs_read(redtank, 1, dfs_size(fp), fp);
  // Reading is complete, close the file
  dfs_close(fp);

  /* Kick off animation update timer to fire thirty times a second */
  new_timer(TIMER_TICKS(1000000 / 30), TF_CONTINUOUS, update_counter);

  debugf("Starting main loop!\n");
  
  /* Main loop test */
  while (1) {

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
    rdp_draw_filled_rectangle(0, 0, resolution_x, resolution_y);

    // =============================
    // Step 2: Draw The Tank
    // =============================

    // Assure RDP is ready for new commands
    rdp_sync(SYNC_PIPE);

    // Enable sprite display instead of solid color fill
    rdp_enable_texture_copy();

    int tankWidth = redtank->width/9;
    int hSlicesPerSprite = redtank->hslices/9; // TODO
    int degrees = (animcounter<<1) % 360;

    int spriteStart = (degrees % 90) * hSlicesPerSprite;
    int reverseDrawOrder = 0;
    int mirror;
    if (degrees < 90) {
      mirror = MIRROR_DISABLED;
    } else if (degrees < 180) {
      spriteStart = (90 - (degrees % 90) - 1) * hSlicesPerSprite;
      reverseDrawOrder = 1;
      mirror = MIRROR_X;
    } else if (degrees < 270) {
      mirror = MIRROR_XY;
      reverseDrawOrder = 1;
    } else {
      spriteStart = (90 - (degrees % 90) - 1) * hSlicesPerSprite;
      mirror = MIRROR_Y;
    }

    int i;
    for (i = 0; i < hSlicesPerSprite; i++) {

      int stride = reverseDrawOrder
        ? spriteStart + (hSlicesPerSprite - i - 1)
        : spriteStart + i;

      rdp_sync(SYNC_PIPE);
      rdp_load_texture_stride(0, 0, mirror, redtank, stride);
      rdp_draw_sprite(0, 20 + ((tankWidth/hSlicesPerSprite)*i), 50, mirror);
    }

    rdp_detach_display();

    display_show(disp);
  }
}
