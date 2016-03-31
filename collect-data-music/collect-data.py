# collect-data.py
# Python script for data collection in SIXT33N Music version
# 
# EE16B Spring 2016
# Emily Naviasky & Nathaniel Mailoa
#

import serial
import sys
import os
import re

samples = []

# Serial functions
def serial_ports():
    """Lists serial ports

    Raises:
    EnvironmentError:
        On unsupported or unknown platforms
    Returns:
        A list of available serial ports
    """
    if sys.platform.startswith('win'):
        ports = ['COM' + str(i + 1) for i in range(256)]

    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this is to exclude your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')

    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')

    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

def open_port(port, baud):
    """Open a serial port.

    Args:
    port (string): port to open, on Unix typically begin with '/dev/tty', on
        or 'COM#' on Windows.
    baud (int, optional): baud rate for serial communication

    Raises:
    SerialException: raised for errors from serial communication

    Returns:
       A pySerial object to communicate with the serial port.
    """
    ser = serial.Serial()
    try:
        ser = serial.Serial(port, baud, timeout=10)
        logger.info("Opened serial connection on port %s"%port)
        return ser
    except serial.SerialException:
        raise

def save_txt(buff, txtfile):
  text_file = open(txtfile, 'a')
  text_file.write(','.join([str(x) for x in buff]))
  text_file.write('\n')
  text_file.close()

def run(filename):
  samples = []
  print("EE16B Front End Lab")

  ports = serial_ports()
  if ports:
    print("Available serial ports:")
    for (i,p) in enumerate(ports):
      print("%d) %s"%(i+1,p))
  else:
    print("No ports available. Check serial connection and try again.")
    print("Exiting...")
    return

  portNo = input("Select the port to use: ")
  ser = serial.Serial(ports[int(portNo)-1])
  ser.baudrate=38400
  ser.timeout=20
  print('foo: %s' % ser.readline())
  
  print('Collecting data... Please wait.')

  

  while(True):
    samples = []
    while (not re.match("Start", ser.readline().decode('utf-8'))):
      pass
    for i in range(2*128):
      samples.append((float)(ser.readline().decode().rstrip('\n')))
    save_txt(samples, filename)
    print('Done writing to ' + filename)

  ser.close()


if __name__ == "__main__":
    if(len(sys.argv) != 2):
        print("Usage: python collect-data.py <filename.csv>")
        exit()
    run(sys.argv[1])
