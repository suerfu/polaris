#!/usr/bin/python

import socket
import sys

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
from collections import deque

frame = 100
maxlen = 100


hostname = 'localhost'
#hostname = "10.25.250.152"

port = 6400

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print >>sys.stderr, 'Connecting to %s' % hostname

try:
    sock.connect( (hostname, port) )
except socket.error, msg:
	print >>sys.stderr, msg
	sys.exit(1)

print 'Connection established'


fig = plt.figure()

deqx = deque( maxlen=maxlen )
deqy = deque( maxlen=maxlen )

def animate(i):

    try:
        data = sock.recv( frame );
        if( data=="" ):
            print "No data received"
            sys.exit(1)

    except:
        print "Problem receiving data from server"
        sys.exit(1)

#    array = [float(x) for x in data.split()]
    deqy.append( float( data.split()[0]) )

    fig.clear()
    plt.plot(deqy )


ani = animation.FuncAnimation(fig, animate, interval=100)
plt.show()

print 'Remote closed...Terminating...'
sock.close()

