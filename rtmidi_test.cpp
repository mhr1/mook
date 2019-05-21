
//*****************************************//
//  midiout.cpp
//  by Gary Scavone, 2003-2004.
//
//  Simple program to test MIDI output.
//
//*****************************************//

#include <iostream>
#include <cstdlib>
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
// It returns false if there are no ports available.
bool chooseMidiPort( RtMidiOut *rtmidi );

#define STIME 1000

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
  int array_notes = 15;

// Probability array
  int A[array_notes] = {75, 30, 75, 75, 0, 75, 0, 75, 30, 75, 75, 0, 75, 0, 75 };


  range = 0;
	for(i = 0; i < array_notes; i++)
		range += A[i];

    x = (int)((double)my_rand() * (double)range/MY_RAND_MAX);

		sum = 0;
		j = 0;
  //while(x >= sum)sum += A[j++];
    for(j = 0; j < array_notes; j++)
    {
      sum += A[j];
      if(x <= sum)
        break;
    }
    cout << "Note " << j + 1 << " given x = " << x << endl;

    return(j);
}


int get_space(void)
{   int j = 0;
    int x = (int)((double)my_rand() * (double)100/MY_RAND_MAX);

    if(x > 60)j = 1;
    return(j);
}


int main( void )
{
  RtMidiOut *midiout = 0;
  vector<unsigned char> message;
  int note1, note2, note3;

  int note_array[3][22] = {{48, 50, 52, 53, 55, 57, 59,
                        60, 62, 64, 65, 67, 69, 71,
                        72, 74, 76, 77, 79, 81, 83,
                        84},
                        {48, 50, 51, 53, 55, 57, 58,
                        60, 62, 63, 65, 67, 69, 72,
                        72, 74, 75, 77, 79, 81, 82,
                        84},
                        {48, 49, 51, 53, 55, 56, 58,
                        60, 61, 63, 65, 67, 68, 72,
                        72, 73, 75, 77, 79, 80, 82,
                        84}};

  int mode = 2; // 0 = Ionian, 1 = Dorian, 2 = Phrygian
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

  SLEEP(STIME);

  /*message[0] = 0xF1;
  message[1] = 60;
  midiout->sendMessage( &message );*/

  // Control Change: 176, 7, 100 (volume)
  message[0] = 176;
  message[1] = 7;
  message.push_back( 100 );
  midiout->sendMessage( &message );

  for (int g = 0; g < 256; g++)
  {
    if(get_space()) //space
      SLEEP(STIME);
    else
    {
      note1 = get_note();
      //note2 = get_note();
      //note3 = get_note();

      message[0] = 144; // Note on
      message[1] = note_array[mode][note1];
      message[2] = 90;
      midiout->sendMessage( &message );

      message[0] = 144; // Note on
      message[1] = note_array[mode][note1 + 2];
      message[2] = 90;
      midiout->sendMessage( &message );

      message[0] = 144; // Note on
      message[1] = note_array[mode][note1 + 4];
      message[2] = 90;
      midiout->sendMessage( &message );

      message[0] = 144; // Note on
      message[1] = note_array[mode][note1 + 6];
      message[2] = 90;
      midiout->sendMessage( &message );

      SLEEP(STIME);

      message[0] = 128; // Note off
      message[1] = note_array[mode][note1];
      message[2] = 40;
      midiout->sendMessage( &message );

      message[0] = 128; // Note off
      message[1] = note_array[mode][note1 + 2];
      message[2] = 40;
      midiout->sendMessage( &message );

      message[0] = 128; // Note off
      message[1] = note_array[mode][note1 + 4];
      message[2] = 40;
      midiout->sendMessage( &message );

      message[0] = 128; // Note off
      message[1] = note_array[mode][note1 + 6];
      message[2] = 40;
      midiout->sendMessage( &message );
    }
    //SLEEP(STIME * 2);
  }
  // Note On: 144, 64, 90
/*  message[0] = 144;
  message[1] = 64;
  message[2] = 90;
  midiout->sendMessage( &message );

  message[0] = 144;
  message[1] = 61;
  message[2] = 90;
  midiout->sendMessage( &message );

  SLEEP( 500 );

  // Note Off: 128, 64, 40
  message[0] = 128;
  message[1] = 64;
  message[2] = 40;
  midiout->sendMessage( &message );

  message[0] = 128;
  message[1] = 61;
  message[2] = 40;
  midiout->sendMessage( &message );
  //SLEEP( 500 );

  // Note On: 144, 64, 90
/ message[0] = 144;
  message[1] = 66;
  message[2] = 90;
  midiout->sendMessage(&message );

  SLEEP( 500 );

  // Note Off: 128, 64, 40
  message[0] = 128;
  message[1] = 66;
  message[2] = 40;
  midiout->sendMessage( &message );

  //SLEEP( 500 );

  // Note On: 144, 64, 90
  message[0] = 144;
  message[1] = 61;
  message[2] = 90;
  midiout->sendMessage( &message );

  SLEEP( 500 );

  // Note Off: 128, 64, 40
  message[0] = 128;
  message[1] = 61;
  message[2] = 40;
  midiout->sendMessage( &message );*/

  SLEEP(STIME * 8); // Allow time for the notes to ring on


  // Control Change: 176, 7, 40
  message[0] = 176;
  message[1] = 7;
  message[2] = 40;
  midiout->sendMessage( &message );

  SLEEP( 500 );

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

bool chooseMidiPort( RtMidiOut *rtmidi )
{
  /*std::cout << "\nWould you like to open a virtual output port? [y/N] ";

  std::string keyHit;
  std::getline( std::cin, keyHit );
  if ( keyHit == "y" ) {
    rtmidi->openVirtualPort();
    return true;
  }

  std::string portName;
  unsigned int i = 0, nPorts = rtmidi->getPortCount();
  if ( nPorts == 0 ) {
    std::cout << "No output ports available!" << std::endl;
    return false;
  }

  if ( nPorts == 1 ) {
    std::cout << "\nOpening " << rtmidi->getPortName() << std::endl;
  }
  else {
    for ( i=0; i<nPorts; i++ ) {
      portName = rtmidi->getPortName(i);
      std::cout << "  Output port #" << i << ": " << portName << '\n';
    }

    do {
      std::cout << "\nChoose a port number: ";
      std::cin >> i;
    } while ( i >= nPorts );
  }

  std::cout << "\n";
  rtmidi->openPort( i );*/
  if(rtmidi->getPortCount() > 0)
    rtmidi->openPort(0);
  else
    cout << "No ports found" << endl;

  return true;
}
