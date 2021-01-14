//
// Created by Shimmy on 8/23/2018.
//
/**
 * @file calculator.c
 * @author  Shimon Balsam <shimon.balsam@mail.huji.ac.il>
 * @version 1.0
 * @date 26 Aug 2018
 *
 * @brief System to calculate a given grid's heat map.
 *
 * @section DESCRIPTION
 * The system calculates a given grid's heat map.
 * Input  : Grid, array of starting points, grid size, termination border, cyclic calculation
 * manner and heat calculation function (based on neighbors).
 * Process: runs the heat calculation function based on given inpout coordinates etc.
 * Output : a double representing the differnce between final round's heat calcualion to its most
 * previous round's calculation.
 */

// ------------------------------ includes ------------------------------
#include "calculator.h"
#include <math.h>
#include <stdio.h>

// -------------------------- const definitions -------------------------

// @brief A macro that sets the boolean value if a point on grid is a source point.
#define IS_SOURCE 1

// @brief A macro that sets the boolean value if a point on grid is not a source point.
#define IS_NOT_SOURCE 0

// ------------------------------ functions -----------------------------

/**
 * checks the bordering neighbors at edges of grid, based on cyclic manner and calculates cell's
 * heat accordingly
 * @param is_cyclic binary flag describing the cyclic manner of the grid.
 * @param n number of grid's rows.
 * @param m number of grid's columns.
 * @param i the row of the cell that needs to be calculated.
 * @param j the column of the cell that needs to be calculated.
 * @param grid the grid.
 * @param function the heat calculation function (based on neighbors).
 * @return cell's heat
 */
double cellHelper(int is_cyclic, size_t n, size_t m, const int i, const int j,
                  double **grid, diff_func function)
{
    double top = grid[(i - 1 + n) % n][j];
    double bottom = grid[(i + 1) % n][j];
    double right = grid[i][(j + 1) % m];
    double left = grid[i][(j - 1 + m) % m];
    if (!is_cyclic)
    {
        if (i == 0)
        {
            top = 0;
        }
        if (i == (int)n -1)
        {
            bottom = 0;
        }
        if (j == (int)m -1)
        {
            right = 0;
        }
        if (j == 0)
        {
            left = 0;
        }
    }
    return function(grid[i][j], right, top, left, bottom);
}

/**
 * checks if given cell holds a source point
 * @param sources array of starting source points.
 * @param num_sources amount of sources, equivalently size of sources array.
 * @param i the row of cell being checked.
 * @param j the column of cell being checked.
 * @return 1 if cell is souce point, 0 otherwise.
 */
int sourceCheck(source_point *sources, size_t num_sources, const int i, const int j)
{
    for (int m = 0; m < (int)num_sources; m++)
    {
        if (sources[m].x == i && sources[m].y == j)
        {
            return IS_SOURCE;
        }
    }
    return IS_NOT_SOURCE;
}

/**
 * updates the cell's current heat level calc
 * @param i the row of the cell
 * @param j the column of the cell
 * @param grid the grid
 * @param current the current heat level of cell before update
 * @param sources array of starting source points.
 * @param num_sources amount of sources, equivalently size of sources array.
 * @param is_cyclic binary flag describing the cyclic manner of the grid.
 * @param n number of grid's rows.
 * @param m number of grid's columns.
 * @param function the heat calculation function (based on neighbors).
 * @return the current heat level after activating the neighbored based heat function.
 */
double currentUpdate(const int i, const int j, double **grid, double current, source_point *sources,
                     const size_t num_sources, const int is_cyclic, const size_t n, const size_t m,
                     diff_func function)
{
    if(!sourceCheck(sources, num_sources, i, j))
    {
        grid[i][j] = cellHelper(is_cyclic, n, m, i, j, grid, function);
        current += grid[i][j];
    }
    return current;
}

/**
 * calculates the grids heat
 * @param function the heat calculation function (based on neighbors).
 * @param grid the grid.
 * @param n number of grid's rows.
 * @param m number of grid's columns.
 * @param sources array of starting source points.
 * @param num_sources amount of sources, equivalently size of sources array.
 * @param terminate border for heat difference
 * @param n_iter number of itterartions for calculation
 * @param is_cyclic binary flag describing the cyclic manner of the grid.
 * @return the heat level of the grid, more specifically, the differnce between thr final round's
 * calculation and the calculation of the round before last.
 */
double calculate(diff_func function, double **grid, size_t n, size_t m, source_point *sources,
                 size_t num_sources, double terminate, unsigned int n_iter, int is_cyclic)
{
    double previous = 0, current = terminate;
    if (n_iter > 0)
    {
        for(int l = 0; l < (int)n_iter; l++)
        {
            previous = current;
            current = 0;
            for(int i = 0; i < (int)n; i++)
            {
                for(int j = 0; j < (int)m; j++)
                {
                    current = currentUpdate(i, j, grid, current, sources, num_sources, is_cyclic,
                                            n, m, function);
                }
            }
        }
    }
    else
    {
        while(fabs(current - previous) >= terminate)
        {
            previous = current;
            current = 0;
            for(int i = 0; i < (int)n; i++)
            {
                for(int j = 0; j < (int)m; j++)
                {
                    current = currentUpdate(i, j, grid, current, sources, num_sources, is_cyclic,
                                            n, m, function);
                }
            }
        }
    }
    return fabs(current - previous);
}