/**
 * reader.c file
 * reads input and manages the exercise's logic.
 */

//--------------------- includes ---------------------
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <assert.h>
#include "heat_eqn.h"
#include "calculator.h"

//--------------------- const definitions ---------------------
/**
 * error message for wrong argument format.
 */
#define MAL_ARGS_MSG "Error- wrong args format.\n"

/**
 * error message for wrong delimiter format.
 */
#define MAL_DELIMITER_MSG "Error- wrong delimiter format.\n"

/**
 * error message for wrong sources input formt.
 */
#define MAL_SRC_INPUT "Error- wrong sources input.\n"

/**
 * error message for wrong file input formt.
 */
#define MAL_FILE_INPUT "Error- wrong file input.\n"

/**
 * delimiter char
 */
#define DELIMITER '-'

/**
 * False = 0
 */
#define FALSE 0

/**
 * True = 1
 */
#define TRUE 1

/**
 * * return value of main if all went well.
 */
#define EXIT_SUCCESS 0

/**
 * return value of main if an error occur.
 */
#define EXIT_ERROR 1

/**
 * correct number of args per source
 */
#define SRC_ARGS 3

/**
 * correct number of args
 */
#define ARGS_NUMBER 2

/**
 * default initial size of sources array
 */
#define INITIAL_SIZE 10

/**
 * global variable- the grid's matrix.
 */
double **gGrid;

/**
 * number of sources
 */
size_t gNum_Sources;

// ------------------------------ functions -----------------------------

/**
 * creates a valid matrix for the grid.
 * @param n size of the grid
 * @param m size of the grid's element
 * @param grid- the grid as a nXm matrix
 * @return True iff the initialization is a success. False otherwise.
 */
int gridFactory(size_t n, size_t m, double **grid)
{
    if ((!n) || (!m))
    {
        return FALSE;
    }
    unsigned int i, j;
    for (i = 0; i < n; i++)
    {
        grid[i] = (double *) malloc((m) * sizeof(double));
        for (j = 0; j < m; j++)
        {
            grid[i][j] = 0;
        }
    }
    gGrid = grid;
    return TRUE;
}


/**
 * checks validity of the delimiter.
 * @param filePtr- a pointer for the input file.
 * @return True iff the delimiter is valid. False otherwise.
 */
int delimiterManager(FILE *filePtr)
{
    char delimiter;
    int check = fscanf(filePtr, "%c", &delimiter);
    while ((delimiter != '\n') && (delimiter != '\r') && (check))
    {
        if (delimiter != DELIMITER)
        {
            return FALSE;
        }
        check = fscanf(filePtr, "%c", &delimiter);
    }
    return TRUE;
}


/**
 * manages source creation and grid updating.
 * @param filePtr- a pointer for the input file.
 * @param srcArray- a pointer to an array of source_points.
 * @param n- size of the grid
 * @param m- size of the grid's element
 * @return True iff all src points are valid, and grid was updated successfully. False otherwise.
 */
int sourceFactory(FILE *filePtr, source_point *srcArray, size_t n, size_t m)
{
    int srcFlag = FALSE, counter = 0;
    int x = 0, y = 0;
    double value = 0;
    int check = fscanf(filePtr, "%d, %d, %lf\n", &x, &y, &value);
    while (check == SRC_ARGS)
    {
        //checking for validity of indices
        if ((x >= (int)m) || (y >= (int)n) || (x < 0) || (y < 0))
        {
            fprintf(stderr, "%s", MAL_SRC_INPUT);
            return FALSE;
        }
        source_point src = {x, y, value};
        //reallocating the array if needed
        if (counter >= INITIAL_SIZE)
        {
            srcArray = (source_point *) realloc(srcArray, ((sizeof(source_point) * (counter + 1))));
        }
        *(srcArray + counter) = src;
        counter++;
        gGrid[x][y] = value;
        srcFlag = TRUE;
        check = fscanf(filePtr, "%d, %d, %lf\n", &x, &y, &value);
    }
    if (!srcFlag)
    {
        return FALSE;
    }
    gNum_Sources = (size_t)counter;
    return TRUE;
}


/**
 * manages source creation validity of delimiter after the source's.
 * @param filePtr- a pointer for the input file.
 * @param srcArray- a pointer to an array of source_points.
 * @param n- size of the grid
 * @param m- size of the grid's element
 * @return True iff all src points creation has succeeded, and delimiter is valid. False otherwise.
 */
int srcInitializer(FILE *filePtr, source_point *srcArray, size_t n, size_t m)
{
    if (srcArray == NULL)
    {
        return FALSE;
    }
    int flag = sourceFactory(filePtr, srcArray, n, m);
    if (!flag)
    {
        fprintf(stderr, "%s", MAL_DELIMITER_MSG);
        return FALSE;
    }
    if (!delimiterManager(filePtr))
    {
        fprintf(stderr, "%s", MAL_DELIMITER_MSG);
        return FALSE;
    }
    return TRUE;
}


/**
 * updates the down mentioned variables.
 * @param filePtr- a pointer for the input file.
 * @param terminate- a pointer to 'terminte' variable.
 * @param n_iter- a pointer to 'n_iter' variable.
 * @param is_cyclic- a pointer to 'is_cyclic' variable.
 * @return True iff all variables assignment are valid, False otherwise.
 */
