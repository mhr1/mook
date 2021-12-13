#!/usr/bin/env python


from MidiFile import MIDIFile
import random

modes = [[48, 50, 52, 53, 55, 57, 59,
                        60, 62, 64, 65, 67, 69, 71,
                        72, 74, 76, 77, 79, 81, 83,
                        84],
                        [48, 50, 51, 53, 55, 57, 58,
                        60, 62, 63, 65, 67, 69, 72,
                        72, 74, 75, 77, 79, 81, 82,
                        84],
                        [48, 49, 51, 53, 55, 56, 58,
                        60, 61, 63, 65, 67, 68, 72,
                        72, 73, 75, 77, 79, 80, 82,
                        84],
                        [48, 50, 52, 54, 55, 57, 59,
                        60, 62, 64, 66, 67, 69, 71,
                        72, 74, 76, 78, 79, 81, 83,
                        84],
                        [48, 50, 52, 53, 55, 57, 58,
                        60, 62, 64, 65, 67, 69, 70,
                        72, 74, 76, 77, 79, 81, 82,
                        84],
                        [48, 50, 51, 53, 55, 56, 58,
                        60, 62, 63, 65, 67, 68, 70,
                        72, 74, 75, 77, 79, 80, 82,
                        84],
                        [48, 49, 51, 53, 54, 56, 58,
                        60, 61, 63, 65, 66, 68, 70,
                        72, 73, 75, 77, 78, 80, 82,
                        84],
                        [48, 50, 51, 53, 55, 56, 59,
                        60, 62, 63, 65, 67, 68, 71,
                        72, 74, 75, 77, 79, 80, 83,
                        84]]
mode = 0; 
""" 0 = Ionian, 1 = Dorian, 2 = Phrygian. 3 = Lydian
  // 4 = Mixolydian, 5 = Aeolian, 6 = Locrian, 7 = Harmonic Minor
  // Melodic minor changes between going up the scale and going down. This
  // is a special case for later inclusion
"""
prnmode = ["Ionian", "Dorian", "Phrygian", "Lydian", "Mixolydian", "Aeolian", "Locrian", "Harmonic minor"]
prnkey = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
degrees  = [60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, 81, 83, 84]  # MIDI note number
#degrees  = [35, 38, 44] # Bass, snare, hihat on channel 9
bass = [36, 38, 40, 43, 45, 48]
track    = 0
channel  = 0
time     = 0    # In beats
duration = 1    # In beats
tempo    = 120   # In BPM
volume   = 50  # 0-127, as per the MIDI standard

MyMIDI = MIDIFile(3)  # One track, defaults to format 1 (tempo track is created
                      # automatically)
MyMIDI.addTrackName(track, time, "Sample Track")
MyMIDI.addTempo(track, time, tempo)

bass_track = [0, -1, -1, 3, 0, -1, -1, 4]

x = 0
key = 0
transpose = 0
print(prnmode[mode])
print(prnkey[key])

for time in range(0, 31):

    a = random.randint(0, 15)
    b = random.randint(0, 9)
    if(b > 3):
        MyMIDI.addNote(0, channel, modes[mode][a] + transpose + key, time, 1, volume)
    if(x == 0):
        a = random.randint(0, 7)
        MyMIDI.addNote(1, channel, modes[mode][a] + key, time, 7, volume)
        MyMIDI.addNote(1, channel, modes[mode][a + 2] + key, time, 7, volume)
        MyMIDI.addNote(1, channel, modes[mode][a + 4] + key, time, 7, volume)
        MyMIDI.addNote(1, channel, modes[mode][a + 6] + key, time, 7, volume)

    if(bass_track[x] > -1):
        MyMIDI.addNote(2, channel, modes[mode][bass_track[x]] -12 + key, time, 2, volume)

    x += 1
    if(x > 7): x = 0

# Bass

