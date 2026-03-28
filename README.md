# AstroJumps

A keyboard-driven space platformer built with C and raylib. Jump higher and higher through the cosmos — but watch out, the controls shuffle every milestone.

## High Score Screenshots (Can you beat us? 🤔)


## Features

- **Bouncing physics:** Land on a platform and you're launched back up.
- **Key shuffle:** Every milestone your left/right keys are randomly reassigned. Stay sharp.
- **Platform variety:** Eight platform types that unlock progressively such as flickers, movers, teleporters, and more!
- **Powerups:** Jetpack, Revive, Boots, Elixir, Feather.
- **Random events:** Rising lava, wind gusts, flash blindness, blackout, and screen inversion.
- **Space aesthetic:** Pure black background, neon-lit platforms, starfield feel.

## Dependencies

- `raylib` — must be installed system-wide
- `gcc` — C17-compatible

#### Arch Linux

```bash
sudo pacman -S raylib gcc make
```

#### Ubuntu / Debian

```bash
sudo apt install libraylib-dev gcc make
```

#### Windows (MinGW64)

Install [MSYS2](https://www.msys2.org/), then from the **MinGW64 terminal**:
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-raylib make
```

## Build from Source

```bash
make
```

### Debug build

```bash
make debug
```

### Clean

```bash
make clean
```

## Platform types
| Color | Type | Behavior |
|-------|------|----------|
| Blue | Normal | Standard solid platform |
| Dotted blue | Fragile | Breaks on first contact |
| Green | Flicker | Phases in and out |
| Yellow | Flicker H | Flickers + moves horizontally |
| Orange | Flicker V | Flickers + moves vertically |
| Cyan | Moving H | Slides horizontally |
| Red | Moving HV | Slides in both axes |
| Purple | Teleport | Jumps to a random screen position periodically |

## Powerups

| Icon | Powerup |
|------|---------|
| Jetpack | Continuous upward flight for 5 s |
| Revive | One automatic revive on death |
| Boots | <kbd>SPACE</kbd> for a double jump (15 s window) |
| Elixir | Widens hitbox for 10 s |
| Feather | Slows gravity for 10 s |

## Project structure

```
astrojumps/
├── Makefile
├── assets/                   — Game's textures and sound effects
├── include/
│   ├── controls.h            — Controls struct, key shuffle
│   ├── debug.h               — debug panel (DEBUG_BUILD only)
│   ├── effects.h             — ActiveEffects (transient powerup state)
│   ├── events.h              — EventSystem, EventType enum
│   ├── game.h                — Game struct, screen/fps constants
│   ├── platform.h            — Platform, PlatformList, PlatformType
│   ├── player.h              — Player struct, physics constants
│   └── powerup.h             — PowerupSystem, powerup durations
└── src/
    ├── main.c                — window init, icon load, game loop
    ├── controls.c            — key binding, shuffle, HUD
    ├── debug.c               — debug panel (compiled out in release)
    ├── events.c              — event trigger, update, draw overlays
    ├── game.c                — state machine, score, cleanup
    ├── platform.c            — platform spawn, update, draw, collision
    ├── player.c              — physics, texture swap, powerup FX
    └── powerup.c             — world powerup spawn, pickup, HUD timers
```
