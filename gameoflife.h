#pragma once

#include <array>
#include <cstdio>
#include <iostream>
#include <vector>

namespace gol {

typedef std::array<bool, 18> Rule;
class Grid

{

public:
  Grid(int width, int height, Rule rule);
  void Print();
  void Step();

  void SetCell(int x, int y, bool state);
  void toggleCell(int x, int y);

  bool getCell(int x, int y);
  std::vector<bool> states;

private:
  int width;
  int height;
  Rule rule;
  int SumNeighbour(int x, int y);
};
} // namespace gol
