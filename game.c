#include <raylib.h>

#include "./src/mapgen.c"
#include "./src/utils.h"

#define ROOMNUMBER  10
#define MINCELLSIZE  10

int main(void) {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rogventure");
  SetTargetFPS(60);

  srand(time(NULL)); 

  Map map = initMap(30, ROOMNUMBER, MINCELLSIZE);
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
