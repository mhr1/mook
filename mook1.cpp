
//*****************************************//
//  midiout.cpp
//  by Gary Scavone, 2003-2004.
//
//  Simple program to test MIDI output.
//
//*****************************************//

#include <iostream>
#include <cstdlib>
#include <signal.h> // for signal (interrupt??)
//#include <stdio.h>
//#include <gtk/gtk.h>
#include "RtMidi.h"
bool done;

using namespace std;

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

#define STIME 100

static unsigned long my_seed = 1223; //11056; //default random seed
#define MY_RAND_MAX 2147483646 // 2^32 - 2, 1 less than mod value in rand no gen

unsigned long my_rand(void)
{
	unsigned long random_number;

	 random_number = (16807 * my_seed) % (MY_RAND_MAX + 1);
	my_seed = random_number;
	return(random_number);
}

class note
{
public:
  note(){};
  ~note(){};

  int get_note(int last_note)
  {
    int range = 0, sum = 0;
    int i, j, x;

  	for(i = 0; i < 7; i++)
  		range += A[last_note][i];

    x = (int)((double)my_rand() * (double)range/MY_RAND_MAX);

    for(j = 0; j < 7; j++)
    {
      sum += A[last_note][j];
      if(x <= sum) break;
    }
    return(j);
  };

  int get_note(int last_note, int penum_note)
  {
    int range = 0, sum = 0;
    int i, j, x;

  	for(i = 0; i < 7; i++)
  		range += B[last_note][penum_note][i];

      x = (int)((double)my_rand() * (double)range/MY_RAND_MAX);

      for(j = 0; j < 7; j++)
      {
        sum += B[last_note][penum_note][j];
        if(x <= sum)break;
      }
      //cout << "Note " << j << " given x = " << x <<  endl;

      return(j);
  };

  int rand_A(void)
  {
    int i, j;
    for(i = 0; i < 7; i++)
      for(j = 0; j < 7; j++)
        A[i][j] = (int)((double)my_rand() * (double)99/MY_RAND_MAX);
    return(1);
  };

  int rand_B(void)
  {
    int i, j, k;
    for(i = 0; i < 7; i++)
      for(j = 0; j < 7; j++)
        for(k = 0; k < 7; k++)
          B[i][j][k] = (int)((double)my_rand() * (double)99/MY_RAND_MAX);
    return(1);
  };

  int reset_B(void)
  {
    int i, j, k;
    for(i = 0; i < 7; i++)
      for(j = 0; j < 7; j++)
        for(k = 0; k < 7; k++)
          B[i][j][k] = 0;
    return(1);
  };

  int set_ones_B(void)
  {
    int i, j, k;
    for(i = 0; i < 7; i++)
      for(j = 0; j < 7; j++)
        for(k = 0; k < 7; k++)
          B[i][j][k] = 1;
    return(1);
  };

  int set_B(int i, int j, int k, int value)
  {
    B[i][j][k] = value;
    return(1);
  };

  int inc_B(int i, int j, int k)
  {
    B[i][j][k]++;
    return(1);
  };

  int show_A(void)
  {
    int i, j;
    for(i = 0; i < 7; i++)
    {
      for(j = 0; j < 7; j++)
        cout << A[i][j] << " ";
      cout << endl;
    }
    return(1);
  };

  int show_B(void)
  {
    int i, j, k;
    for(i = 0; i < 7; i++)
    {
      for(j = 0; j < 7; j++)
      {
        for(k = 0; k < 7; k++)
          cout << B[i][j][k] << " ";
        cout << endl;
      }
      cout << endl;
    }
    return(1);
  };

private:
  int A[7][7] = {{  0,  0,  0,  0,  0,  0, 99 }, // Last_note = 0
                 {  0,  0,  0,  0, 25, 75, 50 },
                 {  0,  0,  0,  0, 25, 75, 50 },
                 {  0,  0,  0,  0, 25, 75, 50 },
                 {  0,  0,  0,  0, 25, 75, 50 },
                 {  0,  0,  0,  0, 25, 75, 50 },
                 { 99,  0,  0,  0,  0,  0,  0 }};

  int B[7][7][7];
};

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
  Goodall Method (get_note2())

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


