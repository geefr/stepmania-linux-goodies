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
#include "serialprotonone.h"
#include "serialprotogf.h"
// #include "serial/mckyla.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sketch configuration - Uncomment these to enable software features
// #define ENABLE_DEBUG // Various debug capability, may not work on your board
// (TODO) #define ENABLE_EEPROM // Save sensitivity data to board's EEPROM, persist settings without connecting to cabinet
// TODO: Configure light-fading for teensy (<1ms update rate), maybe add in the idle animation system
#define ENABLE_LIGHTS // Enable light output


// Global state setup - configure pin -> pad mappings and sketch features here
void initialiseState(State& s)
{
  //////// SETUP STEPS ////////
  // Firstly, Click Tools -> Board -> Teensyduino -> Teensy 4.0
  // (Or your equivalent, this sketch shouldn't care much)
  // Then Click Tools -> USB Type -> "Serial + Keyboard + Mouse + Joystick"
  // (Joystick being the important bit - Serial is a nice to have if your board can do it as well)
  
  // The serial protocol for talking to the cab, debug, or none.
  // Debug is useful for setup but may cause polling issues
  // A serial protocol must exist - If you don't need anything choose none here
  //s.serial.reset(new SerialProtoDebug(115200));
  //s.serial.reset(new SerialProtoNone(115200));
  s.serial.reset(new SerialProtoGF(115200));

  // Configure the mapping between IO pin and panel
  // This is ITGaz's setup, yours will be different
  // Using SerialInterfaceDebug and the arduino serial monitor can help here,
  // or maybe there'll be a handy dandy UI to help set this up if you bug me about it <3
  // s.sensors[analog pin index] = Panel::XXX
  // On Teensy 4 analog index 0-7 correspond to pins 14-21

  s.sensors[0].panel = Panel::P1Left;
  s.sensors[1].panel = Panel::P1Left;

  s.sensors[2].panel = Panel::P1Down;
  s.sensors[3].panel = Panel::P1Down;
  
  s.sensors[4].panel = Panel::P1Up;
  s.sensors[5].panel = Panel::P1Up;

  s.sensors[6].panel = Panel::P1Right;
  s.sensors[7].panel = Panel::P1Right;

  // To hard-code sensitivity set values here
  // Defaults are set in common.h SensorState
  s.sensors[4].triggerValue = 80;
  s.sensors[5].triggerValue = 80;

  // Configure mappings for light pins
  // Output pins should be PWM - This firmware uses analogWrite for some nice effects
  // Lighting constants, or swithcing to digitalWrite can be changed in light.h/cpp
  // s.lights[Panel] = Light(output_pin)
  s.lights[Panel::P1Right] = Light(12);
  s.lights[Panel::P1Up] = Light(11);
  s.lights[Panel::P1Down] = Light(10);
  s.lights[Panel::P1Left] = Light(9);

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

  // If you need to, set pinMode here
  // I'm using the default mode of all these, so no need to change it
  
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
    // Placeholder - no filtering, just offset to account for panel weight, hoping we're still in a relatively linear range
    i.filteredValue = i.rawValue - i.rawValueMin;

    // Placeholder - Direct trigger on sensor min + threshold + schmitt
    // TODO: This is reliable enough, but sensor acceleration would auto-adjust
    // TODO: Encapsulate out of main loop, objectify all the things! :O
    // i.triggered = (i.filteredValue - i.rawValueMin) > (i.triggerValue);

    if( !i.triggered && i.filteredValue > (i.triggerValue + i.triggerSchmittValue))
    {
      i.triggered = true;
    }
    else if( i.triggered && i.filteredValue < (i.triggerValue - i.triggerSchmittValue) )
    {
      i.triggered = false;
    }

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

void readInputPins(State& s)
{
  static bool firstTime = true;

  for( auto& p : s.sensors )
  {
    p.second.rawValue = analogRead(p.first);

    if( firstTime )
      p.second.rawValueMin = std::min(p.second.rawValue, p.second.rawValueMin);
  }
  if( firstTime ) firstTime = false;
}

void lightUpdate(std::map<Panel, bool>& oldPanels, State& newState, float deltaSeconds)
{
  for( auto& panel : newState.panels )
  {
    auto light = newState.lights.find(panel.first);
    if( light == newState.lights.end() )
    {
      continue;
    }
    
    if( panel.second && !oldPanels[panel.first] )
    {
      // Panel pressed
      light->second.target(0xff);
      light->second.set(0xff);
    }
    else if( !panel.second && oldPanels[panel.first] )
    {
      // Panel released
      light->second.target(0x00);
      light->second.set(0x00);
    }
  }

  for( auto& light : newState.lights )
  {
    light.second.tick(deltaSeconds);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
State gState;
std::map<Panel, bool> gOldPanels;
float secondsSinceStartup = 0.f;

void setup(void) {
  initialiseState(gState);

  for( auto& l : gState.lights ) l.second.target(0x00);
  // TODO
  // - eeprom initialisation
  // - eeprom save support
  // - a whole load of stuff
}

void loop(void) {
  gOldPanels = gState.panels;

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

  // Light update
  lightUpdate(gOldPanels, gState, dT);
  
  // Serial update - Handle any commands from the cabinet
  static int i = 0;
  ++i;
  if( i % 10 == 0 ) {
    gState.serial->update(t, dT, gState);
    i = 0;
  }
  
#ifdef ENABLE_DEBUG
  auto anyOn = false;
  for(auto& p : gState.panels) if(p.second) anyOn = true;
  digitalWrite(13, anyOn ? HIGH : LOW);
#endif
}
