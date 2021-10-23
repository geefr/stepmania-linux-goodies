
import serial
import time

# Interface definition, see serialprotogf.h in arduino source
COMMAND_INVALID = 0
COMMAND_GET_PANELS = '1'
COMMAND_GET_VALUES_RAW = '2'
COMMAND_GET_VALUES_RAW_MIN = '3'
COMMAND_GET_VALUES_FILTERED = '4'
COMMAND_GET_TRIGGERED = '5'
COMMAND_GET_TRIGGER_VALUES = '6'
COMMAND_GET_TRIGGER_SCHMITT_VALUES = '7'
COMMAND_SET_TRIGGER_VALUES = '8'
COMMAND_SET_TRIGGER_SCHMITT_VALUES = '9'

# Panel definitions, see common.h
PANEL_INVALID = 0
PANEL_P1UP = 1
PANEL_P1RIGHT = 2
PANEL_P1DOWN = 3
PANEL_P1LEFT = 4
PANEL_P2UP = 5
PANEL_P2RIGHT = 6
PANEL_P2DOWN = 7
PANEL_P2LEFT = 8  

class SerialProtoGF:
  def __init__(self, port):
    self._port = port
    self._serial = serial.Serial(
      port=self._port,
      baudrate=111200,
    )

  def command_get_panels(self):
    """Read panel mappings for each sensor
       Returned values should be one of PANEL_ constants
    """
    self._serial.write(bytes(COMMAND_GET_PANELS, 'ascii'))
    response = self._serial.readline().decode('ascii')
    panels = []
    for c in response.split(' '):
      try:
        panels.append(int(c))
      except ValueError:
        pass
    return panels

  def command_get_values_raw(self):
    self._serial.write(bytes(COMMAND_GET_VALUES_RAW, 'ascii'))
    response = self._serial.readline().decode('ascii')
    result = []
    for c in response.split(' '):
      try:
        result.append(int(c))
      except ValueError:
        pass
    return result

  def command_get_values_raw_min(self):
    self._serial.write(bytes(COMMAND_GET_VALUES_RAW_MIN, 'ascii'))
    response = self._serial.readline().decode('ascii')
    result = []
    for c in response.split(' '):
      try:
        result.append(int(c))
      except ValueError:
        pass
    return result

  def command_get_values_filtered(self):
    self._serial.write(bytes(COMMAND_GET_VALUES_FILTERED, 'ascii'))
    response = self._serial.readline().decode('ascii')
    result = []
    for c in response.split(' '):
      try:
        result.append(int(c))
      except ValueError:
        pass
    return result

  def command_get_triggered(self):
    self._serial.write(bytes(COMMAND_GET_TRIGGERED, 'ascii'))
    response = self._serial.readline().decode('ascii')
    result = []
    for c in response.split(' '):
      try:
        result.append(bool(int(c)))
      except ValueError: 
        pass
    return result

  def command_get_trigger_values(self):
    self._serial.write(bytes(COMMAND_GET_TRIGGER_VALUES, 'ascii'))
    response = self._serial.readline().decode('ascii')
    result = []
    for c in response.split(' '):
      try:
        result.append(int(c))
      except ValueError:
        pass
    return result

  def command_get_trigger_schmitt_values(self):
    self._serial.write(bytes(COMMAND_GET_TRIGGER_SCHMITT_VALUES, 'ascii'))
    response = self._serial.readline().decode('ascii')
    result = []
    for c in response.split(' '):
      try:
        result.append(int(c))
      except ValueError:
        pass
    return result
  def command_set_trigger_values(self, trigger_values):
    self._serial.write(bytes(COMMAND_SET_TRIGGER_VALUES, 'ascii'))
    msg = str()
    for v in trigger_values:
      msg += f"{v} "
    msg += "\n"
    self._serial.write(bytes(msg, 'ascii'))

  def command_set_trigger_schmitt_values(self, trigger_schmitt_values):
    self._serial.write(bytes(COMMAND_SET_TRIGGER_SCHMITT_VALUES, 'ascii'))
    msg = str()
    for v in trigger_schmitt_values:
      msg += f"{v} "
    msg += "\n"
    self._serial.write(bytes(msg, 'ascii'))
