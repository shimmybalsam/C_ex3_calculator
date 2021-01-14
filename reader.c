//
// Created by Shimmy on 8/23/2018.
//
/**
 * @file reader.c
 * @author  Shimon Balsam <shimon.balsam@mail.huji.ac.il>
 * @version 1.0
 * @date 26 Aug 2018
 *
 * @brief System to read a file, parse its given coordinates and return a heat map accordingly.
 *
 * @section DESCRIPTION
 * The system runs a single game of battleships.
 * Input  : Board size and itterative move input.
 * Process: Scans the input file line by line, builds a grid based on given size and starter
 * coordinates then runs calculate.c functions in order to calculate relevant heat map.
 * Output : Prints a grid of coordinates representing the needed heat map.
 */

// ------------------------------ includes ------------------------------
#include <stdio.h>
#include <memory.h>
#include "calculator.h"
#include "heat_eqn.h"

// -------------------------- const definitions -------------------------

// @brief A macro that sets the number of args needed as input.
#define NUM_OF_ARGS 2

// @brief A macro that sets the error message for wrong number of args given as input.
#define WRONG_AMNT_ARGS_MSG "Wrong amount of args given."

// @brief A macro that sets the error message for illegal file given as input.
#define INPUT_FILE_ERR "Can't read file."

// @brief A macro that sets the error message for wrong format of file given as input.
#define FORMAT_ERR_MSG "Invalid format of file"

// @brief A macro that sets separator line between file sections.
#define END_SECTION "----"

// @brief A macro that sets the error return value.
#define ERR_RETURN 1

// ------------------------------ functions -----------------------------

/**
 * Creates a grid.
 * @param n number of grid's rows.
 * @param m number of grid's columns.
 * @param sources array of starting source points.
 * @param numOfSources amount of sources, equivalently size of sources array.
 * @return a grid (well a pointer to a grid) with the given values in the sources point and 0
 * in all other points.
 */
double **makeGrid(const size_t n, const size_t m, source_point *sources, const size_t numOfSources)
{
    double **grid = (double**)malloc(n*sizeof(double*));
    if (grid == NULL)
    {
        return NULL;
    }
    for (int i = 0; i < (int)n; i++)
    {
        grid[i] = (double*)malloc(m*sizeof(double));
        if(grid[i] == NULL)
        {
            for (int k = 0; k < i; k++)
            {
                free(grid[k]);
            }
            return NULL;
        }
        for (int j = 0; j < (int)m; j++)
        {
            grid[i][j] = 0;
        }
    }
    for (int i = 0; i < (int)numOfSources; i++)
    {
        int x = sources[i].x;
        int y = sources[i].y;
        grid[x][y] = sources[i].value;
    }
    return grid;
}

/**
 * prints the grid.
 * @param n number of grid's rows.
 * @param m number of grid's columns.
 * @param grid the grid
 */
void printGrid(const size_t n, const size_t m, double **grid)
{
    for (int i = 0; i < (int)n; i++)
    {
        for (int j = 0; j < (int)m; j++)
        {
            printf("%.4f,", grid[i][j]);
        }
        printf("\n");
    }
}

/**
 * calculates and prints the grids heat map
 * @param grid the grid
 * @param n number of grid's rows.
 * @param m number of grid's columns.
 * @param function the heat calculation function.
 * @param sources array of starting source points.
 * @param numOfSources amount of sources, equivalently size of sources array.
 * @param is_cyclic binary flag telling if the grid should be calculated in a cyclic manner or not.
 * @param n_iter number of maximum iterations.
 * @param terminate border between each rounds calculation.
 */
void calcGrid(double **grid, const size_t n, const size_t m, diff_func function,
              source_point *sources, size_t numOfSources, const int is_cyclic,
              unsigned int n_iter, const double terminate)
{
    double temp = terminate;
    while (temp >= terminate)
    {
        temp = calculate(function, grid, n, m, sources, numOfSources, terminate, n_iter, is_cyclic);
        printf("%lf\n", temp);
        printGrid(n, m, grid);
    }
}

/**
 * Reads input file
 * @param argc num of args
 * @param argv the file to be read
 * @return 1 and error message if the file is illegal or in a bad format or wrong amount of args,
 * otherwise prints the updates heat grid based on input file and returns 0.
 */
int main(int argc, char* argv[])
{
    char line[5];
    int is_cyclic = 0, counter = 0;
    size_t n = 0, m = 0, numOfSources = 0;
    unsigned int n_iter = 0;
    double terminate = 0;
    if(argc != NUM_OF_ARGS)
    {
        fprintf(stderr, WRONG_AMNT_ARGS_MSG);
        return ERR_RETURN;
    }
    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        fprintf(stderr, INPUT_FILE_ERR);
        return ERR_RETURN;
    }
    if (fscanf(f, "%lu, %lu", &m, &n) != 2 || (int)m < 0 || (int)n < 0)
    {
        fprintf(stderr, FORMAT_ERR_MSG);
        fclose(f);
        return ERR_RETURN;
    }
    if (fscanf(f, "%s", line) != 1 || strcmp(line, END_SECTION) != 0)
    {
        fprintf(stderr, FORMAT_ERR_MSG);
        fclose(f);
        return ERR_RETURN;
    }

    int size = 2;
    source_point* sources = (source_point*)malloc(size*sizeof(source_point));
    if (sources == NULL)
    {
        free(sources);
        fclose(f);
        return ERR_RETURN;
    }
    source_point sp;
    while (fscanf(f, "%d, %d, %lf", &sp.x, &sp.y, &sp.value) == 3)
    {
        if (sp.x >= (int)n || sp.y >= (int)m)
        {
            fprintf(stderr, FORMAT_ERR_MSG);
            free(sources);
            fclose(f);
            return ERR_RETURN;
        }
        if (counter == size)
        {
            size = size * size;
            sources = realloc(sources, size * sizeof(source_point));
            if (sources == NULL)
            {
                free(sources);
                fclose(f);
                return ERR_RETURN;
            }
        }
        sources[counter] = sp;
        counter++;
    }
    if (fscanf(f, "%s", line) != 1)
    {
        fprintf(stderr, FORMAT_ERR_MSG);
        free(sources);
        fclose(f);
        return ERR_RETURN;
    }
    if (fscanf(f, "%le", &terminate) != 1 || terminate <= 0)
    {
        fprintf(stderr, FORMAT_ERR_MSG);
        free(sources);
        fclose(f);
        return ERR_RETURN;
    }
    if (fscanf(f, "%u", &n_iter) != 1)
    {
        fprintf(stderr, FORMAT_ERR_MSG);
        free(sources);
        fclose(f);
        return ERR_RETURN;
    }
    if (fscanf(f, "%d", &is_cyclic) != 1 || is_cyclic < 0)
    {
        fprintf(stderr, FORMAT_ERR_MSG);
        free(sources);
        fclose(f);
        return ERR_RETURN;
    }
    numOfSources = (size_t)counter;
    double **grid = makeGrid(n, m, sources, numOfSources);
    if(grid == NULL)
    {
        free(grid);
        free(sources);
        fclose(f);
        return ERR_RETURN;
    }
    calcGrid(grid, n, m, heat_eqn, sources, numOfSources, is_cyclic, n_iter, terminate);
    for (int i = 0; i < (int)n; i++)
    {
        free(grid[i]);
    }
    free(grid);
    free(sources);
    fclose(f);
    return 0;
}