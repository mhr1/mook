
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
bool chooseMidiPort( RtMidiOut *rtmidi )
{
  if(rtmidi->getPortCount() > 0)
    rtmidi->openPort(0);
  else
    cout << "No ports found" << endl;

  return true;
}

#define STIME 500

static unsigned long my_seed = 1111; //11056; //default random seed
#define MY_RAND_MAX 2147483646 // 2^32 - 2, 1 less than mod value in rand no gen

unsigned long my_rand(void)
{
	unsigned long random_number;

	 random_number = (16807 * my_seed) % (MY_RAND_MAX + 1);
	my_seed = random_number;
	return(random_number);
}

int get_note1(void)
{
  int range, sum;
  int i, j, x;
  int array_notes = 7;

// Probability array
  //int A[array_notes] = {75, 30, 75, 0, 50, 75, 0, 75, 30, 75, 75, 0, 75, 0, 75 };
  int A[array_notes] = {100, 75, 75, 40, 50, 75, 20 };
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
    //cout << "Note " << j + 1 << " given x = " << x << endl;

    return(j);
}

/*
  Goodall Method

  get_note2() uses the probability of moving up or down the musical scale as
  opposed to get_note1() that uses the probability of a given note in the scale
  occurring
  This method is prone to instability by diverging from the initial condition
  continuously even with a balanced probability array (A).

  Can it be useful for short sequences?
*/
int get_note2(int last_note)
{
  int range, sum;
  int i, j, x;
  int array_notes = 15;

// Probability array    -7  -6  -5  -4  -3  -2  -1   0  +1  +2  +3  +4  +5  +6  +7
  int A[array_notes] = { 0,  0,  0,  0, 25, 75, 50, 75, 50, 75, 25,  0,  0,  0,  0 };
  //int A[array_notes] = {100, 75, 75, 40, 50, 75, 20 };
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

    i = last_note + j - 7;
    cout << "Note " << i + 1 << " given x = " << x << " and j = " << j << endl;

    return(i);
}


int get_space(void)
{   int j = 0;
    int x = (int)((double)my_rand() * (double)100/MY_RAND_MAX);

    if(x > 40)j = 1;
    return(j);
}


int main( void )
{
  RtMidiOut *midiout = 0;
  vector<unsigned char> message;
  int last_note = 60; // key of C
  int note1, note2, note3, note4, i, j;
  int notes[10][32];
  int order[20] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9};
  int modes[7][22] = {{48, 50, 52, 53, 55, 57, 59,
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
                        84},
                        {48, 50, 52, 54, 55, 57, 59,
                        60, 62, 64, 66, 67, 69, 71,
                        72, 74, 76, 78, 79, 81, 83,
                        84},
                        {48, 50, 52, 53, 55, 57, 58,
                        60, 62, 64, 65, 67, 69, 70,
                        72, 74, 76, 77, 79, 81, 82,
                        84},
                        {48, 50, 51, 53, 55, 56, 58,
                        60, 62, 63, 65, 67, 68, 70,
                        72, 74, 75, 77, 79, 80, 82,
                        84},
                        {48, 49, 51, 53, 54, 56, 58,
                        60, 61, 63, 65, 66, 68, 70,
                        72, 73, 75, 77, 78, 80, 82,
                        84}};

  int mode = 5; // 0 = Ionian, 1 = Dorian, 2 = Phrygian. 3 = Lydian
  // 4 = Mixolydian, 5 = Aeolian, 6 = Locrian

  // Fill notes array
  for(i = 0; i < 10; i++)
    for(j = 0; j < 32; j++)
    {
      if(get_space())
        notes[i][j] = -1;
      else
      {
        notes[i][j] = get_note1();
        //notes[i][j] = get_note2(last_note);
        last_note = notes[i][j];
      }
    }

  //exit(0);

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
  message[0] = 0x91; // Note on
  message[1] = modes[mode][0] - 12;
  message[2] = 90;
  midiout->sendMessage( &message );

  SLEEP(STIME * 4);

  for(i = 0; i < 20; i++)
  {
    cout << "Loop " << order[i];

    for (int g = 0; g < 8; g++)
    {
      if(notes[order[i]][g] < 0)
      {
        SLEEP(STIME);
        cout << " -1";
      }
      else
      {
        note1 = modes[mode][notes[order[i]][g]];
        note2 = modes[mode][notes[order[i]][g] + 2];
        note3 = modes[mode][notes[order[i]][g] + 4];
        note4 = modes[mode][notes[order[i]][g] + 6];

        cout << " " << note1;

        message[0] = 0x90; // Note on
        message[1] = note1;
        message[2] = 90;
        midiout->sendMessage( &message );

        cout << " " << note2;

        message[0] = 144; // Note on
        message[1] = note2;
        message[2] = 90;
        midiout->sendMessage( &message );

        cout << " " << note3;

        message[0] = 144; // Note on
        message[1] = note3;
        message[2] = 90;
        midiout->sendMessage( &message );

        cout << " " << note4;

        message[0] = 144; // Note on
        message[1] = note4;
        message[2] = 90;
        midiout->sendMessage( &message );

        SLEEP(STIME);

        message[0] = 0x80; // Note off
        message[1] = note1;
        message[2] = 90;
        midiout->sendMessage( &message );

        message[0] = 128; // Note off
        message[1] = note2;
        message[2] = 40;
        midiout->sendMessage( &message );

        message[0] = 128; // Note off
        message[1] = note3;
        message[2] = 40;
        midiout->sendMessage( &message );

        message[0] = 128; // Note off
        message[1] = note4;
        message[2] = 40;
        midiout->sendMessage( &message );
      }
    }
    cout << endl;
  }

  message[0] = 0x81; // Note off
  message[1] = modes[mode][0] - 12;
  message[2] = 90;
  midiout->sendMessage( &message );

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
