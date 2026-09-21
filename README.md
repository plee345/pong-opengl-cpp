# Pong — C++ and OpenGL

A local two-player Pong game built in C++17 with OpenGL. Players control opposing paddles to return the ball; missing a return awards the opponent a point and starts a new rally. Scores appear in the terminal.

This project demonstrates a game loop, keyboard input, shader-based rendering, axis-aligned bounding-box collision checks, and score tracking. Optional debug output shows paddle positions and ball movement.

## Technology

- **C++17:** game state, input handling, collision detection, and scoring.
- **OpenGL 3.3 / GLSL:** vertex buffers and shaders for rendering the paddles and ball.
- **GLFW:** window creation, the OpenGL context, and keyboard input; version 3.4 is bundled.
- **GLAD:** OpenGL function loading.
- **Clang and Make:** compilation and build commands.

## Local setup (macOS)

The current Makefile and bundled GLFW library target **Apple Silicon Macs (arm64) running macOS 15 or later**. Other platforms require changes to the library and linker configuration.

1. Install the Xcode Command Line Tools if needed, then complete the installer:

   ```bash
   xcode-select --install
   ```

2. Install [Homebrew](https://brew.sh/) using its default Apple Silicon location, then install [GLFW](https://formulae.brew.sh/formula/glfw):

   ```bash
   brew install glfw
   ```

   The bundled library references `/opt/homebrew/opt/glfw/lib/libglfw.3.dylib` at runtime, so this installation is required by the current build.

3. Clone, build, and run:

   ```bash
   git clone https://github.com/plee345/pong-opengl-cpp.git
   cd pong-opengl-cpp
   make
   make run
   ```

Run from the repository directory so the game can load `shader.vs` and `shader.fs`. Keep the terminal open to see scores. Use `make clean` to remove build output, or `make all` to clean, rebuild, and launch.

## Controls

| Action | Keys |
| --- | --- |
| Move left paddle | W / S |
| Move right paddle | Up / Down arrows |
| Toggle terminal debug output | D |
| Quit | Escape |

Debug output starts disabled and prints at most four snapshots per second when enabled.

## Code guide

- `main.cpp`: game loop, controls, collision handling, goal detection, and terminal output.
- `shader.vs` / `shader.fs`: vertex and fragment shaders.
- `shader_s.h`: shader loading and compilation helper used by the build.
- `glad.c` / `dependencies/`: OpenGL loader and bundled library files.
