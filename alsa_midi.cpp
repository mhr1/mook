#/*
exec gcc $0 -Wall -O2 -o miditest -lasound
#*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <alsa/asoundlib.h>

int dest_client = 14, dest_port = 0; // was 128 and 0


snd_seq_t * connect(int * source_port) {
	snd_seq_t * s;
	if (snd_seq_open(&s, "hw", SND_SEQ_OPEN_OUTPUT, 0) < 0) {
		fprintf(stderr, "Error: snd_seq_open\n");
		return 0;
	}
	*source_port = snd_seq_create_simple_port(s, "earpractice",
		SND_SEQ_PORT_CAP_READ, SND_SEQ_PORT_TYPE_MIDI_GENERIC |
		SND_SEQ_PORT_TYPE_APPLICATION);
	if (*source_port < 0) {
		fprintf(stderr, "Error: snd_seq_create_simple_port\n");
		snd_seq_close(s);
		return 0;
	}
	if (snd_seq_connect_to(s, *source_port, dest_client, dest_port) < 0) {
		fprintf(stderr, "Error: snd_seq_connect_to\n");
		snd_seq_close(s);
		return 0;
	}
	return s;
}

int randnum(int n) {
	return (int)(rand()/(RAND_MAX+1.0)*n);
}

snd_seq_event_t ev;
snd_seq_t * seq;
int port;

void send_ev() {
	snd_seq_ev_set_direct(&ev);
	snd_seq_ev_set_source(&ev, port);
	snd_seq_ev_set_dest(&ev, dest_client, dest_port);
	snd_seq_event_output(seq, &ev);
}

void note_on(int note) {
	snd_seq_ev_set_noteon(&ev, 0, note, 90);
	send_ev();
}

void note_off(int note) {
	snd_seq_ev_set_noteoff(&ev, 0, note, 0);
	send_ev();
}

int main(int argc, char ** argv)
{
	//snd_seq_t seq;
  snd_seq_open(&seq, "default", SND_SEQ_OPEN_DUPLEX, 0);

	int port;
	port = snd_seq_create_simple_port(seq, "my port",
	            SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_WRITE,
	            SND_SEQ_PORT_TYPE_APPLICATION);

	snd_seq_event_t ev;
  snd_seq_ev_clear(&ev);
  snd_seq_ev_set_direct(&ev);

  /* either */
  snd_seq_ev_set_dest(&ev, 14, 0); /* send to 64:0 (ev, port, 0)*/
  /* or */
  //snd_seq_ev_set_subs(&ev);        /* send to subscribers of source port */

  snd_seq_ev_set_noteon(&ev, 0, 60, 127);
  snd_seq_event_output(seq, &ev);
	snd_seq_drain_output(seq);
usleep(3000000);
	snd_seq_ev_set_noteoff(&ev, 0, 60, 0);
  snd_seq_event_output(seq, &ev);
snd_seq_drain_output(seq);

	/*srand(time(0));
	seq = connect(&port);
	if (!seq)
	{
		printf("Error\n");
	 	return 1;
	}

	//note_on(54);
	//note_on(64);
	note_on(67);
	snd_seq_drain_output(seq);
	usleep(3000000);
	//note_off(60);
	//note_off(64);
	note_off(67);
snd_seq_drain_output(seq);
	snd_seq_close(seq);

	usleep(1000000);*/
	return 0;
}
