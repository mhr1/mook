// Markov tests
#include <iostream>

using namespace std;

static unsigned long my_seed = 11056; //default random seed
#define MY_RAND_MAX 2147483646 // 2^32 - 2, 1 less than mod value in rand no gen

unsigned long my_rand(void)
{
	// Park & Miller's minimal standard multiplicative congruential algorithm
   // USE: Pick a random number x  from range
	//	x = (int)((double)my_rand() * (double)range/MY_RAND_MAX);
	unsigned long random_number;

	random_number = (16807 * my_seed) % (MY_RAND_MAX + 1);
	my_seed = random_number;
	return(random_number);
}

int main(void)
{
  int range, sum;
  int i, j, x;

  int A[7] = {75, 30, 20, 50, 50, 30, 20};


  range = 0;
	for(i = 0; i < 7; i++)
		range += A[i];

  for(i = 0; i < 100; i++)
  {
    x = (int)((double)my_rand() * (double)range/MY_RAND_MAX);

		sum = 0;
		j = 0;
    while(x >= sum)sum += A[j++];

    cout << "Note " << j << " given x = " << x << endl;
  }

}
