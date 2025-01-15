#include<iostream>
#include<algorithm>//needed for next_permutation 

const unsigned long long NUM_VERTICES = 4;//the number of vertices in the graph

void convertFact(unsigned long long, int[]);
void orderPermutation(int[], unsigned long long, int);
unsigned long long factorial(unsigned long long);

using namespace std;

/*
* This takes heavy reference from the following, math-heavy link: 
* http://irenes-coding-blog.blogspot.com/2012/07/factorial-base-numbers-and-permutations.html?m=1.
* It discusses how the factoradic numbers can be used to calculate the nth permutation.
*/

//returns the factorial for the given number
//x: the index for which factorial number you want.
//returns: x!
unsigned long long factorial(unsigned long long x)
{
	if(x == 1)
		return 1;
	else
		return x * factorial(x-1);
}

//converts a number into base factorial
//num: the number to convert to base factorial
//digits: a storage array to store the digits of the base factorial number
//numbers are stored in reverse order (so the 2nd digit is in position 1, the third in 2, etc.
//digits[0] will contain the length of the number in digits, since the first number of a base factorial number is always 0
void convertFact(unsigned long long num, int digits[])
{
	int numDigits = 1;//there is always a spare 0 in these numbers
	while(num > 0)
	{
		digits[numDigits] = num % (numDigits + 1);
		numDigits++;
		num /= numDigits;
	}
	digits[0] = numDigits;//because the first digit is always zero, we will store the length of the array in the 0th slot
}


//returns the array transposed to the nth permutation after the given ordering
//toOrder: the set for which you would like to define the nth permutation
//m: the numbered permutation to be stored in toOrder
//size: the size of the array toOrder
//warning: gives unpredictable behavior if n is > the number of unique permutations for toOrder
void orderPermutation(int toOrder[], unsigned long long n, int size)
{
	int swaps[NUM_VERTICES+ 10];
	convertFact(n, swaps);
	int location = size - swaps[0];//accounts for leading zeros
	int loc = swaps[0] - 1;//used to iterate through the digits of the factoradic number
	while(loc > 0)
	{
		int temp = toOrder[location + swaps[loc]];
		for(int x = location+swaps[loc]; x > location; x--)//moves all the other numbers up
		{
			toOrder[x] = toOrder[x - 1];
		}
		toOrder[location] = temp;
		location++;
		loc--;
	}
	
}


int factoradic_example()
{
	
	cout << "Calculating permutations from factoradic" << endl;
	unsigned long long numPerm = 0;
	int current_path_array[NUM_VERTICES];

	for (numPerm = 0; numPerm < factorial(NUM_VERTICES); numPerm++)
	{
		// reset the path
		for(int index = 0; index < NUM_VERTICES; index++)
		{
			current_path_array[index] = index;
		}

		orderPermutation(current_path_array, numPerm, NUM_VERTICES);
		
		for(int index = 0; index < NUM_VERTICES; index++)
		{
			cout << current_path_array[index] << " ";
		}
		cout << endl;
		
	}


	
	
	cout << "Calculating permutations from next_perm" << endl;
	
	for(int index = 0; index < NUM_VERTICES; index++)
	{
		current_path_array[index] = index;
	}

	do 
	{
	
		for(int index = 0; index < NUM_VERTICES; index++)
		{
			cout << current_path_array[index] << " ";
		}
		cout << endl;
	} while (next_permutation(current_path_array, current_path_array+NUM_VERTICES));
	return 0;
}