#include <stdbool.h>
#include <math.h>
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

  c->hHalls = (HallArray){0};
  c->vHalls = (HallArray){0};
  return c;
}


void drawHall(Hall* hall){
  DrawRectangleLines(hall->x1, hall->y1, hall->x2 - hall->x1, hall->y2 - hall->y1, YELLOW);
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

    // Draw horizontal halls
    if(cell->hHalls.items) {
      for(size_t i = 0; i < cell->hHalls.count; i++)
        drawHall(&cell->hHalls.items[i]);
    }

    // Draw vertical halls
    if(cell->vHalls.items) {
      for(size_t i = 0; i < cell->vHalls.count; i++)
        drawHall(&cell->vHalls.items[i]);
    }
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


void getLeaves(Cell* cell, CellArray* cells) {
    if (!cell) return;

    if (cell->left != NULL) {
        getLeaves(cell->left, cells);
        getLeaves(cell->right, cells);
    } else {
        da_append(cells, cell);
    }
}

#define EPSILON 0.01f

void findNeighbours(Map* map) {
  getLeaves(&map->root, &map->cells);

  for (size_t i = 0; i < map->cells.count; i++) {

    map->cells.items[i]->hNeighbours = (CellArray){0};
    map->cells.items[i]->vNeighbours = (CellArray){0};

    for (size_t j = 0; j < map->cells.count; j++) {
      if (i == j) continue;

      if (fabs(map->cells.items[i]->x2 - map->cells.items[j]->x1) < EPSILON) {
        if (MAX(map->cells.items[i]->y1, map->cells.items[j]->y1) <
          MIN(map->cells.items[i]->y2, map->cells.items[j]->y2)) {
          da_append(&map->cells.items[i]->hNeighbours, map->cells.items[j]);
        }
      }

      if (fabs(map->cells.items[i]->y2 - map->cells.items[j]->y1) < EPSILON) {
        if (MAX(map->cells.items[i]->x1, map->cells.items[j]->x1) <
          MIN(map->cells.items[i]->x2, map->cells.items[j]->x2)) {
          da_append(&map->cells.items[i]->vNeighbours, map->cells.items[j]);
        }
      }
    }
  }
}

void shrinkCells(Cell* cell, uint8_t minCellSize){
  if(cell->left!=NULL){
    shrinkCells(cell->left, minCellSize);
    shrinkCells(cell->right, minCellSize);
  }

  float w = cell->x2-cell->x1;
  float h = cell->y2-cell->y1;
  float newW = MAX(w*RANDBETWEEN(3,9)/10, minCellSize);
  float newH = MAX(h*RANDBETWEEN(3,9)/10, minCellSize);

  cell->x1 = cell->x1 + 0.5*(w-newW);
  cell->x2 = cell->x2 - 0.5*(w-newW);
  cell->y1 = cell->y1 + 0.5*(h-newH);
  cell->y2 = cell->y2 - 0.5*(h-newH);
}

void makeHalls(Map* map) {
  for (size_t i = 0; i < map->cells.count; i++) {
    Cell *cell = map->cells.items[i];

    cell->hHalls = (HallArray){0};
    cell->vHalls = (HallArray){0};

    // Horizontal halls
    for (size_t j = 0; j < cell->hNeighbours.count; j++) {
      Cell *neighbour = cell->hNeighbours.items[j];

      float y_min = MAX(cell->y1, neighbour->y1);
      float y_max = MIN(cell->y2, neighbour->y2) - map->minCellSize;

      if (y_max > y_min) {
        float y = RANDBETWEENF(y_min, y_max);
        Hall hall = { cell->x2, y, neighbour->x1, y + map->minCellSize };
        da_append(&cell->hHalls, hall);
      }
    }

    // Vertical halls
    for (size_t j = 0; j < cell->vNeighbours.count; j++) {
      Cell *neighbour = cell->vNeighbours.items[j];

      float x_min = MAX(cell->x1, neighbour->x1);
      float x_max = MIN(cell->x2, neighbour->x2) - map->minCellSize;

      if (x_max > x_min) {
        float x = RANDBETWEENF(x_min, x_max);
        Hall hall = { x, cell->y2, x + map->minCellSize, neighbour->y1 };
        da_append(&cell->vHalls, hall);
      }
    }
  }
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

void generateMap(Map* map) {
  devideMap(map);
  findNeighbours(map);
  shrinkCells(&map->root, map->minCellSize);
  makeHalls(map);
}


void drawMap(Map* map) {
  if (!map) return;

  drawCell(&map->root);
}
