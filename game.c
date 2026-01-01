#include <raylib.h>

#include "./src/mapgen.c"
#include "./src/utils.h"


int main(void) {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rogventure");
  SetTargetFPS(60);

  srand(time(NULL)); 

  Map map = initMap(30, 10, 10);
  generateMap(&map);
  
  while (!WindowShouldClose()) {
    BeginDrawing();
    drawMap(&map);

    ClearBackground(RED);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
