#include <vector>   // For std::vector<>
#include <cstring>  // For std::strlen()
#include <cstdio>   // For std::fopen(), std::fwrite(), std::fclose()
#include <iostream>


using namespace std;

class midi_vector: public vector<unsigned char>
{
public:
    // Methods for appending raw data into the vector:
    template<typename... arg_list>
    void add_args(unsigned char data, arg_list...args)
    {
        push_back(data);
        add_args(args...);
    }
    
    template<typename... arg_list>
    void add_args(const char* s, arg_list...args)
    {
        insert(end(), s, s + std::strlen(s));
        add_args(args...);
    }
    
    void add_args() { }
};

/* Define a class which encodes MIDI events into a track */
class MIDItrack: public midi_vector
{
protected:
    unsigned delay, running_status;
public:
    MIDItrack()
        : midi_vector(), delay(0), running_status(0)
    {
    }
    
    // Methods for indicating how much time elapses:
    void AddDelay(unsigned amount) { delay += amount; }
    
    void AddVarLen(unsigned t)
    {
        if(t >> 21) add_args(0x80 | ((t >> 21) & 0x7F));
        if(t >> 14) add_args(0x80 | ((t >> 14) & 0x7F));
        if(t >>  7) add_args(0x80 | ((t >>  7) & 0x7F));
        add_args(((t >> 0) & 0x7F));
    }
    
    void Flush()
    {
        AddVarLen(delay);
        delay = 0;
    }
    
    // Methods for appending events into the track:
    template<typename... Args>
    void AddEvent(unsigned char data, Args...args)
    {
        /* MIDI tracks have the following structure:
         *
         * { timestamp [metaevent ... ] event } ...
         *
         * Each event is prefixed with a timestamp,
         * which is encoded in a variable-length format.
         * The timestamp describes the amount of time that
         * must be elapsed before this event can be handled.
         *
         * After the timestamp, comes the event data.
         * The first byte of the event always has the high bit on,
         * and the remaining bytes always have the high bit off.
         *
         * The first byte can however be omitted; in that case,
         * it is assumed that the first byte is the same as in
         * the previous command. This is called "running status".
         * The event may furthermore beprefixed
         * with a number of meta events.
         */
       Flush();
       if(data != running_status) add_args(running_status = data);
       add_args(args...);
    }
    void AddEvent() { }
    
    template<typename... Args>
    void AddMetaEvent(unsigned char metatype, unsigned char nbytes, Args...args)
    {
        Flush();
        add_args(0xFF, metatype, nbytes, args...);
    }
    
    // Key-related parameters: channel number, note number, pressure
    void KeyOn(int ch, int n, int p)    { if(n>=0)AddEvent(0x90|ch, n, p); }
    void KeyOff(int ch, int n, int p)   { if(n>=0)AddEvent(0x80|ch, n, p); }
    void KeyTouch(int ch, int n, int p) { if(n>=0)AddEvent(0xA0|ch, n, p); }
    // Events with other types of parameters:
    void Control(int ch, int c, int v) { AddEvent(0xB0|ch, c, v); }
    void Patch(int ch, int patchno)    { AddEvent(0xC0|ch, patchno); }
    void Wheel(int ch, unsigned value)
        { AddEvent(0xE0|ch, value&0x7F, (value>>7)&0x7F); }
    
    // Methods for appending metadata into the track:
    void AddText(int texttype, const char* text)
    {
        AddMetaEvent(texttype, std::strlen(text), text);
    }
};

/* Define a class that encapsulates all methods needed to craft a MIDI file. */
class MIDIfile: public midi_vector
{
protected:
    std::vector<MIDItrack> tracks;
    unsigned deltaticks, tempo;
public:
    MIDIfile()
        : midi_vector(), tracks(), deltaticks(1000), tempo(1000000)
    {
    }
    
    void AddLoopStart()  { (*this)[0].AddText(6, "loopStart"); }
    void AddLoopEnd()    { (*this)[0].AddText(6, "loopEnd"); }
    
    MIDItrack& operator[] (unsigned trackno)
    {
        if(trackno >= tracks.size())
        {
            tracks.reserve(16);
            tracks.resize(trackno+1);
        }
        
        MIDItrack& result = tracks[trackno];
        if(result.empty())
        {
            // Meta 0x58 (misc settings):
                //      time signature: 4/2
                //      ticks/metro:    24
                //      32nd per 1/4:   8
            result.AddMetaEvent(0x58,4,  4,2, 24,8);
            // Meta 0x51 (tempo):
            result.AddMetaEvent(0x51,3,  tempo>>16, tempo>>8, tempo);
        }
        return result;
    }
    
