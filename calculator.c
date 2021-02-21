/**
 * calculator.c file
 * calculates the heating of the grid, updates and prints it.
 */

//--------------------- includes ---------------------
#include <endian.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

//--------------------- const definitions ---------------------

/**
 * False = 0
 */
#define FALSE 0

/**
 * True = 1
 */
#define TRUE 1

/**
 * Structure to hold heat sources.
 */
typedef struct
{
    int x, y;
    double value;
} source_point;

/**
 * Useful typedef.
 */
typedef double (*diff_func)(double cell, double right, double top, double left, double bottom);

// ------------------------------ functions -----------------------------

/**
 * given a cell, checks if it's a source.
 * @param i- x coor. of the point checked.
 * @param j- y coor. of the point checked.
 * @param sources- the array of sources.
 * @param num_sources- number of sources.
 * @return true iff the cell checked is a source, false otherwise.
 */
int isSource(unsigned int i, unsigned int j, source_point *sources, size_t num_sources)
{
    unsigned int k;
    for (k = 0; k < num_sources; k++)
    {
        source_point source = sources[k];
        if (((unsigned int)source.x == i) && ((unsigned int)source.y == j))
        {
            return TRUE;
        }
    }
    return FALSE;
}


/**
 * updates the cell's value according the function's value and if is_cyclic.
 * @param function- a pointer for the function updating the grid.
 * @param grid- a pointer to the grid.
 * @param n- num of arrays in the grid.
 * @param m- size of each array.
 * @param is_cyclic- is_cyclic variable.
 * @param i- x coor. of the point checked.
 * @param j- y coor. of the point checked.
 */
void cellsUpdater(diff_func function, double **grid, size_t n, size_t m, int is_cyclic, unsigned int i, \
                  unsigned int j)
{
    double top = 0, bottom = 0, left = 0, right = 0, cell = grid[i][j];
    unsigned int leftJ = ((j + ((int) m - 1)) % (unsigned int) m);
    unsigned int rightJ = ((j + 1) % (unsigned int) m);
    unsigned int topI = ((i + ((int) n - 1)) % (unsigned int) n);
    unsigned int bottomI = ((i + 1) % (unsigned int) n);
    top = grid[topI][j];
    bottom = grid[bottomI][j];
    left = grid[i][leftJ];
    right = grid[i][rightJ];
    if (!is_cyclic)
    {
        if (leftJ > j)
        {
            left = 0;
        }
        else if (rightJ < j)
        {
            right = 0;
        }
        if (topI > i)
        {
            top = 0;
        }
        else if (bottomI < i)
        {
            bottom = 0;
        }
    }
    grid[i][j] = function(cell, right, top, left, bottom);
}


/**
 * an aid function for calculate, calculates the wanted difference.
 * @param function- a pointer for the function updating the grid.
 * @param grid- a pointer to the grid.
 * @param n- num of arrays in the grid.
 * @param m- size of each array.
 * @param sources- the array of sources.
 * @param num_sources- number of sources.
 * @param is_cyclic- is_cyclic variable.
 * @return the difference between currentSum and newSum in abs. value.
 */
double calculatorHelper(diff_func function, double **grid, size_t n, size_t m, \
                        source_point *sources, size_t num_sources, int is_cyclic)
{
    unsigned int i, j;
    static int firstIterFlag = TRUE;
    static double currentSum = 0, newSum = 0;
    if (!firstIterFlag)
    {
        currentSum = newSum;
        newSum = 0;
    }
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            if (firstIterFlag)
            {
                currentSum += grid[i][j];
            }
            if (!isSource(i, j, sources, num_sources))
            {
                cellsUpdater(function, grid, n, m, is_cyclic, i, j);
            }
            newSum += grid[i][j];
        }
    }
    firstIterFlag = FALSE;
    return fabs(newSum - currentSum);
}


/**
 * checks that all args of calculate function are not null.
 * @return FALSE if there's a null arg, True otherwise.
 */
double nullChecker (diff_func function, double **grid, size_t n, size_t m, source_point *sources, \
                    double terminate)
{
    if ((function == NULL) || (grid == NULL) || (!n) || (!m) || (sources == NULL) \
        || (!terminate))
    {
        return FALSE;
    }
    return TRUE;
}


/**
 * Calculator function. Applies the given function to every point in the grid iteratively for
 * n_iter loops, or until the cumulative difference is below terminate (if n_iter is 0).
 */
double calculate(diff_func function, double **grid, size_t n, size_t m, source_point *sources, \
                 size_t num_sources, double terminate, unsigned int n_iter, int is_cyclic)
{
    if (!nullChecker(function, grid, n, m, sources, terminate))
    {
        return FALSE;
    }
    double diff = (terminate + 1);
    //if n_iter == 0, calculate until diff < terminate , and return diff.
    if (!n_iter)
    {
        while (diff >= terminate)
        {
            diff = calculatorHelper(function, grid, n, m, sources, num_sources, is_cyclic);
        }
    }
    //else, calculate and print grid every n_iter iterations.
    else
    {
        unsigned int i;
        for (i = 0; i < n_iter; i++)
        {
            diff = calculatorHelper(function, grid, n, m, sources, num_sources, is_cyclic);
        }
    }
    return diff;
}

