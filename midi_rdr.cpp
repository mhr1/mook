#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

int main(void)
{
	char c, hdr[16];
	uint8_t k;
	int i, length, format, ntracks, time_div;
	
	ifstream fin("test.mid");

// Get header ID

	for(i = 0; i < 4; i++) fin.get(hdr[i]);
	
	if(!strcmp(hdr, "MThd"))
		cout << "Got header \n";
		
// Get length - 4 bytes
	for(i = 0; i < 4; i++) fin.get(hdr[i]);
	length = (int)hdr[3];

// Get format - 2 bytes 0 = single track file format 1 = multiple track file format 
// 2 = multiple song file format
	for(i = 0; i < 2; i++){fin.get(hdr[i]); cout << hdr[i] << endl;}
	
	format = (int)hdr[1];
// Get n -  2bytes number of track chunks that follow the header chunk
	for(i = 0; i < 2; i++) fin.get(hdr[i]);
	
	ntracks = (int)hdr[1];

// Get division	- 2 bytes  unit of time for delta timing. If the value is positive, 
// then it represents the units perbeat. For example, +96 would mean 96 ticks per beat. 
// If the value is negative, delta times are in SMPTE compatible units.
	for(i = 0; i < 2; i++){fin.get(hdr[i]); cout << hdr[i] << endl;}
	
	time_div = ((int)hdr[0] * 256) + (int)hdr[1];
	
	cout << "Header data: length " << length << " and format " << format << endl;
	cout << "Number of tracks " << ntracks << " and time division " << time_div << endl;
	
	// Get Track chunk ID

	for(i = 0; i < 4; i++) fin.get(hdr[i]);
	
	if(!strcmp(hdr, "MTrk"))
		cout << "Got Track \n";
		
// Get track length - in bytes after this 
	for(i = 0; i < 4; i++) fin.get(hdr[i]);
	
	int track_length = (int)hdr[3]	+ (256 * (int)hdr[2]) + (65536 * (int)hdr[1])
		+ (16777216 * (int)hdr[0]);
		
	cout << "Track length (bytes) = " << track_length << endl;
	
// Get Track event - v_time
	int v_time;
	
	do
	{	
		fin.get(c);
		k = (int)c;
		cout << hex << (int)k << endl;
		
	}while(k >= 0x80);
	
	v_time = (int)c;
	cout << "V Time " << v_time << endl;
	
// Get Event
	fin.get(c);
	k = (int)c;
	cout << hex << (int)k << endl;
	if(k == 0xff) 
		cout << "Meta Event\n";
	else if(k == 0xf0 || k == 0xf7) 
		cout << "Sysex event\n";
	else
		cout << "Midi event\n";
		
	for(i = 0; i < 32; i++)
	{
		fin.get(c);
		k = (int)c;
		cout << hex << (int)k << endl;
	}
	
	fin.close();
	return(0);
}