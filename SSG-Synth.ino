/* 
 * Audio synthesizer based on the Yamaha YM2149F sound chips
 * 
 * Compatibility :
 * 
 * YM2149F (SSG)
 * Two possible operating modes:
 * - 3 monophonic instruments (default)
 * - 1 polyphonic (3 voices) instrument
 * 
 * +--------+      +---------+      +---------+
 * | MIDI   |      | ARDUINO |      |   SSG   |
 * | INPUT  | ---> |   UNO   | ---> | YM2149F | ---> Sound
 * +--------+      +---------+      +---------+
 * 
 */
#include "pins.h"

#include "SSG.h"
SSG ssg;

#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

// channel status
// true when envelope is used instead of level control
bool chanA_env = false;
bool chanB_env = false;
bool chanC_env = false;

#define POLYPHONIC


#ifndef POLYPHONIC
// monophonic, new note replace old note

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  // Note on : set frequency, set volume (velocity)
  switch(channel)
  {
    case 1:
      ssg.set_chanA_frequency(pitch);
      //ssg.set_chanA_mixer(true, false);
      if(!chanA_env)
        // 0->127 -> 0->15
        ssg.set_chanA_level(round(velocity/8));
    break;
    case 2:
      ssg.set_chanB_frequency(pitch);
      //ssg.set_chanB_mixer(true, false);
      if(!chanB_env)
        ssg.set_chanB_level(round(velocity/8));
    break;
    case 3:
      ssg.set_chanC_frequency(pitch);
      //ssg.set_chanC_mixer(true, false);
      if(!chanC_env)
        ssg.set_chanC_level(round(velocity/8));
    break;
    default:
    break;
  }
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  // Note off = set volume 0
  switch(channel)
  {
    case 1:
      //ssg.set_chanA_mixer(false, false);
      ssg.set_chanA_level(0);
    break;
    case 2:
      //ssg.set_chanB_mixer(false, false);
      ssg.set_chanB_level(0);
    break;
    case 3:
      //ssg.set_chanC_mixer(false, false);
      ssg.set_chanC_level(0);
    break;
    default:
    break;
  }
}

#else

// polyphonic, up to 3 simultaneous notes, new note drop if already 3 being played

byte Afree = 0;
byte Bfree = 0;
byte Cfree = 0;

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  // Note on : set frequency, set volume (velocity)
  switch(channel)
  {
    case 1:
      if(Afree == 0)
      {
        Afree = pitch;
        ssg.set_chanA_frequency(pitch);
        if(!chanA_env)
          ssg.set_chanA_level(round(velocity/8));
      }
      else if(Bfree ==0)
      {
        Bfree = pitch;
        ssg.set_chanB_frequency(pitch);
        if(!chanB_env)
          ssg.set_chanB_level(round(velocity/8));
      }
      else if(Cfree ==0)
      {
        Cfree = pitch;
        ssg.set_chanC_frequency(pitch);
        if(!chanC_env)
          ssg.set_chanC_level(round(velocity/8));
      }
    break;
    default:
    break;
  }
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  // Note off = set volume 0
  switch(channel)
  {
    case 1:
      if(pitch == Afree)
      {
        ssg.set_chanA_level(0);
        Afree = 0;
      }
      if(pitch == Bfree)
      {
        ssg.set_chanB_level(0);
        Bfree = 0;
      }
      if(pitch == Cfree)
      {
        ssg.set_chanC_level(0);
        Cfree = 0;
      }
    break;
    default:
    break;
  }
}
#endif

void setup()
{
  // hardware envelope input
  pinMode(ENV_CONT, INPUT);
  pinMode(ENV_ATT, INPUT);
  pinMode(ENV_ALT, INPUT);
  pinMode(ENV_HOLD, INPUT);
  pinMode(ENV_FREQ, INPUT);
  // hardware setup input (using parallel to serial shifting-in CD4021)
  pinMode(INIT_SETUP_DATA, INPUT);
  pinMode(INIT_SETUP_CLOCK, OUTPUT);
  pinMode(INIT_SETUP_LATCH, OUTPUT);
  digitalWrite(INIT_SETUP_CLOCK, LOW);
  digitalWrite(INIT_SETUP_LATCH, LOW);
  /*
   * loop to get setup (operating mode, channel selection, ...)
   */
  
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI); // Listen to all incoming messages
}

