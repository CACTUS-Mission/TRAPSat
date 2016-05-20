# For Arduino Data:
import serial, signal, sys
################################################################################################
ARDUINO_SERIAL_PORT = '/dev/ttyACM0' 
# Serial port should be set to the port found in the Arduino Program when programming the board

parallel_data_file = "parallel_data_file.raw"
# File to write data to
#################################################################################################



file = open(parallel_data_file, 'w')
ard_ser = serial.Serial(ARDUINO_SERIAL_PORT, 19200) 



# SIGINT Handler -- ensures port and file are closed properly
def signal_handler(signal, frame):
	print("Exiting Arduino Serial Read!")
	ard_ser.close()
	file.close()
	sys.exit(0)

# Register signal handler
signal.signal(signal.SIGINT, signal_handler)



# Read Data From Arduino Serial Port
print "Arduino Serial Read Running"
while 1:
	data = ard_ser.read(1)
	file.write( str(data) )
