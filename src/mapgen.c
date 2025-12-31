#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "./constants.c"
#include "./mapgen.h"
#include "./utils.h"


Cell* makeCell(float x1, float y1, float x2, float y2) {
    Cell* c = malloc(sizeof(Cell));
    c->x1 = x1; c->y1 = y1; c->x2 = x2; c->y2 = y2;
    c->left = NULL; c->right = NULL;
    return c;
}


void drawCell(Cell* cell){
  if(cell->left != NULL){
    drawCell(cell->left);
    drawCell(cell->right);
  } else {
    DrawRectangleLinesEx(
      (Rectangle){ cell->x1, cell->y1, cell->x2 - cell->x1, cell->y2 - cell->y1 },
      1,
      GREEN
    );
  }
}


bool devideCell(Cell* cell, uint8_t minCellSize){

  float width = cell->x2-cell->x1;
  float height = cell->y2-cell->y1;
  if (width<minCellSize && height<minCellSize) { return false;}
  if (cell->left != NULL){
    if (rand() % 2){
      return devideCell(cell->left, minCellSize);
    } else {
      return devideCell(cell->right, minCellSize);
    }
  }

  if (width > height) {
    float mid = cell->x1 + (RANDBETWEEN(3, 6) / 10.0f) * width;
    cell->left  = makeCell(cell->x1, cell->y1, mid, cell->y2);
    cell->right = makeCell(mid, cell->y1, cell->x2, cell->y2);
    return true;
  } else {
    float mid = cell->y1 + (RANDBETWEEN(3, 6) / 10.0f) * height;
    cell->left  = makeCell(cell->x1, cell->y1, cell->x2, mid);
    cell->right = makeCell(cell->x1, mid, cell->x2, cell->y2);
    return true;
  }

  return false;
}


Map initMap(uint16_t margin, uint8_t numRooms, uint8_t minCellSize) {
  Map map = {
    .root = {
      .x1 = margin,
      .y1 = margin,
      .x2 = WINDOW_WIDTH - 2 * margin,
      .y2 = WINDOW_HEIGHT - 2 * margin,
      .left = NULL,
      .right = NULL
    }, 

    .numRooms = numRooms,
    .minCellSize = minCellSize,
  };

  //TODO: Add randomizer for rooms and minCellSize

  return map;
}


void devideMap(Map* map){
  size_t rooms = 1;
  while(rooms<map->numRooms){
    if(devideCell(&map->root, map->minCellSize)){rooms++;}
  }
}


void drawMap(Map* map) {
    if (!map) return;

    drawCell(&map->root);
}