void loop()
{
  MIDI.read();

  // envelope selection
  /*
  digitalRead(ENV_CONT);
  digitalRead(ENV_ATT);
  digitalRead(ENV_ALT);
  digitalRead(ENV_HOLD);
  */
  // envelope frequency
  /* 0->1023 -> 0->65535
   * f(x) = x/64
   * or
   * f(x) = (x*x)/16
   * or
   * f(x) = 65535-(x*x)/16
   */
  /*
  analogRead(ENV_FREQ);
  */
  

  /*
  unsigned int i;
  
  // 1 - Test Music

  ssg.set_chan_level(0b1111, chanA | chanB | chanC);
  ssg.set_chan_mixer_mute(chanA | chanB | chanC);
  ssg.set_chan_mixer(true, false, chanA);
  
  for(i = 1; i < 6; ++i)
  {
    ssg.set_chanA_frequency(C, i);
    delay_blink(200);
    ssg.set_chanA_frequency(D, i);
    delay_blink(200);
    ssg.set_chanA_frequency(E, i);
    delay_blink(200);
    ssg.set_chanA_frequency(F, i);
    delay_blink(200);
    ssg.set_chanA_frequency(G, i);
    delay_blink(200);
    ssg.set_chanA_frequency(A, i);
    delay_blink(200);
    ssg.set_chanA_frequency(B, i);
    delay_blink(200);
  }
  ssg.set_chan_mixer(true, false, chanA | chanB | chanC);

  ssg.set_chanA_frequency(C, 3);
  ssg.set_chanB_frequency(E, 4);
  ssg.set_chanC_frequency(G, 5);
  delay_blink(1000);
  
  ssg.set_chan_frequency(C, 4, chanA | chanB | chanC);
  delay_blink(1000);
  
  ssg.set_chanA_frequency(C, 4);
  ssg.set_chanB_frequency(E, 5);
  ssg.set_chanC_frequency(G, 6);
  delay_blink(1000);


  // 2 - Noise and Mixer
  ssg.set_noise_frequency(0b01111);
  delay_blink(1000);
  
  ssg.set_chanA_mixer(false, true);
  delay_blink(1000);
  ssg.set_chanB_mixer(false, true);
  delay_blink(1000);
  ssg.set_chanC_mixer(false, true);
  delay_blink(1000);
  
  ssg.set_noise_frequency(0b11111);
  delay_blink(1000);

  ssg.set_chan_mixer(false, false, chanA | chanB | chanC);
  delay_blink(1000);
  ssg.set_chanA_mixer(true, false);
  ssg.set_chanB_mixer(false, false);
  ssg.set_chanC_mixer(false, false);
  delay_blink(1000);


  // 3 - Test Level
  
  ssg.set_chanA_level(0b1111);
  ssg.set_chanA_frequency(A, 4);
  delay_blink(1000);

  for(i = 0; i < 0b1111; ++i)
  {
    ssg.set_chanA_level(i);
    delay_blink(200);
  }
  
  
  // 4 - Test Envelope
  
  ssg.set_envelope_shape(true, false, true, false);
  ssg.set_chanA_use_envelope();

  ssg.set_envelope_frequency(32);
  delay_blink(1000);
  
  ssg.set_envelope_frequency(64);
  delay_blink(1000);
  
  ssg.set_envelope_frequency(128);
  delay_blink(1000);
  
  ssg.set_envelope_frequency(256);
  delay_blink(1000);
  
  ssg.set_envelope_frequency(512);
  delay_blink(1000);
  
  ssg.set_envelope_frequency(1024);
  delay_blink(1000);
  
  ssg.set_envelope_frequency(2048);
  delay_blink(1000);
  

  ssg.set_envelope_shape(false, false, false, false);
  ssg.set_chanA_use_envelope();

  ssg.set_envelope_frequency(512);
  delay_blink(1000);
  
  ssg.set_envelope_shape(false, true, false, false);
  ssg.set_chanA_use_envelope();

  ssg.set_envelope_frequency(512);
  delay_blink(1000);


  ssg.set_envelope_shape(true, false, false, false);
  ssg.set_chanA_use_envelope();

  ssg.set_envelope_frequency(32);
  delay_blink(1000);
  
  ssg.set_envelope_frequency(64);
  delay_blink(1000);
  
  ssg.set_envelope_frequency(128);
  delay_blink(1000);
  
  ssg.set_envelope_frequency(256);
  delay_blink(1000);
  
  ssg.set_envelope_frequency(512);
  delay_blink(1000);
  
  ssg.set_envelope_frequency(1024);
  delay_blink(1000);
  
  ssg.set_envelope_frequency(2048);
  delay_blink(1000);
  */
}
