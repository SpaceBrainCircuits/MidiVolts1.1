/*
  MidiVolts - Sample code for public use by Space Brain Circuits...
*/

// ************************************* USER DEFINE PARAMETERS *************************************

// PLEASE CHOOSE MODE
// 1 VOICE MONOPHONIC ON V0 WITH VELOCITY ON V1, AFTERTOUCH ON V2, AND MOD WHEEL ON V3: ............1
// 2 VOICE DUOPHONIC ON V0, V1 WITH VELOCITY ON V2 AND V3 RESPECTIVELY: ............................2
// 3 VOICE POLYPHONIC ON V0, V1, V2 WITH MOD WHEEL ON V3: ..........................................3
// 4 VOICE POLYPHONIC ON V0, V1, V2, V3: ...........................................................4
// 4 VOICE UNISON ON V0, V1, V2, V3: ...............................................................5
#define MODE 3

// CHOOSE RANGE OF SEMITONES FOR PITCH BEND WHEEL (DEFAULT UP: 2, DEFAULT DOWN: 12)
#define PITCH_BEND_SEMITONES_UP 2
#define PITCH_BEND_SEMITONES_DOWN 12

// CHOOSE CONTROL FUNCTION DURING CC STATUS BYTE 192 (DEFAULT 1: MOD WHEEL)
#define CONTROL_FUNCTION 1

// CHOOSE STATUS BYTE WHEN READING AFTERTOUCH (DEFAULT 160: POLYPHONIC AFTERTOUCH)
#define AFTERTOUCH 160

// CHOOSE PIN TO OUTPUT CLOCK PULSE (DEFAULT: 8)
#define CLOCK_PIN 8

// PIN NUMBER ASSIGNMENTS
#define V0 0
#define V1 1
#define V2 2
#define V3 3

// **************************************************************************************************


#include "MidiVolts.h"
#include "Midi.h"

Midi midi; //Initialize class for listening to Midi Messages

MidiVolts voice[4] = {MidiVolts(10, V0), MidiVolts(11, V1), MidiVolts(12, V2), MidiVolts(13, V3)}; // MidiVolts(GatePin, Channel)

void setup() {

// for fine tune calibration, please see README or Calibration Guide
// sample code for calibration
//  voice[0].Gain = 1;
//  voice[0].Offset = 0;
//  voice[1].Gain = 1.008;
//  voice[1].Offset = 0;
//  voice[2].Gain = 1;
//  voice[2].Offset = 0;
//  voice[3].Gain = 1.004;
//  voice[3].Offset = 0.003;

  Serial.begin(31250); //Midi baud
  Wire.begin();

  pinMode(CLOCK_PIN, OUTPUT);

  if (MODE == 1) {
    voice[V0].VelocityPin = V1; //pitch CV set on V0 will have corresponding Velocity on V1
  }
  else if (MODE == 2)
  {
    voice[V0].VelocityPin = V2; //pitch CV set on V0 will have corresponding Velocity on V2
    voice[V1].VelocityPin = V3; //pitch CV set on V1 will have corresponding Velocity on V3
  }
}

void loop() {
  midi.Listen(); // Listen for Midi Messages

  if (midi.On == true)
  {
    NoteOn(midi.MidiNum, midi.Velocity); // Set Voltage to Voice, Velocity and turn on Gate
  }

  if (midi.Off == true)
  {
    NoteOff(midi.MidiNum); // Turn off Gate
  }

  if (midi.BendOn == true) // Bend notes
  {
    if (MODE == 1)
    {
      voice[V0].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
    }
    else if (MODE == 2)
    {
      voice[V0].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
      voice[V1].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
    }
    else if (MODE == 3)
    {
      voice[V0].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
      voice[V1].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
      voice[V2].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
    }
    else // MODE 4 or 5
    {
      voice[V0].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
      voice[V1].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
      voice[V2].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
      voice[V3].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
    }
  }

  if (midi.ControlOn == true) // CC
  {
    if (midi.ControlFunction == CONTROL_FUNCTION)
    {
      if (MODE == 1 || MODE == 3)
      {
        voice[V3].CC(midi.Control);
      }
    }
  }

  if (midi.AftertouchOn == true)
  {
    if (midi.Aftertouch == AFTERTOUCH)
    {
      if (MODE == 1)
      {
        voice[V2].CC(midi.Aftertouch);
      }
    }
  }

  if (midi.ClockOn == true) // Clock
  {
    digitalWrite(CLOCK_PIN, HIGH);
  }
  else if (midi.ClockOff == true)
  {
    digitalWrite(CLOCK_PIN, LOW);
  }
}

void NoteOn(byte midiNum, byte velocity) // Sets voltage to next available voice.
{
  if (MODE != 5)
  {
    for (int i = 0; i < MODE; i++) {

      if (voice[i].noteState == false)
      {
        voice[i].NoteOn(midiNum);
        voice[i].VelocityOn(velocity);

        return;
      }
      else if (i == (MODE - 1))
      {
        voice[i].NoteOn(midiNum);
        voice[i].VelocityOn(velocity);
      }
    }
  }
  else
  {
    voice[V0].NoteOn(midiNum);
    voice[V1].NoteOn(midiNum);
    voice[V2].NoteOn(midiNum);
    voice[V3].NoteOn(midiNum);
  }
}

void NoteOff(byte midiNum)
{
  if (MODE != 5)
  {
    for (int i = 0; i < MODE; i++) {
      if (voice[i].noteState == true && voice[i].MidiNum == midiNum)
      {
        voice[i].NoteOff();
        return;
      }
    }
  }
  else
  {
    if (voice[0].noteState == true && voice[0].MidiNum == midiNum)
    {
      voice[V0].NoteOff();
      voice[V1].NoteOff();
      voice[V2].NoteOff();
      voice[V3].NoteOff();
    }
  }
}
