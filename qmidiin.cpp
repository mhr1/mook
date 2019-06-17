 // qmidiin.cpp
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include "RtMidi.h"
bool done;

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

static void finish(int ignore){ done = true; }

int main()
{
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
  try {if(!chooseMidiPort(midiout)) cout << "fail\n";}
  catch (RtMidiError &error)
  {
    error.printMessage();
  }


//midiout->openPort( 0 );
// Program change: 192, 5
message.push_back( 0xF1 );
message.push_back(60);
midiout->sendMessage( &message );

// Control Change: 176, 7, 100 (volume)
message[0] = 0xB0; //176;
message[1] = 7;
message.push_back( 100 );
midiout->sendMessage( &message );

message[0] = 0x90; // Note on
message[1] = 60;
message[2] = 90;
midiout->sendMessage( &message );
SLEEP(500);
message[0] = 0x80; // Note off
message[1] = 60;
message[2] = 90;
midiout->sendMessage( &message );

//return(0);


  RtMidiIn *midiin = new RtMidiIn();
  std::vector<unsigned char> imessage;
  int nBytes, i;
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
        message[0] = 0x90; // Note on
        message[1] = (int)imessage[1];
        message[2] = 90;
        midiout->sendMessage( &message );
        SLEEP(500);
        message[0] = 0x80; // Note off
        message[1] = (int)imessage[1];
        message[2] = 90;
        midiout->sendMessage( &message );
      }

    }

    if ( nBytes > 0 )
    {
      std::cout << "stamp = " << stamp << std::endl;

      /*message[0] = 0x90; // Note on
      message[1] = (int)imessage[1];
      message[2] = 90;
      midiout->sendMessage( &message );
      SLEEP(500);
      message[0] = 0x80; // Note off
      message[1] = (int)imessage[1];
      message[2] = 90;
      midiout->sendMessage( &message );*/
    }
    SLEEP( 10 );
  }


  delete midiin;
  delete midiout;
  return 0;
}
