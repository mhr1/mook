/*
  r_mook.cpp

  rhythm generator - Monday 17th June 2019

  Using Drums on MIDI Channel 9

  */

#include <iostream>
#include <cstdlib>
//#include <stdio.h>
//#include <gtk/gtk.h>
#include "RtMidi.h"

using namespace std;

#define ON 0x98 // Note on - channel 9
#define OFF 0x88 // Note off - channel 9

#define BASS 35
#define SNARE 38
#define HIHAT 44

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

// Platform-dependent sleep routines.
#if defined(WIN32)
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds )
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

// This function should be embedded in a try/catch block in case of
// an exception.  It offers the user a choice of MIDI ports to open.
bool chooseMidiPort( RtMidiOut *rtmidi )
{
  if(rtmidi->getPortCount() > 0)
    rtmidi->openPort(0);
  else
    cout << "No ports found" << endl;

  return true;
}

#define STIME 500
#define TICK 16

static unsigned long my_seed = 1223; //11056; //default random seed
#define MY_RAND_MAX 2147483646 // 2^32 - 2, 1 less than mod value in rand no gen

unsigned long my_rand(void)
{
	unsigned long random_number;

	 random_number = (16807 * my_seed) % (MY_RAND_MAX + 1);
	my_seed = random_number;
	return(random_number);
}

int get_beat(int probability)
{   int j = 0;
    int x = (int)((double)my_rand() * (double)100/MY_RAND_MAX);

    if(x < probability) j = 1;
    return(j);
}


int main( void )
{
  int i;
  int bass_pdm[8] = {99, 0, 0, 0, 99, 0, 0, 0};
  int snare_pdm[8] = {0, 0, 99, 0, 0, 0, 99, 0};
  int hihat_pdm[8] = {50, 50, 50, 50, 50, 50, 50, 50};

  RtMidiOut *midiout = 0;
  vector<unsigned char> message;

  // RtMidiOut constructor
  try {midiout = new RtMidiOut();}
  catch (RtMidiError &error)
  {
    error.printMessage();
    exit( EXIT_FAILURE );
  }
  // Call function to select port.
  try
  {
    if(!chooseMidiPort(midiout))
    { delete midiout;
    //return(-1);
    }
  }
  catch (RtMidiError &error)
  {
    error.printMessage();
    delete midiout;
    //return(-1);
  }

  // Send out a series of MIDI messages.

  // Program change: 192, 5
  message.push_back( 0xF1 );
  message.push_back(60);
  midiout->sendMessage( &message );

  // Control Change: 176, 7, 100 (volume)
  message[0] = 0xB0; //176;
  message[1] = 7;
  message.push_back( 100 );
  //midiout->sendMessage( &message );
  // This will be first message sent in main loop
/*
  message[0] = 0xB1; //176;
  message[1] = 7;
  message.push_back( 100 );
  midiout->sendMessage( &message );
*/
  int c[12][3] = {{0, ON, BASS},
                  {0, ON, HIHAT},
                  {10, OFF, HIHAT},
                  {0, ON, HIHAT},
                  {10, OFF, BASS},
                  {0, OFF, HIHAT},
                  {0, ON, SNARE},
                  {0, ON, HIHAT},
                  {10, OFF, HIHAT},
                  {0, ON, HIHAT},
                  {10, OFF, SNARE},
                  {0, OFF, HIHAT}
                };

  cout << "Start\n";
  // SLEEP(STIME * 4);

  int tick_time = 0;
  int t = 0;
  while(1)
  {
    if(tick_time-- < 1)
    {
      while(tick_time < 1)
      {
        midiout->sendMessage( &message ); // send message after waiting tick_time
        //std::cout << std::dec << message[0] << message[1] << message[2]  << endl;
       cout << "Time " << t << endl;
        tick_time = c[t][0];
        message[0] = c[t][1];
        message[1] = c[t][2];
        message[2] = 99;

        if(t++ > 10) t = 0;
      }
    }

    if(kbhit())break;
    SLEEP(TICK);
  }


/*
  for(i = 0; i < 16; i++)
  {
    message[0] = 0x98; // Note on
    message[1] = 40; // crash drum
    message[2] = 90;
    midiout->sendMessage( &message );

    SLEEP(STIME);

    message[0] = 0x88; // Note off
    message[1] = 40;
    message[2] = 90;
    midiout->sendMessage( &message );

  }

  SLEEP(STIME * 8); // Allow time for the notes to ring on

/*  // Control Change: 176, 7, 40
  message[0] = 176;
  message[1] = 7;
  message[2] = 40;
  midiout->sendMessage( &message );*/

//  SLEEP( 500 );

  // Sysex: 240, 67, 4, 3, 2, 247
  message[0] = 240;
  message[1] = 67;
  message[2] = 4;
  message.push_back( 3 );
  message.push_back( 2 );
  message.push_back( 247 );
  midiout->sendMessage( &message );

  // Clean up

  delete midiout;

  return 0;
}
