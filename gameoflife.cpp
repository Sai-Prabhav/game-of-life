#include "gameoflife.h"
#include <iostream>
#include <vector>

namespace gol {

Grid::Grid(int width, int height)
    : width(width), height(height),
      states(width * height, false) {}
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
      if (sum < 2 && state) {
        newStates[yi * width + xi] = false;
      } else if ((sum == 3 || sum == 2) && state) {
        newStates[yi * width + xi] = true;
      } else if (sum > 3) {
        newStates[yi * width + xi] = false;
      } else if (sum == 3 && !state) {
        newStates[yi * width + xi] = true;
      } else {
        newStates[yi * width + xi] = state;
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
