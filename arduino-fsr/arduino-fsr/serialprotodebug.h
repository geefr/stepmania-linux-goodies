#ifndef SERIALPROTODEBUG_H
#define SERIALPROTODEBUG_H

#include "serialproto.h"

/// Debug serial interface - Dumps FSR information to serial console
class SerialProtoDebug final : public SerialProto
{
  public:
    SerialProtoDebug(int baud);
    virtual ~SerialProtoDebug();

    /// First-time setup
    void initialise() override;

    /// Periodic update - Proto needs to read from serial
    /// @param t Seconds since startup
    /// @param dT Seconds since last update
    /// @param state Sketch state - Access to sensors
    void update(float t, float dT, State& state) override;
};

#endif
