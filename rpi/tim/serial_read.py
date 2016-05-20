#!/usr/bin/python

import sys, serial, time, re, os, subprocess, signal

SERIAL_PORT = "/dev/ttyUSB0"
RAW_DATA_FILE = "test.data"


#####################################################################################
DEFAULT_RX_TIME = 465                   # seconds, (End at T+330 seconds)
LONG_RX_TIME = 495                      # additional 30 second test run
HOUR_RX_TIME = 3600                     # seconds in 1 hour
HALF_DAY_RX_TIME = HOUR_RX_TIME * 12    # half-day test
#####################################################################################
RX_TIME_SEC = DEFAULT_RX_TIME           # Time used by program: change this for tests


def getSerialData():
	try:
		ser = serial.Serial(SERIAL_PORT, 19200, timeout=0)
	except serial.SerialException:
		print "Dont Panic!"
		print "Serial Port could not be opened."
		print "1. Check to see if the USB cable is plugged in correctly."
		print "\tand the breakout wires go to the correct pins"
		print "\t\tAsync Data BLACK wire connects to pin 33"
		print "\t\tAsync Data RED wire connects to pin 32"
		print "\t\tDont f*** it up, it's important."
		print "2. If the cable is ok, a reboot may fix this issue, "
		print "3. If not, run (dmesg | grep \"tty\") to see USB status"
		print "4. Otherwise, please notify Keegan or Zach."
		print "\tZach: 302-245-6170"
		print "\tKeegan: 302-542-2293"
		exit(1)


	waiting = 1

	print("getSerialData() called.")
	file = open('test.data', 'w')
	
	while(len(ser.read(1)) == 0): # wait for the first byte
		if(waiting):
			print("waitng for serial data.")	
			waiting = 0

	# Start recording from Arduino in subprocess
	ard_ser = subprocess.Popen(["sudo", "python", "parallel_read_arduino.py"])

	file.write(chr(0xF1))
	
	time_start = int(time.time())
	last_time = time_start
	print("start time: "+str(time_start))
	while( (int(int(time.time()) - time_start) < int(RX_TIME_SEC)) ): # while time < 8 min
		if( (int(time.time()) - time_start) != last_time ):
			print("time: " + str( int(int(time.time())-time_start) ) )
			last_time = int(time.time()) - time_start
		data = ser.read(1)  # read one byte at a time
		if(len(data) != 0):
			file.write(data) 	# write one byte to file at a time
			#print("data written to file: "+str(data))
	
	ard_ser.send_signal(signal.SIGINT) # Kill arduino read subprocess
	ser.close()
	file.close()


def getFileNames():
	read_file = open(RAW_DATA_FILE, 'r') # reopen from the beggining
	write_file = open('test_file_names.txt', 'w')
	read_data = read_file.read( os.path.getsize('test.data') )
	#print read_data -- you don't want to do this. Bad idea
	
	#regex = re.compile("..._._....\....")
	
	filenames = re.findall(r'..._[0-1]_.{4,5}\....', read_data, re.S)
	reset = 0
	for name in filenames:
		if(reset == 1 and name == "001_1_0001.jpg"): # break when we see the log file
			break
		if(name == "001_1_0001.jpg"):
			reset = 1
 		write_file.write(name)
		write_file.write("\n")

	write_file.close()


#### Main ####
#print("Get Serial Data?")
print "Start TRAPSat RockSat-x Test? (Y/n)"
while 1:
	i = raw_input()
	if(str(i).lower() == "y"):
		getSerialData()
		break
	if(str(i).lower() == "n"):
		break
print("Starting Data Parser...")
pipe = subprocess.Popen(["perl", "pop.pl", RAW_DATA_FILE])

# print("Parse filenames?")
# while 1:
# 	i = raw_input();
# 	if(i == "y"):
# 		getFileNames()
# 		break
# 	if(i == "n"):
# 		break

exit()

