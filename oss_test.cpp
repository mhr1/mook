 #include <sys/ioctl.h>
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/soundcard.h>
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
	double k, f;
      TYPE buf[ARRAYSIZE];
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

      	for (i = 0; i < NUM_FREQS; i++) 
	{
            	switch (i) 
		{
                  case 0:
                        f = k * 262;
                        break;
                  case 1:
                        f = k * 330;
                        break;
                  case 2:
                        f = k * 392;
                        break;
            	}

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