    void Finish()
    {
        clear();
        add_args(
            // MIDI signature (MThd and number 6)
            "MThd", 0,0,0,6,
            // Format number (1: multiple tracks, synchronous)
            0,1,
            tracks.size() >> 8, tracks.size(),
            deltaticks    >> 8, deltaticks);
            
        for(unsigned a=0; a<tracks.size(); ++a)
        {
            // Add meta 0x2F to the track, indicating the track end:
            tracks[a].AddMetaEvent(0x2F, 0);
            // Add the track into the MIDI file:
            add_args("MTrk",
                tracks[a].size() >> 24,
                tracks[a].size() >> 16,
                tracks[a].size() >>  8,
                tracks[a].size() >>  0);
            insert(end(), tracks[a].begin(), tracks[a].end());
        }
    }
};

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

int main()
{
    // Now that we have a class that can create MIDI files, let's create
    // music.
    
    // Begin with some chords.
    static const int chords[][3] =
    {
        { 12,4,7 }, // +C  E  G  = 0
        { 12,9,5 }, // +C  A  F  = 1
        { 12,8,3 }, // +C  G# D# = 2
        { 12,7,3 }, // +C  G  D# = 3
        { 12,5,8 }, // +C  F  G# = 4
        { 12,3,8 }, // +C  D# G# = 5
        { 11,2,7 }, //  B  D  G  = 6
        { 10,2,7 }, // A#  D  G  = 7
        { 14,7,5 }, // +D  G  F  = 8
        { 14,7,11 },// +D  G  B  = 9
        { 14,19,11 }// +D +G  B  = 10
    };
    const char x = 99; // Arbitrary value we use here to indicate "no note"
    static const char chordline[64] =
    {
        0,x,0,0,x,0,x, 1,x,1,x,1,1,x,1,x,  2,x,2,2,x,2,x, 3,x,3,x,3,3,x,3,x,
        4,x,4,4,x,4,x, 5,x,5,x,5,5,x,5,x,  6,7,6,x,8,x,9,x,10,x,x,x,x,x,x,x
    };
    static const char chordline2[64] =
    {
        0,x,x,x,x,x,x, 1,x,x,x,x,x,x,x,x,  2,x,x,x,x,x,x, 3,x,x,x,x,x,x,x,x,
        4,x,x,x,x,x,x, 5,x,x,x,x,x,x,x,x,  6,x,x,x,x,x,x,x, 6,x,x,x,x,x,x,x
    };
    static const char bassline[64] =
    {
        0,x,x,x,x,x,x, 5,x,x,x,x,x,x,x,x,  8,x,x,0,x,3,x, 7,x,x,x,x,x,x,x,x,
        5,x,x,x,x,x,x, 3,x,x,x,x,x,x,x,x,  2,x,x,x,x,x,x,-5,x,x,x,x,x,x,x,x
    };
    static const char fluteline[64] =
    {
        12,x,12,12, x,9, x, 17,x,16,x,14,x,12,x,x,
         8,x, x,15,14,x,12,  x,7, x,x, x,x, x,x,x,
         8,x, x, 8,12,x, 8,  x,7, x,8, x,3, x,x,x,
         5,x, 7, x, 2,x,-5,  x,5, x,x, x,x, x,x,x
    };
    
    MIDIfile file;
    //file.AddLoopStart();
    
    /* 
    //Choose instruments ("patches") for each channel: 
    static const char patches[16] =
    {
        0,0,0, 52,52,52, 48,48,48, 0,0,0,0,0, 35,74
        // 0=piano, 52=choir aahs, 48=strings, 35=fretless bass, 74=pan flute 
    };
    for(unsigned c=0; c<16; ++c)
        if(c != 10) // Patch any other channel but not the percussion channel.
            file[0].Patch(c, patches[c]);
    
    int keys_on[16] = {-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1 };
    for(unsigned loops=0; loops<2; ++loops)
    {
        for(unsigned row=0; row<128; ++row)
        {
            for(unsigned c=0; c<16; ++c)
            {
                int note = x, add = 0, vol = 127;
                if(c < 3) // Piano chord
                  { int chord = chordline[row%64];
                    if(chord != x) note = chords[chord][c%3], add=12*5, vol=0x4B; }
                else if(c >= 3 && c < 5) // Aux chord (choir)
                  { int chord = chordline2[row%64];
                    if(chord != x) note = chords[chord][c%3], add=12*4, vol=0x50; }
                else if(c >= 6 && c < 8) // Aux chord (strings)
                  { int chord = chordline2[row%64];
                    if(chord != x) note = chords[chord][c%3], add=12*5, vol=0x45; }
                else if(c == 14) // Bass
                    note = bassline[row%64], add=12*3, vol=0x6F;
                else if(c == 15 && row >= 64) // Flute
                    note = fluteline[row%64], add=12*5, vol=0x6F;
                    
                if(note == x && (c<15 || row%31)) continue;
                
                file[0].KeyOff(c, keys_on[c], 0x20);
                keys_on[c] = -1;
                
                if(note == x) continue;
                
                file[0].KeyOn(c, keys_on[c] = note+add, vol);
            }
            file[0].AddDelay(200);
        }
        if(loops == 0) file.AddLoopEnd();
    }
    
    file[0].AddEvent(0x90|1, 0 + (12 * 4), 80);
    file[0].AddEvent(0x90|1, 4 + (12 * 4), 80);
    file[0].AddEvent(0x90|1, 7 + (12 * 4), 80);
    //file[0].KeyOff(1, -1, 0x20);
    file[0].AddDelay(1000); 
                //keys_on[c] = -1;                           
                
    file[0].KeyOn(1, 0 + (12 * 5), 0x4B);
    file[0].AddDelay(200);
    file[0].KeyOff(1, -1, 0x20);
                //keys_on[c] = -1;                           
                
    file[0].KeyOn(1, 2 + (12 * 5), 0x4B);
    file[0].AddDelay(200);
    file[0].KeyOff(1, -1, 0x20);
                //keys_on[c] = -1;                           
                
    file[0].KeyOn(1, 4 + (12 * 5), 0x4B);
    file[0].AddDelay(200);
    file[0].KeyOff(1, -1, 0x20);
                //keys_on[c] = -1;                           
                
    file[0].KeyOn(1, 5 + (12 * 5), 0x4B);
    file[0].AddDelay(200);
    file[0].KeyOff(1, -1, 0x20);
                //keys_on[c] = -1;                           
                
    file[0].KeyOn(1, 7 + (12 * 5), 0x4B);
    file[0].AddDelay(200);
    file[0].KeyOff(1, -1, 0x20);
    
    file[0].KeyOn(1, 9 + (12 * 5), 0x4B);
    file[0].AddDelay(200);
    file[0].KeyOff(1, -1, 0x20);
       
    file[0].KeyOn(1, 11 + (12 * 5), 0x4B);
    file[0].AddDelay(200);
    file[0].KeyOff(1, -1, 0x20);
       
    file[0].KeyOn(1, 12 + (12 * 5), 0x4B);
    file[0].AddDelay(200);
    file[0].KeyOff(1, -1, 0x20);
    */
    int y, range = 15, r = 4;
    int mynotes[22] = {0,2,4,5,7,9,11,12,14,16,17,19,21,23,24,26,28,29,31,33,35,36};
    
    for(int i = 0; i < 64; i++)
    {
    	y = (int)((double)my_rand() * (double)range/MY_RAND_MAX); 
    	file[0].AddEvent(0x90|1, mynotes[y] + (12 * 4), 80);
    	
    	if(r++ > 2)
    	{
    		file[0].AddEvent(0x90|1, 0 + (12 * 4), 80);
    		file[0].AddEvent(0x90|1, 4 + (12 * 4), 80);
    		file[0].AddEvent(0x90|1, 7 + (12 * 4), 80);
    		r = 0;
    	}
    		
    	file[0].AddDelay(1000);
    } 
    //file.AddLoopEnd();
    
    file.Finish();
    
    FILE* fp = std::fopen("test.mid", "wb");
    std::fwrite(&file.at(0), 1, file.size(), fp);
    std::fclose(fp);
    
    //cout << file;
    
    //ofstream outbin( "binary.bin", ios::binary );
  //outbin.write( reinterpret_cast <const char*> (&year), sizeof( year ) );
  //outbin.close();

    
    return 0;
}