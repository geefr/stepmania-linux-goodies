/**
 * Copyright (c) Gareth Francis (gfrancis.dev@gmail.com) 2018 All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation 
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Code running on Geefr/ITGaz's pad for handling FSR input
 * Inspired by Mckyla's FSR sketch: https://github.com/hippaheikki/mckyla-fsr
 */

// Enum and state definitions
#include "common.h"
// Serial interfaces/protocols
// - Debug info/helpers
// - Compatibility with cabinet-side software
#include "serialproto.h"
#include "serialprotodebug.h"
// #include "serial/mckyla.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sketch configuration - Uncomment these to enable software features
// #define ENABLE_EEPROM // Save sensitivity data to board's EEPROM, persist settings without connecting to cabinet

// Global state setup - configure pin -> pad mappings and sketch features here
void initialiseState(State& s)
{
  // The serial protocol for talking to the cab, or debug. Optional.
  // s.serial.reset(new SerialProtoDebug(115200));
  
  // Configure the mapping between IO pin and panel
  // s.sensors[analog pin index] = Panel::XXX
  s.sensors[0].panel = Panel::P1Left;
  s.sensors[1].panel = Panel::P1Right;
  s.sensors[2].panel = Panel::P1Up;
  s.sensors[3].panel = Panel::P1Up;
  // s.sensors[4].panel = Panel::P1Right; // broken :-[
  s.sensors[5].panel = Panel::P1Down;
  s.sensors[6].panel = Panel::P1Left;
  s.sensors[7].panel = Panel::P1Down;

  // Other state initialisation - Don't change this
  if( s.serial ) s.serial->initialise();
  for( auto& p : s.sensors ) s.panels[p.second.panel] = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#ifdef ENABLE_EEPROM
//# include <EEPROM.h>
//#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FSR algorithm - Handles FSR state, applies smoothing, applies threshold detection logic
// TODO - Some classes here - Dumb direct threshold, first order filter, that with schmitt trigger and such
void fsrAlgorithm(State& s)
{
  for( auto& p : s.sensors )
  {
    auto& i = p.second;
    // Placeholder - no filtering
    i.filteredValue = i.rawValue;
    // Placeholder - Direct trigger on sensor min + threshold
    i.triggered = (i.filteredValue - i.rawValueMin) > i.triggerValue;
  }
}

// Panel algorithm - Combines per-FSR state into per-panel state
// TODO - Simple OR at first, seems sensible, is anything more complicated needed?
void panelAlgorithm(State& s)
{
  // Collate sensor info to panel info
  std::map<Panel, bool> panels;
  for( auto& p : s.sensors ) panels[p.second.panel] = false;

  for( auto& p : s.sensors )
  {
    if( p.second.triggered )
    {
      panels[p.second.panel] = true;
    }
  }

  // Send all joystick updates in one batch
  Joystick.useManualSend(true);

  // Detect change in panel state
  for( auto& p : s.panels )
  {
    if( !p.second && panels[p.first] )
    {
      // Pressed
      Joystick.button(static_cast<int>(p.first), true);
      p.second = true;
    }
    else if( p.second && !panels[p.first] )
    {
      // Released
      Joystick.button(static_cast<int>(p.first), false);
      p.second = false;
    }
  }

  Joystick.send_now();
  // Reset to default in case other chunks of code use Joystick
  Joystick.useManualSend(false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main sketch functions - Binds basic sequence to the various program elements
void readInputPins(State& s)
{
  for( auto& p : s.sensors )
  {
    p.second.rawValue = analogRead(p.first);
    p.second.rawValueMin = std::min(p.second.rawValue, p.second.rawValueMin);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
State gState;
float secondsSinceStartup = 0.f;

void setup(void) {
  initialiseState(gState);
  // TODO
  // - eeprom initialisation
  // - eeprom save support
  // - a whole load of stuff
}

void loop(void) {
  // Update timing data
  auto t = static_cast<float>(millis()) / 1000.f;
  auto dT = t - secondsSinceStartup;
  secondsSinceStartup = t;
  
  // Read data from sensors
  readInputPins(gState);

  // Apply filters to fsr signals, determine if each sensor is triggered
  fsrAlgorithm(gState);

  // Combine the sensors, work out if panels are on/off, send joystick updates
  panelAlgorithm(gState);
  
  // Serial update - Handle any commands from the cabinet
  gState.serial->update(t, dT, gState);
}