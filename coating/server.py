#!/usr/bin/python

import socket
import sys

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
from collections import deque

maxlen = 250
index_x0 = 0
index_y0 = 1
index_y1 = 2


hostname = 'localhost'

#server_address = '/tmp/12345';
#server_address = "10.25.250.152"

hostname = 'localhost'  # socket.gethostname()
port = 6400

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    sock.bind( (hostname, port) )
except:
    print "Bind failed"
    sys.exit(1)

sock.listen(5)

newsock, newaddr = sock.accept()

print "Accepted connection from: ", newaddr

fig = plt.figure()
ax1 = fig.add_subplot(2,1,1)
ax2 = fig.add_subplot(2,1,2)

deqx = deque(maxlen=maxlen)
deqy0 = deque(maxlen=maxlen)
deqy1 = deque(maxlen=maxlen)


def animate(i):
    try:
        data = newsock.recv(1000);
        if( data=="" ):
            print "No data received"
            sys.exit(1)
        #print data

    except:
        print "Problem receiving data from server"
        sys.exit(1)

    deqx.append( float( data.split()[index_x0]))
    deqy0.append( float( data.split()[index_y0]) )
    deqy1.append( float( data.split()[index_y1]) )

    ax1.clear()
    ax1.plot(deqx,deqy0)
    plt.subplot(211)
    plt.title("Temperature vs Time")
    plt.xlabel("Time (s)")
    plt.ylabel("Temperature (C)")

    ax2.clear()
    ax2.plot(deqx,deqy1)
    plt.subplot(212)
    plt.title("Pressure vs Time")
    plt.xlabel("Time (s)")
    plt.ylabel("Pressure (Torr)")


ani = animation.FuncAnimation(fig, animate, interval=100)
plt.show()

print 'Remote closed...Terminating...'
sock.close()