int variableInitializer(FILE *filePtr, double *terminate, unsigned int *n_iter, int *is_cyclic)
{
    int check;
    //get terminate
    check = fscanf(filePtr, "%lf\n", terminate);
    if (!check)
    {
        fprintf(stderr, "Error: wrong terminate input format.");
        return FALSE;
    }
    //get n_iter
    int test;
    check = fscanf(filePtr, "%d", &test);
    if (test <= 0)
    {
        *n_iter = 0;
    }
    else
    {
        *n_iter = (unsigned int)test;
    }
    if (!check)
    {
        fprintf(stderr, "Error: wrong n_iter input format.");
        return FALSE;
    }
    //get is_cyclic
    check = fscanf(filePtr, "%d", is_cyclic);
    if (((*is_cyclic != 0) && (*is_cyclic != 1)) || (!check))
    {
        fprintf(stderr, "Error: wrong is_cyclic input format.");
        return FALSE;
    }
    return TRUE;
}


/**
 * sets the value of n
 * @param filePtr- pointer to the file
 * @param n- pointer to n
 * @return pointer to n
 */
size_t getN(FILE *filePtr, size_t *n)
{
    if ((filePtr == NULL) || (n == NULL))
    {
        return FALSE;
    }
    //receiving size of grid.
    int check = fscanf(filePtr, "%lu ,", n);
    if ((!check) || (*n <= 0))
    {
        fprintf(stderr, "Error: wrong format of grid size input.");
        return FALSE;
    }
    return *n;
}


/**
 * sets the value of m
 * @param filePtr- pointer to the file
 * @param m- pointer to m
 * @return pointer to m
 */
size_t getM(FILE *filePtr, size_t *m)
{
    if ((filePtr == NULL) || (m == NULL))
    {
        return FALSE;
    }
    //receiving size of grid.
    int check = fscanf(filePtr, "%lu\n", m);
    if ((!check) || (*m <= 0))
    {
        fprintf(stderr, "Error: wrong format of grid size input.");
        return FALSE;
    }
    return *m;
}


/**
 * initializes the grid's matrix.
 * @param filePtr- a pointer for the input file.
 * @param grid- the grid as a nXm matrix
 * @param n- size of the grid
 * @param m- size of the grid's element
 * @return
 */
double** gridInitializer(FILE *filePtr, size_t *n, size_t *m)
{
    gGrid = (double **) malloc((*n) * sizeof(double *));
    if (!gridFactory(*n, *m, gGrid))
    {
        fprintf(stderr, "Error: failed creating a grid using the given size.");
        return FALSE;
    }
    if (!delimiterManager(filePtr))
    {
        fprintf(stderr, "%s", MAL_DELIMITER_MSG);
        return FALSE;
    }
    return gGrid;
}


/**
 * frees the memory allocated for the grid and for the source array.
 * @param n- size of grid.
 * @param srcArray- the source array.
 */
void freeProgram(size_t n)
{
    unsigned int i;
    for (i = 0; i < n; i++)
    {
        free(gGrid[i]);
    }
    free(gGrid);
}


/**
 * prints the grid
 * @param grid- a pointer to the grid
 * @param n- num of arrays in the grid
 * @param m- size of each array
 */
void printGrid(double **grid, size_t n, size_t m)
{
    unsigned int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            printf("%.4lf,", grid[i][j]);
        }
        printf("\n");
    }
}


/**
 * main function.
 * @param argc- args count.
 * @param argv- args vector.
 * @return 0 if success, 1 otherwise.
 */
int main(int argc, char *argv[])
{
    //valid args number.
    if (argc != ARGS_NUMBER)
    {
        fprintf(stderr, MAL_ARGS_MSG);
        return EXIT_ERROR;
    }
    FILE *filePtr = fopen(argv[1], "r");
    if (filePtr == NULL)
    {
        fprintf(stderr, MAL_FILE_INPUT);
        return EXIT_ERROR;
    }
    size_t n = 0, m = 0;
    size_t *pn = &n, *pm = &m;
    //create grid
    n = getN(filePtr, &n);
    m = getM(filePtr, &m);
    gGrid = gridInitializer(filePtr, pn, pm);
    if (gGrid == NULL)
    {
        fprintf(stderr, "Error: failed creating a valid grid.");
        return EXIT_ERROR;
    }
    //initialize sources
    source_point *srcArray = (source_point*)malloc(sizeof(source_point) * INITIAL_SIZE);
    if (!srcInitializer(filePtr, srcArray, n, m))
    {
        fprintf(stderr, MAL_SRC_INPUT);
        return EXIT_ERROR;
    }
    //initialize terminate, n_iter and is_cyclic.
    double terminate = 0;
    unsigned int n_iter;
    int is_cyclic = 0;
    if (variableInitializer(filePtr, &terminate, &n_iter, &is_cyclic))
    {
        double diff = calculate(heat_eqn, gGrid, n, m, srcArray, gNum_Sources, terminate, \
                                n_iter, is_cyclic);
        if (!diff)
        {
            fclose(filePtr);
            freeProgram(n);
            fprintf(stderr, "Error: something with the calculation went wrong.");
            return EXIT_ERROR;
        }
        if (n_iter)
        {
            printf("%lf\n", diff);
            printGrid(gGrid, n, m);
            while (diff >= terminate)
            {
                diff = calculate(heat_eqn, gGrid, n, m, srcArray, gNum_Sources, \
                                 terminate, n_iter, is_cyclic);
                printf("%lf\n", diff);
                printGrid(gGrid, n, m);
            }
        }
        else
        {
            printf("%lf\n", diff);
            printGrid(gGrid, n, m);
        }
        freeProgram(n);
        free(srcArray);
        fclose(filePtr);
        return EXIT_SUCCESS;
    }
    freeProgram(n);
    free(srcArray);
    fclose(filePtr);
    fprintf(stderr, MAL_ARGS_MSG);
    return EXIT_ERROR;
}

