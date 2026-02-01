#pragma once

#include <cstdio>
#include <iostream>
#include <vector>

namespace gol {

class Grid

{

public:
  Grid(int width, int height);
  void Print();
  void Step();

  void SetCell(int x, int y, bool state);
  void toggleCell(int x, int y);

  bool getCell(int x, int y);
  std::vector<bool> states;

private:
  int width;
  int height;

  int SumNeighbour(int x, int y);
};
} // namespace gol
