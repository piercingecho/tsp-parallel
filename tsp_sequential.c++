#include <stdio.h> 
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <limits.h> //for INT_MAX to initialize best value.

#include "FileData.h" // for reading tsp file

#define FILE_NAME "cities.txt"
#define WORDY_OUTPUT 1

using namespace std;

struct FileData read_tsp_file(char const* filename); // const* because string is literal
void free_file_data(int** city_weights, int num_cities);
int find_tsp_solution_sequential(int* best_permute_array,
                    int* start_permute_array, 
                    int* end_permute_array, 
                    int** city_weights,
                    int num_cities);
int compute_tsp_permutation(int* permutation, int** city_weights, int num_cities);
void report_tsp_solution(int* best_permute_array, int lowest_cost, int num_cities);
int int_array_equals(int* arrayone, int* arraytwo, int length);
//:3 

int main_sequential(void)
{
    // FileData's city weights is a malloc'd int array, free it later
    struct FileData file_data = read_tsp_file(FILE_NAME);

    int** city_weights = file_data.city_weights;
    int num_cities = file_data.num_cities;


    //Complete the TSP.
    int best_permute_array[num_cities - 1];
    int start_permute_array[num_cities - 1];
    int end_permute_array[num_cities - 1];
    for(int i=0; i<num_cities - 1; i++)
    {
        start_permute_array[i] = i+1;
        end_permute_array[i] = i+1;
        best_permute_array[i] = i+1;
    }

    int best_value = find_tsp_solution_sequential(best_permute_array, start_permute_array, end_permute_array, city_weights, num_cities);

    if(WORDY_OUTPUT)
    {
        report_tsp_solution(best_permute_array, best_value, num_cities);
    }

    free_file_data(city_weights, num_cities);

    return 0;
}

struct FileData read_tsp_file(char const* filename)
{
    FILE *f = fopen(filename, "r");
    if(f == NULL) {
        printf("Unable to open file.\n");
        exit(1);
    }
    
    // read first line for number of cities
    int num_cities;
    fscanf(f, "%d", &num_cities);

    int** city_weights = (int**) malloc(num_cities*sizeof(int *));
    for (int i = 0; i < num_cities; i++)
    {
        city_weights[i] = (int*)malloc(num_cities * sizeof(int));
        if(city_weights[i] == NULL)
        {
            printf("Malloc failed; exiting.");
            exit(1);
        }
    }

    for(int i=0; i<num_cities; i++)
    {
        // char *line = readline(f);

        for(int j=0; j<num_cities; j++)
        {
            
            //tokenize line by spaces and atoi the weight
            fscanf(f, "%d", &city_weights[i][j]);

        }
    }

    struct FileData converted_file_data = {num_cities, city_weights};
    return converted_file_data;
}

void free_file_data(int** city_weights, int num_cities)
{
    for (int i = 0; i < num_cities; i++)
    {
        free(city_weights[i]);
    }
    free(city_weights);
}

int find_tsp_solution_sequential(int* best_permute,
                    int* start_permute, 
                    int* end_permute, 
                    int** city_weights,
                    int num_cities) 
{
    /*
    * Finds the best TSP permutation for the given permutation range.
    * We try all permutations in lexicographic order from start_permute
    * to end_permute.
    * 
    * Arguments:
    *   best_permute: the array to store the best permutation in.
    *                 This array is altered in-place, and has a length of
    *                 num_cities - 1.
    *   start_permute: the permutation to start with.
    *   end_permute: the permutation to end with. If it is the same as
    *                start_permute, all permutations will be tried.
    *   city_weights: the weights of all cities. num_cities * num_cities
    *                 2d array.
    *   num_cities: the number of cities to begin with.
    * 
    * Returns:
    *   best_value: integer representing the lowest cost Hamiltonian path.
    *   Also alters best_permute array in-place.
    */
    // initialize problem. Assume we start at city 0.
    int best_value, curr_value;
    int curr_permute[num_cities];
    for(int i=0; i<num_cities - 1; i++)
    {
        curr_permute[i] = start_permute[i];
        best_permute[i] = start_permute[i];
    }
    best_value = INT_MAX;

        
    do {
        curr_value = compute_tsp_permutation(curr_permute, city_weights, num_cities);

        if(curr_value < best_value)
        {
            best_value = curr_value;
            for(int i=0; i<num_cities - 1; i++)
            {
                best_permute[i] = curr_permute[i];
            }
        }

        if(WORDY_OUTPUT)
        {
            printf("0 ");
            for(int i=0; i<num_cities - 1; i++)
            {
                printf("%d ", curr_permute[i]);
            }
            printf("0 %d\n", curr_value);
        }

    
        next_permutation(curr_permute,curr_permute+(num_cities - 1));

    // iterate through each permutation until curr_permute == end_permute
    }while( !int_array_equals(curr_permute, end_permute, num_cities - 1));
    return best_value;
}

int compute_tsp_permutation(int* permutation, int** city_weights, int num_cities)
{
    /* Returns the cost of the hamiltonian path with the given permutation.
    * 
    * 
    */
    int curr_city = 0;
    int curr_cost = 0;
    int next_city;
    for(int i=0; i<num_cities-1; i++)
    {
        next_city = permutation[i];
        curr_cost += city_weights[curr_city][next_city];
        curr_city = next_city;
    }
    // back to start
    curr_cost += city_weights[curr_city][0];

    return curr_cost;
}

void report_tsp_solution(int* best_permute_array, int lowest_cost, int num_cities)
{
    printf("===\n");
    printf("0 ");
    for(int i=0; i<num_cities - 1; i++)
    {
        printf("%d ", best_permute_array[i]);
    }
    printf("0: %d", lowest_cost);
    return;
}

int int_array_equals(int* arrayone, int* arraytwo, int length)
{
    for(int i=0; i<length; i++)
    {
        if(arrayone[i] != arraytwo[i])
        {
            return 0;
        }
    }
    return 1;
}