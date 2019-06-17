/*
  r_mook.cpp

  rhythm generator - Monday 17th June 2019

  */



#include <iostream>
#include <cstdlib>
//#include <stdio.h>
//#include <gtk/gtk.h>
#include "RtMidi.h"


using namespace std;


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

static unsigned long my_seed = 1223; //11056; //default random seed
#define MY_RAND_MAX 2147483646 // 2^32 - 2, 1 less than mod value in rand no gen

unsigned long my_rand(void)
{
	unsigned long random_number;

	 random_number = (16807 * my_seed) % (MY_RAND_MAX + 1);
	my_seed = random_number;
	return(random_number);
}


int main( void )
{
  int i;

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
  try {if(!chooseMidiPort(midiout)) goto cleanup;}
  catch (RtMidiError &error)
  {
    error.printMessage();
    goto cleanup;
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
  midiout->sendMessage( &message );
/*
  message[0] = 0xB1; //176;
  message[1] = 7;
  message.push_back( 100 );
  midiout->sendMessage( &message );
*/


  SLEEP(STIME * 4);

  for(i = 0; i < 16; i++)
  {
    message[0] = 0x98; // Note on
    message[1] = 36; // Bass drum
    message[2] = 90;
    midiout->sendMessage( &message );

    SLEEP(STIME);

    message[0] = 0x88; // Note off
    message[1] = 36;
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
 cleanup:
  delete midiout;

  return 0;
}
