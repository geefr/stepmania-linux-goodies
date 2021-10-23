
import dearpygui.dearpygui as dpg
import time
import sys
from serialprotogf import PANEL_P1DOWN, PANEL_P1LEFT, PANEL_P1RIGHT, PANEL_P1UP, SerialProtoGF

def get_panel_mapping(serial: SerialProtoGF):
  panels = serial.command_get_panels()
  mapping = dict()
  for i, p in enumerate(panels):
    if p not in mapping.keys():
      mapping[p] = list()
    mapping[p].append(i)
  return mapping

def main(port):
  # Should be configurable or whatever, for the other pad protocols
  serial = SerialProtoGF('/dev/ttyACM0')

  # Fetch the pad config / base settings
  # [panel no] = list[sensor indices]
  panel_mapping = get_panel_mapping(serial)
  trigger_values = serial.command_get_trigger_values()
  schmitt_values = serial.command_get_trigger_schmitt_values()

  dpg.create_context()
  dpg.create_viewport(
    title="ITGaz's Awesome Pad GUI",
    width=800, height=600,
    resizable=False, decorated=True)
  dpg.setup_dearpygui()

  vals_raw = [0 for x in trigger_values]
  

  with dpg.window(label="", width=800, height=600, no_resize=True, no_title_bar=True, no_move=True):
    dpg.add_text("LEFT:")
    with dpg.group(horizontal=True):
      for s in panel_mapping[PANEL_P1LEFT]:
        dpg.add_slider_float(no_input=True, vertical=False, tag=f"sensorbar_{s}", width=300)
        dpg.add_checkbox(tag=f"sensortriggered_{s}")
    
    dpg.add_text("DOWN:")
    with dpg.group(horizontal=True):
      for s in panel_mapping[PANEL_P1DOWN]:
        dpg.add_slider_float(no_input=True, vertical=False, tag=f"sensorbar_{s}", width=300)
        dpg.add_checkbox(tag=f"sensortriggered_{s}")

    dpg.add_text("UP:")
    with dpg.group(horizontal=True):
      for s in panel_mapping[PANEL_P1UP]:
        dpg.add_slider_float(no_input=True, vertical=False, tag=f"sensorbar_{s}", width=300)
        dpg.add_checkbox(tag=f"sensortriggered_{s}")

    dpg.add_text("RIGHT:")
    with dpg.group(horizontal=True):
      for s in panel_mapping[PANEL_P1RIGHT]:
        dpg.add_slider_float(no_input=True, vertical=False, tag=f"sensorbar_{s}", width=300)
        dpg.add_checkbox(tag=f"sensortriggered_{s}")

  dpg.show_viewport()
  # dpg.start_dearpygui()

  while dpg.is_dearpygui_running():
    vals_raw = serial.command_get_values_raw()
    vals_raw_min = serial.command_get_values_raw_min()
    vals_filtered = serial.command_get_values_filtered()
    triggered = serial.command_get_triggered()

    for s,v in enumerate(vals_filtered):
      slider_tag = f"sensorbar_{s}"
      triggered_tag = f"sensortriggered_{s}"

      # slider_config = dpg.get_item_configuration(tag)
      slider_min = 0
      slider_max = 1024
      # slider_min = min(slider_config['min_value'], vals_raw_min[s])
      # slider_max = max(slider_config['max_value'], vals_raw[s])
      dpg.configure_item(slider_tag, min_value=slider_min, max_value=slider_max)
      dpg.set_value(slider_tag, v)
      dpg.set_value(triggered_tag, triggered[s])

    # render_ui(panel_mapping, trigger_values, schmitt_values, vals_raw, vals_raw_min, vals_filtered, triggered)
    dpg.render_dearpygui_frame()
    time.sleep(0.05)

  dpg.destroy_context()

if __name__ == '__main__':
  port = '/dev/ttyACM0'
  if len(sys.argv) > 1:
    port = sys.argv[1]
  main(port)
