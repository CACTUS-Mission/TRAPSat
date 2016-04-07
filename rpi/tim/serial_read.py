#!/usr/bin/python

import sys, serial, time, re, os
ser = serial.Serial("/dev/ttyUSB0", 19200, timeout=0)
print ser.portstr

def getSerialData():
	print("getSerialData() called.")
	file = open('test.data', 'w')

	while(len(ser.read(1)) == 0): # wait for the first byte
		print("waitng for serial data.")
	file.write(chr(0xF1))
	
	time_start = int(time.time())
	last_time = time_start
	print("start time: "+str(time_start))
	while( (int(int(time.time()) - time_start) < int(7*60)) ): # while time < 8 min
		if( (int(time.time()) - time_start) != last_time ):
			print("time: " + str( int(int(time.time())-time_start) ) )
			last_time = int(time.time()) - time_start
		data = ser.read(1)  # read one byte at a time
		if(len(data) != 0):
			file.write(data) 	# write one byte to file at a time
			#print("data written to file: "+str(data))
	ser.close()
	file.close()


def getFileNames():
	read_file = open('test.data', 'r') # reopen from the beggining
	write_file = open('test_file_names.txt', 'w')
	read_data = read_file.read( os.path.getsize('test.data') )
	#print read_data -- you don't want to do this. Bad idea
	
	#regex = re.compile("..._._....\....")
	
	filenames = re.findall(r'..._[0-1]_.{4,5}\....', read_data, re.S)
	reset = 0
	for name in filenames:
		if(reset == 1 and name == "001_1_0000.jpg"): # break when we see the log file
			break
		if(name == "001_1_0000.jpg"):
			reset = 1
 		write_file.write(name)
		write_file.write("\n")

	write_file.close()


#### Main ####
print("Get Serial Data?")
while 1:
	i = raw_input()
	if(i == "y"):
		getSerialData()
		break
	if(i == "n"):
		break

print("Parse filenames?")
while 1:
	i = raw_input();
	if(i == "y"):
		getFileNames()
		break
	if(i == "n"):
		break

exit()