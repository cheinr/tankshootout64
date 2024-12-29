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

  /* Initialize subsystems */
  display_init(RESOLUTION_640x480,
           DEPTH_16_BPP,
           DOUBLE_BUFFERING,
           GAMMA_NONE,
           ANTIALIAS_RESAMPLE_FETCH_ALWAYS);
  dfs_init(DFS_DEFAULT_LOCATION);
  rdp_init();
  controller_init();
  timer_init();
  
  // Get the file handle for our sprite
  int fp = dfs_open("/redtank64.sprite");
  // Allocate a buffer for our file
  sprite_t *redtank = malloc(dfs_size(fp));
  // Load the sprite into the buffer
  dfs_read(redtank, 1, dfs_size(fp), fp);
  // Reading is complete, close the file
  dfs_close(fp);

  /* Kick off animation update timer to fire thirty times a second */
  new_timer(TIMER_TICKS(1000000 / 30), TF_CONTINUOUS, update_counter);
  
  /* Main loop test */
  while (1) {

    while(!(disp = display_lock()));

    // Assure RDP is ready for new commands
    rdp_sync(SYNC_PIPE);

    // Remove any clipping windows
    rdp_set_default_clipping();

    // Enable sprite display instead of solid color fill
    rdp_enable_texture_copy();

    // Attach RDP to display
    rdp_attach_display(disp);
                    
    // Ensure the RDP is ready to receive sprites
    rdp_sync(SYNC_PIPE);

    char buffer[20];
    sprintf(buffer, "hslices: %d\n", redtank->hslices);
    graphics_draw_text( disp, 5, 5, buffer);
    sprintf(buffer, "vslices: %d\n", redtank->vslices);
    graphics_draw_text( disp, 5, 15, buffer);
    
    rdp_sync(SYNC_PIPE);
    rdp_load_texture_stride(0, 0, MIRROR_DISABLED, redtank, 0);
    rdp_draw_sprite(0, 20, 50, MIRROR_DISABLED);

    rdp_sync(SYNC_PIPE);
    rdp_load_texture_stride(0, 0, MIRROR_DISABLED, redtank, 1);
    rdp_draw_sprite(0, 20, 50 + redtank->height/2, MIRROR_DISABLED);

    rdp_detach_display();

    display_show(disp);
  }
}
