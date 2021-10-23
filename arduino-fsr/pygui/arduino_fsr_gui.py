
import dearpygui
import time
from serialprotogf import SerialProtoGF

def main():
  serial = SerialProtoGF('/dev/ttyACM0')
  while True:
    panels = serial.command_get_values_raw_min()
    print(panels)
    time.sleep(0.1)

if __name__ == '__main__':
  main()
