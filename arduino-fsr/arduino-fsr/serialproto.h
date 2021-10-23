#ifndef SERIALPROTO_H
#define SERIALPROTO_H

class State;

/// Interface for serial handlers
class SerialProto
{
  public:
    virtual ~SerialProto();

    /// First-time setup
    virtual void initialise() = 0;

    /// Periodic update - Proto needs to read from serial
    /// @param t Seconds since startup
    /// @param dT Seconds since last update
    /// @param state Sketch state - Access to sensors
    virtual void update(float t, float dT, State& state) = 0;

  protected:
    SerialProto(int baud);
    

    int mBaud = 9600;
};

inline SerialProto::SerialProto(int baud)
  : mBaud(baud)
{}

inline SerialProto::~SerialProto() {
}

#endif
