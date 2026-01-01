#ifndef MAPGEN_H_
#define MAPGEN_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct Cell Cell;

typedef struct {
  Cell **items;
  size_t count;
  size_t capacity;
} CellArray;

struct Cell {
  float x1, y1, x2, y2;
  Cell *left;
  Cell *right;
  CellArray hNeighbours;
  CellArray vNeighbours;
};

typedef struct {
  Cell root;
  uint8_t numRooms;
  uint8_t minCellSize;
  CellArray cells;
} Map;

Cell *makeCell(float x1, float y1, float x2, float y2);
void drawCell(Cell *cell);

Map initMap(uint16_t margin, uint8_t numRooms, uint8_t minCellSize);
void devideMap(Map *map);
bool devideRoot(Cell root, uint8_t minCellSize);

void generateMap(Map *map);
void drawMap(Map *map);

#endif // MAPGEN_H_
