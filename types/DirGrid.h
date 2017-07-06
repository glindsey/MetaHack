#pragma once

#include "types/Direction.h"
#include "types/Vec2.h"

/// A DirGrid is simply a map of a certain type to directions.
template <typename T>
using DirGrid = std::map<DirectionIndex, T>;

/// Given the four corners (NW/NE/SE/SW), interpolate the rest of the grid.
template <typename T>
void interpolateFromCorners(DirGrid<T>& grid)
{
  grid[Direction::Center] = (grid[Direction::Northwest] + grid[Direction::Northeast] + grid[Direction::Southeast] + grid[Direction::Southwest]) / 4;
  grid[Direction::North] = (grid[Direction::Northwest] + grid[Direction::Northeast]) / 2;
  grid[Direction::East] = (grid[Direction::Northeast] + grid[Direction::Southeast]) / 2;
  grid[Direction::South] = (grid[Direction::Southwest] + grid[Direction::Southeast]) / 2;
  grid[Direction::West] = (grid[Direction::Northwest] + grid[Direction::Southwest]) / 2;
}

/// Given the upper corner, a horizontal perimeter amount, and a vertical perimeter amount, interpolate a 3x3 grid.
template <typename T>
DirGrid<T> interpolateFromNWCorner(T nwCorner, T horizAdd, T vertAdd)
{
  DirGrid grid;
  grid[Direction::Northwest] = nwCorner;
  grid[Direction::Northeast] = nwCorner + horizAdd;
  grid[Direction::Southeast] = nwCorner + horizAdd + vertAdd;
  grid[Direction::Southwest] = nwCorner + vertAdd;
  interpolateFromCorners(grid);
  return grid;
}

/// Given the center, a horizontal perimeter amount, and a vertical perimeter amount, interpolate a 3x3 grid.
template <typename T>
DirGrid<T> interpolateFromCenter(T center, T horizAdd, T vertAdd)
{
  DirGrid grid;
  T halfHoriz = horizAdd / 2;
  T halfVert = vertAdd / 2;
  grid[Direction::Center] = center;
  grid[Direction::Northwest] = center - halfVert - halfHoriz;
  grid[Direction::North    ] = center - halfVert;
  grid[Direction::Northeast] = center - halfVert + halfHoriz;
  grid[Direction::East     ] = center            + halfHoriz;
  grid[Direction::Southeast] = center + halfVert + halfHoriz;
  grid[Direction::South    ] = center + halfVert;
  grid[Direction::Southwest] = center + halfVert - halfHoriz;
  grid[Direction::West     ] = center            - halfHoriz;
  return grid;
}