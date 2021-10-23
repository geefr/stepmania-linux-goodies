#ifndef SERIALPROTOGF_H
#define SERIALPROTOGF_H

#include "serialproto.h"

/// Serial commands. Mostly GetXXX, return requested data, then carriage return
enum Command {
  InvalidCommand = 0x00,
  /// Return list of panels for each sensor, space delimited
  GetPanels = '1',
  /// Return sensor values, space delmited
  GetValuesRaw = '2',
  GetValuesRawMin = '3',
  GetValuesFiltered = '4',
  /// Return whether each sensor is triggered, space delmited
  GetTriggered = '5',
  GetTriggerValues = '6',
  GetTriggerSchmittValues = '7',
  /// Set trigger values for all sensors
  /// Send command byte, then values, space delimited, then a \n
  SetTriggerValues = '8',
  SetTriggerSchmittValues = '9',
};

/// GF serial interface - Serial interaction with Gaz's monitor system
class SerialProtoGF final : public SerialProto
{
  public:
    SerialProtoGF(int baud);
    virtual ~SerialProtoGF();

    /// First-time setup
    void initialise() override;

    /// Periodic update - Proto needs to read from serial
    /// @param t Seconds since startup
    /// @param dT Seconds since last update
    /// @param state Sketch state - Access to sensors
    void update(float t, float dT, State& state) override;
};

#endif