#   c = random.randint(0,6)
#    b = random.randint(0, 9)
#    if(b > 5):
#        MyMIDI.addNote(2, channel, modes[mode][c] - 12 + key, time, 2, volume)
    
        
x = 0
key = random.randint(0, 11)
mode = random.randint(0, 7)
print(prnmode[mode])
print(prnkey[key])

for time in range(32, 63):

    a = random.randint(0, 15)
    b = random.randint(0, 9)
    if(b > 3):
        MyMIDI.addNote(0, channel, modes[mode][a] + transpose + key, time, 1, volume)
    if(x == 0):
        a = random.randint(0, 7)
        MyMIDI.addNote(1, channel, modes[mode][a] + key, time, 7, volume)
        MyMIDI.addNote(1, channel, modes[mode][a + 2] + key, time, 7, volume)
        MyMIDI.addNote(1, channel, modes[mode][a + 4] + key, time, 7, volume)
        MyMIDI.addNote(1, channel, modes[mode][a + 6] + key, time, 7, volume)

    if(bass_track[x] > -1):
        MyMIDI.addNote(2, channel, modes[mode][bass_track[x]] -12 + key, time, 2, volume)

    x += 1
    if(x > 7): x = 0

# Bass

#    c = random.randint(0,6)
#    b = random.randint(0, 9)
#    if(b > 5):
#        MyMIDI.addNote(2, channel, modes[mode][c] - 12 + key, time, 2, volume)

x = 0
key = random.randint(0, 11)
mode = random.randint(0, 7)
print(prnmode[mode])
print(prnkey[key])

for time in range(64, 95):

    a = random.randint(0, 15)
    b = random.randint(0, 9)
    if(b > 3):
        MyMIDI.addNote(0, channel, modes[mode][a] + transpose + key, time, 1, volume)
    if(x == 0):
        a = random.randint(0, 7)
        MyMIDI.addNote(1, channel, modes[mode][a] + key, time, 7, volume)
        MyMIDI.addNote(1, channel, modes[mode][a + 2] + key, time, 7, volume)
        MyMIDI.addNote(1, channel, modes[mode][a + 4] + key, time, 7, volume)
        MyMIDI.addNote(1, channel, modes[mode][a + 6] + key, time, 7, volume)

    if(bass_track[x] > -1):
        MyMIDI.addNote(2, channel, modes[mode][bass_track[x]] -12 + key, time, 2, volume)

    x += 1
    if(x > 7): x = 0

# Bass

 #   c = random.randint(0,6)
 #   b = random.randint(0, 9)
#    if(b > 5):
#        MyMIDI.addNote(2, channel, modes[mode][c] - 12 + key, time, 2, volume)
x = 0
key = random.randint(0, 11)
mode = random.randint(0, 7)
print(prnmode[mode])
print(prnkey[key])

for time in range(96, 127):

    a = random.randint(0, 15)
    b = random.randint(0, 9)
    if(b > 3):
        MyMIDI.addNote(0, channel, modes[mode][a] + transpose + key, time, 1, volume)
    if(x == 0):
        a = random.randint(0, 7)
        MyMIDI.addNote(1, channel, modes[mode][a] + key, time, 7, volume)
        MyMIDI.addNote(1, channel, modes[mode][a + 2] + key, time, 7, volume)
        MyMIDI.addNote(1, channel, modes[mode][a + 4] + key, time, 7, volume)
        MyMIDI.addNote(1, channel, modes[mode][a + 6] + key, time, 7, volume)

    if(bass_track[x] > -1):
        MyMIDI.addNote(2, channel, modes[mode][bass_track[x]] -12 + key, time, 2, volume)

    x += 1
    if(x > 7): x = 0

# Bass

#    c = random.randint(0,6)
#    b = random.randint(0, 9)
#    if(b > 5):
 #       MyMIDI.addNote(2, channel, modes[mode][c] - 12 + key, time, 2, volume)



with open("major-scale.mid", "wb") as output_file:
    MyMIDI.writeFile(output_file)
