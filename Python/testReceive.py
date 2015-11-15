#!/usr/bin/python

import serial, time

ser = serial.Serial('/dev/tty.usbserial', 115200)
print ("connected to: " + ser.portstr)



#try: 
#    ser.open()
	
#except Exception, e:
#    print "Error opening serial port: " + str(e)
#    exit()

if ser.isOpen():

	#try:
		ser.flushInput() 	#flush input buffer, discarding all its contents
		ser.flushOutput()	#flush output buffer, aborting current output 
							#and discard all that is in buffer							
		while True:
			for c in ser.read():
				print ord(c)
			
	#except Exception, e:
    #	print "Error reading serial port: " + str(e)
    #	exit()
		
else:
	print "Error opening port...: " + str(e1)
