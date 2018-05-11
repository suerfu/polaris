import socket
import sys

# Create a UDS socket
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = '/tmp/12345';
print >>sys.stderr, 'connecting to %s' % server_address

try:
    sock.connect(server_address)
except socket.error, msg:
	print >>sys.stderr, msg
	sys.exit(1)

print 'Connection established'
need = 1000;
while True:
    try:
        data = sock.recv(1000);
        if( data=="" ):
	        break;
        print data
    except:
        break;

print 'Closing socket'
sock.close()
