
#include "serialprotodebug.h"

#include <Arduino.h>

#include "common.h"

SerialProtoDebug::SerialProtoDebug(int baud)
  : SerialProto(baud)
{}

SerialProtoDebug::~SerialProtoDebug()
{}

void SerialProtoDebug::initialise() {
  Serial.begin(mBaud);
}

void SerialProtoDebug::update(float t, float dT, State& state) {
// Raw values
//  for( auto& p : state.sensors )
//  {
//    Serial.print(p.second.rawValue /*/ 1023.f * 3.3f*/);
//    Serial.print(' ');
//  }

// Sensor triggers
  for( auto& p : state.sensors )
  {
    Serial.print(p.second.triggered);
    Serial.print(' ');
  }

// Final joystick buttons
  for( auto& p : state.panels )
  {
    Serial.print(p.second);
    Serial.print(' ');
  }
  Serial.print("\r\n");
}
