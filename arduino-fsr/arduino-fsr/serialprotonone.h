#ifndef SERIALPROTONONE_H
#define SERIALPROTONONE_H

#include "serialproto.h"

/// No-Op serial interface - Does nothing
/// But it needs to exist. If serial isn't started the USB joystick part doesn't work
class SerialProtoNone final : public SerialProto
{
  public:
    SerialProtoNone(int baud);
    virtual ~SerialProtoNone();

    /// First-time setup
    void initialise() override;

    /// Periodic update - Proto needs to read from serial
    /// @param t Seconds since startup
    /// @param dT Seconds since last update
    /// @param state Sketch state - Access to sensors
    void update(float t, float dT, State& state) override;
};

#endif
