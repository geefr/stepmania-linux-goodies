/* 
 * BSD 2-Clause License
 *
 * Copyright (c) 2020, Gareth Francis <gfrancis.dev@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

///////////////////////////////////////////////////////////////////////////////////////////////
// Arduino reader for Stepmania's Sextet Stream light driver
///////////////////////////////////////////////////////////////////////////////////////////////

#include "pad.h"

Pad lePad;

// Buffer for data from stepmania
// https://github.com/stepmania/stepmania/blob/master/src/arch/Lights/LightsDriver_SextetStream.md
// 13 bytes + '\n'
const uint32_t dataSize = 14;
char data[dataSize];

// When there's no data from stepmania run some idle animations and such
// Wait this long before falling into the idle mode
const uint32_t idleTimeoutMS = 2500;
uint32_t lastDataFromStepmania = 0;
uint32_t lastUpdate = 0;

// Definition of a light fade
struct LightFade {
  LightPin pad;
  uint8_t startVal;
  uint8_t endVal;
  uint8_t curVal;
  uint32_t startTimeMS;
  uint32_t nextTimeMS;
  uint32_t rate;

  LightFade()
  : pad(pad1Up), startVal(0), endVal(255), curVal(0), startTimeMS(0), nextTimeMS(0), rate(0)
  {}
  
  LightFade( LightPin p, uint32_t start, uint32_t r, uint8_t a, uint8_t b )
  : pad(p), startVal(a), endVal(b), curVal(startVal), startTimeMS(start), nextTimeMS(start), rate(r)
  {}

  void tick( uint32_t now ) {
    if( now < startTimeMS ) return;
    if( done() ) return;
    if( now > nextTimeMS ) {
      if( startVal < endVal ) ++curVal;
      else --curVal;
      lePad.set( pad, curVal );
      nextTimeMS += rate;
    }
  }

  bool done() const { return curVal == endVal; }
};

struct LightFadeSequence {
  struct LightFade* fades;
  uint32_t numFades;
  LightFadeSequence() : fades(NULL), numFades(0) {}
  LightFadeSequence( struct LightFade* f, uint32_t n ) : fades(f), numFades(n) {}
};

const uint32_t numFadeSequences = 4;
LightFadeSequence fadeSequences[numFadeSequences];
uint32_t currentFadeSequence = 0;
uint32_t numFadeSequenceRepeats = 4;
uint32_t currentFadeSequenceRepeat = 0;

void pattern_runFadeSequence( struct LightFadeSequence& f ) {
  if( f.numFades == 0 ) return;
  auto start = millis();
  // Copy the sequence - Don't want to modify the original
  auto fades = new LightFade[f.numFades];
  for( auto i = 0u; i < f.numFades; ++i ) {
    fades[i] = f.fades[i];
  }
  // Run the sequence
  while( true ) {
    auto now = millis() - start;
    for( auto i = 0u; i < f.numFades; ++i ) {
      auto& fade = fades[i];
      fade.tick(now);
      if( Serial.peek() != -1 ) { delete [] fades; return; }
    }
    if( fades[f.numFades - 1].done() ) break;
  }
  // And don't forget to cleanup, will run out of memory very quickly otherwise
  delete [] fades;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(pad1Left, OUTPUT);
  pinMode(pad1Right, OUTPUT);
  pinMode(pad1Up, OUTPUT);
  pinMode(pad1Down, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);

  // Set a timeout on serial reads - Let us skip if there's no input for a while
  // This should be low to ensure the pad fading works correctly
  Serial.setTimeout(10);

  // Seed the random number generator
  randomSeed(analogRead(0));

  // Initialise a bunch of fade sequences
  // Don't add more than numFadeSequences here
  // Clockwise Roll
  auto fades = new LightFade[8];
  fades[0] = LightFade( pad1Up, 500, 1, 0, 255 );
  fades[1] = LightFade( pad1Right, 1000, 1, 0, 255 );
  fades[2] = LightFade( pad1Down, 1500, 1, 0, 255 );
  fades[3] = LightFade( pad1Left, 2000, 1, 0, 255 );
  fades[4] = LightFade( pad1Up, 2500, 1, 255, 0 );
  fades[5] = LightFade( pad1Right, 3000, 1, 255, 0 );
  fades[6] = LightFade( pad1Down, 3500, 1, 255, 0 );
  fades[7] = LightFade( pad1Left, 4000, 1, 255, 0 );
  fadeSequences[0] = LightFadeSequence(fades, 8);
  
  // Anti Clockwise Roll
  fades = new LightFade[8];
  fades[0] = LightFade( pad1Up, 500, 1, 0, 255 );
  fades[1] = LightFade( pad1Left, 1000, 1, 0, 255 );
  fades[2] = LightFade( pad1Down, 1500, 1, 0, 255 );
  fades[3] = LightFade( pad1Right, 2000, 1, 0, 255 );
  fades[4] = LightFade( pad1Up, 2500, 1, 255, 0 );
  fades[5] = LightFade( pad1Left, 3000, 1, 255, 0 );
  fades[6] = LightFade( pad1Down, 3500, 1, 255, 0 );
  fades[7] = LightFade( pad1Right, 4000, 1, 255, 0 );
  fadeSequences[1] = LightFadeSequence(fades, 8);

  // Pulse all
  fades = new LightFade[8];
  fades[0] = LightFade( pad1Up, 500, 1, 0, 255 );
  fades[1] = LightFade( pad1Left, 500, 1, 0, 255 );
  fades[2] = LightFade( pad1Down, 500, 1, 0, 255 );
  fades[3] = LightFade( pad1Right, 500, 1, 0, 255 );
  fades[4] = LightFade( pad1Up, 2000, 1, 255, 0 );
  fades[5] = LightFade( pad1Left, 2000, 1, 255, 0 );
  fades[6] = LightFade( pad1Down, 2000, 1, 255, 0 );
  fades[7] = LightFade( pad1Right, 2000, 1, 255, 0 );
  fadeSequences[2] = LightFadeSequence(fades, 8);

  // Pulse alternate
  fades = new LightFade[8];
  fades[0] = LightFade( pad1Up, 1000, 1, 0, 255 );
  fades[1] = LightFade( pad1Down, 1000, 1, 0, 255 );
  
  fades[2] = LightFade( pad1Up, 2000, 1, 255, 0 );
  fades[3] = LightFade( pad1Down, 2000, 1, 255, 0 );
  fades[4] = LightFade( pad1Left, 2000, 1, 0, 255 );
  fades[5] = LightFade( pad1Right, 2000, 1, 0, 255 );
  
  fades[6] = LightFade( pad1Left, 3000, 1, 255, 0 );
  fades[7] = LightFade( pad1Right, 3000, 1, 255, 0 );
  fadeSequences[3] = LightFadeSequence(fades, 8);


  numFadeSequenceRepeats = random(0, 4);
  currentFadeSequence = random(0, numFadeSequences);
}

// The loop function runs over and over again forever
void loop() {
  auto now = millis();

  // Update the pad (fade the lights out)
  lePad.tick(now - lastUpdate);
  lastUpdate = now;
  
  auto bytesRead = Serial.readBytes(data, dataSize);
  if( bytesRead != 0 )
  {
    // Got data from stepmania, do as instructed
    // Performance matters here, less so below as long
    // as the loop returns back to reading sm input after
    lastDataFromStepmania = millis();
   
    // Set the pad lights based on the data
    lePad.setFromSextet( data );
    return;
  }

  // Don't start animations for a while - Just in case sm output
  // is being slow.
  if( now - lastDataFromStepmania < idleTimeoutMS ) {
    return;
  }
  
  // Oh no! didn't get anything, either Stepmania isn't running
  // or it's not sending data for some reason. Do something cool with the pad lights.  
  pattern_runFadeSequence( fadeSequences[currentFadeSequence] );

  if( ++currentFadeSequenceRepeat >= numFadeSequenceRepeats ) {
    currentFadeSequenceRepeat = 0;
    numFadeSequenceRepeats = random(0, 4);
   //  if( ++currentFadeSequence >= numFadeSequences ) {
      currentFadeSequence = random(0, numFadeSequences);
    // }
  }  
}
