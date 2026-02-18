#include "gameoflife.h"
#include <iostream>
#include <vector>

namespace gol {

/*****************************************************************
 * Generalized Game of Life.
 *
 * This implementation allows to change the rules of game of
 * life where the constraints are that the state of a cell in
 * the next step is only determined by the number of alive
 * neighbour and the current state of the cell.
 *
 * Any such rule can be represented by 18 bits where the first 9
 * bits represent the state current cell is alive and next 9 bit
 * represent the state current cell is dead. The i th bit in any
 * state represent the next step when when there are exactly i
 * neighbours alive.
 *
 * For example the classic game of life rule can be represented
 * as 000001000000001100.
 ****************************************************************/
Grid::Grid(int width, int height, Rule rule)
    : width(width), height(height),
      states(width * height, false), rule(rule) {}
void Grid::SetCell(int x, int y, bool state) {
  states[y * width + x] = state;
}
void Grid::Print() {

  for (int j = 0; j < width; j++) {
    std::cout << "-";
  }
  for (int i = 0; i < width * height; i++) {
    bool state = states[i];
    if (state) {
      std::cout << "#";
    } else {
      std::cout << " ";
    }
    if (i % width == 0) {
      std::cout << "\n";
    }
  }

  std::cout << "\n";
  for (int j = 0; j < width; j++) {
    std::cout << "#";
  }

  std::cout << std::endl;

  std::cout << "Rule: ";
  for (int a = 0; a < 18; a++) {
    std::cout << rule.at(a) << " ";
  }

  std::cout << std::endl;
  std::cout << "      ";
  for (int a = 0; a < 10; a++) {
    std::cout << a << " ";
  }
  for (int a = 10; a < 18; a++) {
    std::cout << a;
  }

  std::cout << std::endl;
}
bool Grid::getCell(int x, int y) {

  return states[y * width + x];
}
void Grid::toggleCell(int x, int y) {

  SetCell(x, y, !getCell(x, y));
}
void Grid::Step() {
  std::vector<bool> newStates(width * height);

#pragma omp parallel for
  for (int yi = 0; yi < height; yi++) {
    for (int xi = 0; xi < width; xi++) {
      bool state = states[yi * width + xi];
      if (xi < 0 || yi < 0 || yi >= height || xi >= width) {
        continue;
      }
      int sum = SumNeighbour(xi, yi);
      int index = sum;
      if (!state) {
        index += 9;
      }

      if (rule.at(index)) {
        newStates[yi * width + xi] = true;
      } else {
        newStates[yi * width + xi] = false;
      }
    }
  }
  states = newStates;
}

int Grid::SumNeighbour(int x, int y) {
  int sum = 0;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      int xi = x + j;
      int yi = y + i;
      if ((j == 0 && i == 0) || xi < 0 || yi < 0 ||
          yi >= height || xi >= width) {
        continue;
      }
      sum += states[(yi)*width + (xi)];
    }
  }
  return sum;
}
} // namespace gol
