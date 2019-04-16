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
  int range;
  int i, x;

  int A = 45;
  int B = 20;
  int C = 30;

  range = A + B + C;

  for(i = 0; i < 100; i++)
  {
    x = (int)((double)my_rand() * (double)range/MY_RAND_MAX);

    if(x < A) cout << "A" << endl;
    else if(x < A + B) cout << "B" << endl;
    else if(x < A + B + C) cout << "C" << endl;

    //cout << x << endl;
  }

}
