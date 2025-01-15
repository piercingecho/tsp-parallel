// orderPermutation(current_path_array, numPerm, NUM_VERTICES);

#include <stdio.h> 
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <limits.h> //for INT_MAX to initialize best value.
#include <omp.h>

#include "tsp_sequential.c++"
#include "factoradic_id.c++"

#define FILE_NAME "cities.txt"
#define WORDY_OUTPUT 1
#define NUM_THREADS 8

using namespace std;

int find_tsp_solution_parallel(int* best_permute, int* start_permute, int num_iterations, int** city_weights, int num_cities);


int main(void)
{
    // FileData's city weights is a malloc'd int array, free it later
    struct FileData file_data = read_tsp_file(FILE_NAME);

    int** city_weights = file_data.city_weights;
    int num_cities = file_data.num_cities;


    int best_overall_permute_array[num_cities - 1];
    int best_overall_value = INT_MAX; // so one thread is guaranteed to change it

    /* Each thread is assigned a chunk by their thread ID: higher IDs have a higher permutation number.
    *  We break ties by using the lowest combination number, i.e. the lowest thread ID that gave the best value.
    */
    int best_thread = NUM_THREADS + 1; // 

    int num_permutations = (int) factorial(num_cities - 1);
    int chunk_size = round((float) num_permutations / (float) NUM_THREADS);



    /* Create a set of threads to complete problem. 
     * Share the global city weights and , as well as our best overall.
     */

    #pragma omp parallel \
    shared(city_weights, num_cities, num_permutations, chunk_size, \
    best_overall_permute_array, best_overall_value, best_thread) \
    num_threads(NUM_THREADS)
 	{

        int tid = omp_get_thread_num();

        // initialize TSP arrays. Keep local bests, so that we mutex at the end. 
        int best_permute_array[num_cities - 1];
        int start_permute_array[num_cities - 1];
        for(int i=0; i<num_cities - 1; i++)
        {
            start_permute_array[i] = i+1;
            best_permute_array[i] = i+1;
        }

        //find start and end of our loops
        int start_permutation_id = chunk_size * tid;
        int end_permutation_id = chunk_size * (tid + 1);

        if(WORDY_OUTPUT)
        {
            printf("Thread %d: perms %d to %d\n", tid, start_permutation_id, end_permutation_id - 1);
        }

        orderPermutation(start_permute_array, start_permutation_id, num_cities - 1);


        // gets best value AND modifies best_permute_array. 
        int best_value = find_tsp_solution_parallel(best_permute_array, start_permute_array, chunk_size, city_weights, num_cities);
        
        // Lock variables from being accessed while we update globals with best thread possibilities
        #pragma omp critical (global_best_combo_lock)
        {
            if(best_value < best_overall_value ||
               (best_value == best_overall_value && tid < best_thread))
            {
                best_overall_value = best_value;
                best_thread = tid;
                for(int i=0; i<num_cities-1; i++)
                {
                    best_overall_permute_array[i] = best_permute_array[i];
                }
            }
            
        }

        // end lock

        
    } /* end of parallel */

    if(WORDY_OUTPUT)
    {
        report_tsp_solution(best_overall_permute_array, best_overall_value, num_cities);
    }

    free_file_data(city_weights, num_cities);

    return 0;
}

int find_tsp_solution_parallel(int* best_permute, int* start_permute, int num_iterations, int** city_weights, int num_cities){
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
    *   num_iterations: the number of permutations to iterate through.
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

        
    for(int i=0; i<1; i++) // num_iterations; i++)
    {
        curr_value = compute_tsp_permutation(curr_permute, city_weights, num_cities);

        if(curr_value < best_value)
        {
            best_value = curr_value;
            for(int j=0; j<num_cities - 1; j++)
            {
                best_permute[i] = curr_permute[i];
            }
        }
    
        next_permutation(curr_permute, curr_permute+(num_cities - 1));

    // iterate through each permutation until curr_permute == end_permute
    }
    return best_value;
}
