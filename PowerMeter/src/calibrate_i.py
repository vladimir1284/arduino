from pylab import *

Imeter = [.2, .32, .4, .5, .6, .69, .8, .91, .99]
Isensor = [.11, .18, .23, .29, .35, .40, .46, .52, .58]

p = polyfit(Imeter,Isensor,1)
print(p)
x = linspace(min(Imeter), max(Imeter),50)
y = polyval(p,x)

img = figure(figsize=(6,4))
plot(Imeter,Isensor,'r*',x,y)
grid('on')
img.savefig('calibrate_i.png')