int get_note3(int last_note)
{
  int range, sum;
  int i, j, x;
// A[last_note][x]
  int A[7][7] = {{  0,  0,  0,  0,  0,  0, 99 }, // Last_note = 0
                 {  0,  0,  0,  0, 25, 75, 50 },
                 {  0,  0,  0,  0, 25, 75, 50 },
                 {  0,  0,  0,  0, 25, 75, 50 },
                 {  0,  0,  0,  0, 25, 75, 50 },
                 {  0,  0,  0,  0, 25, 75, 50 },
                 { 99,  0,  0,  0,  0,  0,  0 }};

  range = 0;
	for(i = 0; i < 7; i++)
		range += A[last_note][i];

    x = (int)((double)my_rand() * (double)range/MY_RAND_MAX);

		sum = 0;
		j = 0;
  //while(x >= sum)sum += A[j++];
    for(j = 0; j < 7; j++)
    {
      sum += A[last_note][j];
      if(x <= sum)
        break;
    }
    cout << "Note " << j << " given x = " << x <<  endl;

    return(j);
}


int get_space(void)
{   int j = 0;
    int x = (int)((double)my_rand() * (double)100/MY_RAND_MAX);

    if(x > 100)j = 1;
    return(j);
}

static void finish(int ignore){ done = true; }

int main( void )
{
  RtMidiOut *midiout = 0;
  vector<unsigned char> message;

  note mynote;

  int this_note = 0, last_note = 0, pen_note = 0;
  int note1, note2, note3, note4, i, j;
  int notes[10][32];
  int order[20] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int modes[8][22] = {{48, 50, 52, 53, 55, 57, 59,
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
                        84},
                        {48, 50, 51, 53, 55, 56, 59,
                        60, 62, 63, 65, 67, 68, 71,
                        72, 74, 75, 77, 79, 80, 83,
                        84}};

  int mode = 0; // 0 = Ionian, 1 = Dorian, 2 = Phrygian. 3 = Lydian
  // 4 = Mixolydian, 5 = Aeolian, 6 = Locrian, 7 = Harmonic Minor
  // Melodic minor changes between going up the scale and going down. This
  // is a special case for later inclusion

  mynote.set_ones_B();
  /*mynote.set_B(0, 0, 6, 99);
  mynote.set_B(0, 6, 2, 99);
  mynote.set_B(6, 2, 0, 99);*/
  //mynote.rand_B();
  //mynote.rand_B();
  mynote.show_B();

  RtMidiIn *midiin = new RtMidiIn();
  std::vector<unsigned char> imessage;
  int nBytes;
  double stamp;
 // Check available ports.
  unsigned int iPorts = midiin->getPortCount();
  if ( iPorts == 0 ) {
    std::cout << "No ports available!\n";
    delete midiin;
    return(0);
  }
  midiin->openPort( 0 );
  // Don't ignore sysex, timing, or active sensing messages.
  midiin->ignoreTypes( false, false, false );
  // Install an interrupt handler function.
  done = false;
  (void) signal(SIGINT, finish);
  // Periodically check input queue.
  std::cout << "Reading MIDI from port ... quit with Ctrl-C.\n";

  while ( !done ) {
    stamp = midiin->getMessage( &imessage );
    nBytes = imessage.size();
    for ( i=0; i<nBytes; i++ )
    {
      std::cout << "Byte " << i << " = " << (int)imessage[i] << ", ";

      if(i == 1)
      {
        this_note = (int)imessage[i] - 60;
          mynote.inc_B(pen_note, last_note, this_note);
          pen_note = last_note;
          last_note = this_note;
      }

    }

    if ( nBytes > 0 )
    {
      std::cout << "stamp = " << stamp << std::endl;
    }
    SLEEP( 10 );
    if(kbhit())break;
  }
  mynote.show_B();

  cout << "Normal exit\n";

  delete midiin;
//return(0);

  // Fill notes array
  for(i = 0; i < 10; i++)
    for(j = 0; j < 32; j++)
    {
      if(get_space())
        notes[i][j] = -1;
      else
      {
        //notes[i][j] = mynote.get_note(pen_note, last_note);
        notes[i][j] = mynote.get_note(last_note, pen_note); // Will this fix
          // sequences being learned in reverse?
        pen_note = last_note;
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

  for(i = 0; i < 10; i++)
  {
    cout << "Loop " << order[i];

    for (int g = 0; g < 32; g++)
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

        //cout << " " << note2;

        message[0] = 144; // Note on
        message[1] = note2;
        message[2] = 90;
        midiout->sendMessage( &message );

        //cout << " " << note3;

        message[0] = 144; // Note on
        message[1] = note3;
        message[2] = 90;
        midiout->sendMessage( &message );

        //cout << " " << note4;

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
