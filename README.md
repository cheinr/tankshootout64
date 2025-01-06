# Tank Shootout 64

## Purpose
This project is an attempt to create a simple multiplayer game for the Nintendo
64 that is free of proprietary Nintendo software, and thus can be shared freely. To
accomplish this, it will use the open-source [libdragon](https://github.com/DragonMinded/libdragon) sdk,
along with libdragon's [open-source bootcode](https://github.com/DragonMinded/libdragon/tree/trunk/boot) (ipl3).

The project also carries an additional unique constraint that it should be
able to run on less accurate emulators with minimal changes for demo purposes,
with mupen64plus using the GLideN64 video plugin being the main target emulator.
This poses some severe challenges, which are discussed in the "Achieving Emulator Compatibility"
section.

## The Game

### Functional Requirements

1. The game is to be a 2d top-down tank shooter
2. It will initially support 4 players
3. Players will be able to move and rotate their tank with the analog stick
4. Players will be able to fire projectiles from their tank that can destroy other player's tanks
5. There will be collision detection to prevent tanks from overlapping
 
### Deliberate Functional Constraints

1. The tank barrel cannot rotate independently from its body
2. At least initially, the arena the tanks are in will be a single screen frame. There will be no "scrolling"/"camera" functionality


### Achieving Emulator Compatability

The libdragon SDK has evolved to use the Nintendo 64 hardware in ways that the original
Nintendo 64 SDK didn't. This means that most games made today using libdragon will fail
to execute on nearly all but the most accurate of emulators.

Our main strategy for achieving emulator compatibility for this project is to use an older
version of the libdragon sdk, which is included in this repo as a git submodule. This version
is known to be able to produce 2d graphics that can work on the mupen64plus emulator with minimal
changes based on the fact that it was used for [meeq's N64 flappy bird port](https://github.com/meeq/FlappyBird-N64).

Of particular note, the chosen version of the libdragon SDK does not offer the improved "rdpq_"
functions for utilizing the RDP for graphics processing, and we must use the older limited "rdp_"
functions for hardware-accelerated graphics processing.

## Building

### Set up libdragon
1. See https://github.com/DragonMinded/libdragon/wiki/Installing-libdragon
    1. Option 2 is seemingly more flexible for use with our version of libdragon. Make sure to use the copy of libdragon present in this repo


#### Building

```
make
```

## Development Resources
- https://n64squid.com/homebrew/libdragon/
    - https://n64squid.com/homebrew/libdragon/graphics/hardware/legacy/



