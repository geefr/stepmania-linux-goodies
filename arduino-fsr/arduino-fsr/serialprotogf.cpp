
#include "serialprotogf.h"

#include <Arduino.h>
#include <sstream>
#include <string>

#include "common.h"

SerialProtoGF::SerialProtoGF(int baud)
  : SerialProto(baud)
{}

SerialProtoGF::~SerialProtoGF()
{}

void SerialProtoGF::initialise() {
  Serial.begin(mBaud);
}

namespace {
  template<typename T>
  std::string ToString(const T& v)
  {
      std::ostringstream ss;
      ss << v;
      return ss.str();
  }
  
  template<typename T>
  T FromString(const std::string& str)
  {
      std::istringstream ss(str);
      T ret;
      ss >> ret;
      return ret;
  }

  void respondGetPanels(State& state) {
    for( auto& p : state.sensors )
    {
      Serial.print(p.second.panel);
      Serial.print(' ');
    }
    Serial.print('\n');
  }
  
  void respondGetValuesRaw(State& state) {
    for( auto& p : state.sensors )
    {
      Serial.print(p.second.rawValue);
      Serial.print(' ');
    }
    Serial.print('\n');
  }
  
  void respondGetValuesRawMin(State& state) {
    for( auto& p : state.sensors )
    {
      Serial.print(p.second.rawValueMin);
      Serial.print(' ');
    }
    Serial.print('\n');
  }
     
  void respondGetValuesFiltered(State& state) {
    for( auto& p : state.sensors )
    {
      Serial.print(p.second.filteredValue);
      Serial.print(' ');
    }
    Serial.print('\n');
  }

  void respondGetTriggered(State& state) {
    for( auto& p : state.sensors )
    {
      Serial.print(p.second.triggered ? '1' : '0');
      Serial.print(' ');
    }
    Serial.print('\n');
  }

  void respondGetTriggerValues(State& state) {
    for( auto& p : state.sensors )
    {
      Serial.print(p.second.triggerValue);
      Serial.print(' ');
    }
    Serial.print('\n');
  }

  void respondGetTriggerSchmittValues(State& state) {
    for( auto& p : state.sensors )
    {
      Serial.print(p.second.triggerSchmittValue);
      Serial.print(' ');
    }
    Serial.print('\n');
  }

  void setTriggerValues(State& state) {
    String vals = Serial.readStringUntil('\n');
    if( vals.length() == 0 ) return; // TODO: Error beep/led?
    std::stringstream ss(vals.c_str());
 
    std::string s;
    auto currentSensor = 0u;
    while (std::getline(ss, s, ' ')) { 
        state.sensors[currentSensor++].triggerValue = FromString<int>(s);
    } 
  }

  void setTriggerSchmittValues(State& state) {
    String vals = Serial.readStringUntil('\n');
    if( vals.length() == 0 ) return; // TODO: Error beep/led?
    std::stringstream ss(vals.c_str());
 
    std::string s;
    auto currentSensor = 0u;
    while (std::getline(ss, s, ' ')) { 
        state.sensors[currentSensor++].triggerSchmittValue = FromString<int>(s);
    } 
  }
}

void SerialProtoGF::update(float t, float dT, State& state) {
  if( Serial.available() == 0 )
  {
    return;
  }

  Command command = static_cast<Command>(Serial.read());
  switch( command ) {
    case Command::GetPanels:
      respondGetPanels(state);
      break;
    case Command::GetValuesRaw:
      respondGetValuesRaw(state);
      break;
    case Command::GetValuesRawMin:
      respondGetValuesRawMin(state);
      break;
    case Command::GetValuesFiltered:
      respondGetValuesFiltered(state);
      break;
    case Command::GetTriggered:
      respondGetTriggered(state);
      break;
    case Command::GetTriggerValues:
      respondGetTriggerValues(state);
      break;
    case Command::GetTriggerSchmittValues:
      respondGetTriggerSchmittValues(state);
      break;
    case Command::SetTriggerValues:
      setTriggerValues(state);
      break;
    case Command::SetTriggerSchmittValues:
      setTriggerSchmittValues(state);
      break;
    default:
      break;
  }
}

/*
// Raw values
  for( auto& p : state.sensors )
  {
    Serial.print(p.second.filteredValue);
    Serial.print(' ');
  }

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
} */
