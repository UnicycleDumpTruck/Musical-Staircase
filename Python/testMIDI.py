#!/usr/bin/python

import serial, time, mido

ser = serial.Serial('/dev/tty.usbserial', 115200)
print ("connected to: " + ser.portstr)

output = mido.open_output()


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
				command = (ord(c) - 32)
				print command
				if command < 32:
					output.send(mido.Message('note_on', note=command, velocity=120))
				else:
					output.send(mido.Message('note_off', note=(command - 32), velocity=0))
			
	#except Exception, e:
    #	print "Error reading serial port: " + str(e)
    #	exit()
		
else:
	print "Error opening port...: " + str(e1)
