import numpy as np
import serial

class ArduinoStripe:

     def __init__(self,  serial_port_name, serial_port_speed = 230400):
         self.serial_port_name = serial_port_name
         self.serial_port_speed = serial_port_speed
         self.serial_port = serial.Serial(self.serial_port_name, self.serial_port_speed)

     def update(self, pixels):
        # we use 255 as special control character for the arduino to determine the start of the data sequence
        # so clip the color values between 0 and 254 and convert it to binary data
        binaryPixelData = np.clip(pixels, 0, 254).astype(np.uint8).tobytes(order='C')
        if len(pixels) >= 0xFFFF:
            print("Unsupported config: Pixel number is too high")
            return
        if len(pixels)%255==0:
            # since 255 is a special value, we could not allow it inside the length data
            # -> add a tunred off dummy pixel at the end. Arduino code will ignore it, if the strip is too short
            pixel_count = len(pixels)+1
            binaryPixelData = binaryPixelData + b'\x00\x00\x00'
        else:
            pixel_count = len(pixels)
        binaryStripeLengthData = pixel_count.to_bytes(2, byteorder="big", signed=False)
        # use 255 (=0xff) to mark the start of the sequence
        # Todo: check if port is connected and do automatic reconnect if 
        try:
            self._write_data(b'\xff' + binaryStripeLengthData + binaryPixelData)
            return True
        except IOError:
            # try to reopen the port
            try:
                self.serial_port = serial.Serial(self.serial_port_name, self.serial_port_speed)
                self._write_data(b'\xff' + binaryStripeLengthData + binaryPixelData)
                return True
            except IOError as e:
                print("Could not write to serial port: " + str(e))
                return False
        
     def _write_data(self, data):
        self.serial_port.write(data)
        self.serial_port.flush()
