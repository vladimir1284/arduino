# from pylab import *
# import csv

import serial
import matplotlib.pyplot as plt
import numpy as np
import pickle

data_fname = "stored_data.p"
fig_fname = 'logic.eps'
LA_port = "/dev/ttyACM0"  # linux DFU
real = True
#real = False

pinChanged = []
usTime = []
samples = 0
initialState = 0


def store_data(): 
    com = serial.Serial(port=LA_port, baudrate=115200)
    com.write("G10\n")
    
    inString = com.read_until('S')
    inString = com.read_until('\n')
    inString = com.read_until('\n')
    lista = inString.split(':')
    initialState = int(lista[0])
    samples = int(lista[1])
    print("Estado inicial: %i, Muestras: %i" % (initialState, samples))
    pinChanged = []
    usTime = []
    
    for i in range(samples): 
        inString = com.read_until('\n')
        lista = inString.split(':')
        pinChanged.append(int(lista[0]))
        usTime.append(float(lista[1]))
        
    data = [initialState, samples, pinChanged, usTime]
    pickle.dump(data, open(data_fname, "wb"))
    return data
    
       
# Get data from the selected source    
if (real):
    [initialState, samples, pinChanged, usTime] = store_data()
else:
    [initialState, samples, pinChanged, usTime] = pickle.load(open(data_fname , "rb"))
    
# Process Data
mask = 1
initalStates = []
for n in range(4):
    b = initialState & mask
    initalStates.append(bool(b))
    mask <<= 1
    
states = [range(samples), range(samples), range(samples), range(samples)]
for i, change  in enumerate(pinChanged):
    mask = 1
    for n in range(4):
        b = change & mask
        states[n][i] = bool(b)
        mask <<= 1        


def my_lines(ax, pos, *args, **kwargs):
    if ax == 'x':
        for p in pos:
            plt.axvline(p, *args, **kwargs)
    else:
        for p in pos:
            plt.axhline(p, *args, **kwargs)


t = np.array(usTime)
t = t / 1000  # Convert to ms

# X ticks
x0 = t[0]
myXTicks = [0]
for x in t[1:]:
    diference = x - x0
    x0 = x
    myXTicks.append(diference)

strTicks = []
xTicksPos = []
last = True
for i,x in enumerate(myXTicks):
    if (x > 0.5):
        if (last):
            str0 = "{0}"
        else:
            str0 = "\n{0}"
        strTicks.append(str0.format(str(round(myXTicks[i], 1) if diference % 1 else int(myXTicks[i]))))
        last = not(last)
    else:
        strTicks.append("")
    
        
strTicks = np.array(strTicks)

# Plot 
color_pallete = ['b', 'r', 'g', 'k']
legend_array = ["E1", "","S"]
ylines = [0, 1.5, 3, 4.5]
img = plt.figure(figsize=(30,10))
# my_lines('y', [ylines[n]], color='.5', linewidth=0.5)      
# my_lines('x', t, color='.5', linewidth=0.5)
 
for n in range(4):
    if (True in states[n]):
        yValues = []
        initial = initalStates[n]
        for i in range(samples):
            if (states[n][i]):
                initial = not(initial)
            yValues.append(initial)  
        data = np.array(yValues)  
        for j in range(4):
            indx = range(j * samples / 4,(j + 1) * samples / 4)
            plt.subplot(4, 1, j + 1)            
            if (n == 0):
                plt.ylim([-0.5, 6])
                plt.xticks(t[indx],strTicks[indx],fontsize=7)
                plt.yticks([])
                my_lines('y', [ylines[n]], color='.5', linewidth=0.5)  
                my_lines('x', t[indx], color='.5', linewidth=0.5)  
            plt.step(t[indx], data[indx] + 1.5 * n, color_pallete[n], linewidth=1, where='post')

                # plt.gca().axis('off')                        
        # plt.step(t, data + 1.5 * n, color_pallete[n], linewidth=1, where='post')
        # plt.ylim([-0.5, 6])
        # plt.xticks(t,strTicks,fontsize=3)               

#plt.show()
img.savefig(fig_fname)   
