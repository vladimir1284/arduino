from pylab import *

Vmeter = [92, 96.1, 101, 104.8, 110.3, 108.1, 115.4, 120.2, 126.6, 130.5]
Vsensor = [97.4, 101.8, 106.8, 110.9, 116.7, 114.4, 122, 127.1, 133.8, 137.9]

p = polyfit(Vmeter,Vsensor,1)
print(p)
x = linspace(min(Vmeter), max(Vmeter),50)
y = polyval(p,x)

img = figure(figsize=(6,4))
plot(Vmeter,Vsensor,'r*',x,y)
grid('on')
img.savefig('calibrate.png')