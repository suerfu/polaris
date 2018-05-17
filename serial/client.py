#!/usr/bin/python


import socket
import sys


import numpy as np
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure

# Create a UDS socket
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = '/tmp/12345';
print >>sys.stderr, 'Connecting to %s' % server_address

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

print ' Remote closed...Terminating...'
sock.close()



class App:
    def __init__(self, master):

        # Create a container
        frame_button = Tkinter.Frame(master)
        frame_indicator = Tkinter.Frame(master)

        self.go = False
        self.dir = 1
        self.period = np.pi
        self.attn = 3.
        self.attn_var = Tkinter.StringVar()
        self.period_var = Tkinter.StringVar()

        # Create 2 buttons
        self.button_go = Tkinter.Button( frame_button, text="< GO >", command=self.move)
        self.button_go.grid(row=0, column=0)

        self.button_stop = Tkinter.Button( frame_button, text="< STOP >", command=self.stop)
        self.button_stop.grid(row=1, column=0)

        self.button_dir = Tkinter.Button(frame_button,text="< REVERSE >", command=self.reverse)
        self.button_dir.grid(row=2, column=0)

        self.entry_attn = Tkinter.Entry(frame_indicator)
        self.entry_attn.grid(row=0, column=0)
        self.label_attn = Tkinter.Button(frame_indicator, text="enter attn", command=self.Attn)
        self.label_attn.grid( row=0, column=1)

        self.entry_period = Tkinter.Entry(frame_indicator)
        self.entry_period.grid(row=1, column=0)
        self.label_period = Tkinter.Button(frame_indicator, text="enter period", command=self.Period)
        self.label_period.grid( row=1, column=1)

        # Initialize the figure

        self.phase = 0;
        self.phase_inc = self.dir*10*np.pi/500;

        self.x = np.linspace( 0, 5*np.pi, 500)
        self.y = np.exp(-self.x/self.attn)*np.sin(self.x/self.period + self.phase)
        
        fig = Figure()
        self.ax = fig.add_subplot(111)
        self.ax.set_ylim( bottom = -1 )
        self.ax.set_ylim( top = 1 )
        self.ax.set_title("Damp")
        self.line, = self.ax.plot( self.x, self.y, '-')

        self.canvas = FigureCanvasTkAgg(fig,master=master)
        self.canvas.show()
        # self.canvas.get_tk_widget().pack(side='right', fill='both', expand=1)
        self.canvas.get_tk_widget().grid(row=0, column=1)

        # frame_button.pack( side='top' )
        # frame_indicator.pack( side='top' )
        frame_button.grid( row = 0, column = 0 )
        frame_indicator.grid( row = 1, column = 0 )

        # Next figure

        self.x2 = np.linspace( 0, 5*np.pi, 500)
        self.y2 = np.exp(-self.x2/self.attn)*np.cos(self.x2/self.period + self.phase)
        
        fig2 = Figure()
        self.ax2 = fig2.add_subplot(111)
        self.ax2.set_ylim( bottom = -1 )
        self.ax2.set_ylim( top = 1 )
        self.ax2.set_title("Damp-2")
        self.line2, = self.ax2.plot( self.x2, self.y2, '-')

        self.canvas2 = FigureCanvasTkAgg(fig2,master=master)
        self.canvas2.show()
        self.canvas2.get_tk_widget().grid(row=1, column=1)

    def update(self):
        if self.go:
            self.line.set_ydata(np.exp(-self.x/self.attn )*np.sin(self.x/self.period + self.phase))
            self.line2.set_ydata(np.exp(-self.x2/self.attn )*np.cos(self.x2/self.period + self.phase))
            #self.ax.relim()
            #self.ax.autoscale_view()
            self.canvas.draw()
            self.canvas2.draw()
            self.phase += self.dir * self.phase_inc

    def move(self):
        self.go = True

    def stop(self):
        self.go = False

    def reverse(self):
        self.dir *= -1
        if self.dir==-1:
            self.button_dir.configure( bg='yellow' )
        else:
            self.button_dir.configure( bg='grey' )

    def Attn(self):
        print "Callback 1"
        self.attn = float(self.entry_attn.get())
        print self.entry_attn.get()

    def Period(self):
        print "Callback 2"
        self.period = float(self.entry_period.get())
        print self.entry_period.get()

def animate():
    app.update()
    root.after(33, animate)

root = Tkinter.Tk()
app = App(root)
animate()
root.mainloop()
