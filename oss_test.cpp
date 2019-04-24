 #include <sys/ioctl.h>
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <iostream>
#include <unistd.h>

using namespace std;

#define TYPE short
#define LENGTH 1 //number of seconds per frequency
#define RATE 44100 //sampling rate
#define SIZE sizeof(TYPE) //size of sample, in bytes
#define CHANNELS 1 //number of audio channels
#define PI 3.14159
#define NUM_FREQS 3 //total number of frequencies
#define BUFFSIZE (int) (NUM_FREQS*LENGTH*RATE*SIZE*CHANNELS) //bytes sent to audio device
#define ARRAYSIZE (int) (NUM_FREQS*LENGTH*RATE*CHANNELS) //total number of samples
#define SAMPLE_MAX (pow(2,SIZE*8 - 1) - 1)

static unsigned long my_seed = 1102; //11056; //default random seed
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

int get_note(void)
{
  int range, sum;
  int i, j, x;

  int A[9] = {75, 30, 20, 50, 50, 30, 20, 50, 200};


  range = 0;
	for(i = 0; i < 9; i++)
		range += A[i];

    x = (int)((double)my_rand() * (double)range/MY_RAND_MAX);

		sum = 0;
		j = 0;
    while(x >= sum)sum += A[j++];

    cout << "Note " << j << " given x = " << x << endl;

    return(j);
}

void writeToSoundDevice(TYPE buf[], int deviceID) {
      int status;
      status = write(deviceID, buf, 16000);
      if (status != 16000)
            perror("Wrote wrong number of bytes\n");
	// cout << status << endl;

      status = ioctl(deviceID, SNDCTL_DSP_SYNC, 0);
      if (status == -1)
            perror("SNDCTL_DSP_SYNC failed\n");
}

int main() {
      int device_ID, arg, status, t, a, i;
	int attack = 1;
	int decay = 0;
	int peak = SAMPLE_MAX - 200;
  int note;
	double k, f;
      TYPE buf[ARRAYSIZE];

// Note frequencies key C4 major
  double notes[9] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25, 0.0};

      device_ID = open("/dev/dsp", O_WRONLY, 0);
      if (device_ID < 0)
            perror("Opening /dev/dsp failed\n");

// working
      arg = SIZE * 8;
      status = ioctl(device_ID, SNDCTL_DSP_SETFMT, &arg);
      if (status == -1)
            perror("Unable to set sample size\n");
      arg = CHANNELS;
      status = ioctl(device_ID, SNDCTL_DSP_CHANNELS, &arg);
      if (status == -1)
            perror("Unable to set number of channels\n");
      arg = RATE;
      status = ioctl(device_ID, SNDCTL_DSP_SPEED, &arg);
      if (status == -1)
            perror("Unable to set sampling rate\n");
      a = SAMPLE_MAX;

	k = 2*PI/RATE;

  for (i = 0; i < 128; i++)
	{
    note = get_note() - 1;
    f = k * notes[note];
		//cout << f << endl;
		a = 0;
		attack = 1; decay = 0;
            	for (t = 0; t < 16000; t++)
		{
                  	if(attack)
			{
				a = a + 200;
				buf[t] = (TYPE)floor(a * sin(f*t));
				if(a > peak)
				{ attack = 0; decay = 1;}
			}
			else if(decay)
			{
				a = a - 5;
				if(a < 0)a = 0;
				buf[t] = (TYPE)floor(a * sin(f*t));
			}
            	}
      		writeToSoundDevice(buf, device_ID);
	}


	return(0);
}
