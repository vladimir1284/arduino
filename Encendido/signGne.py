from utime import sleep_ms
from machine import Pin

pOut = Pin(2, Pin.OUT)

Nsamples = 50
dutyCycle = 80
rpms = [400, 550, 700, 800, 900, 1000, 1400, 1200, 600, 400]

def rpm2period(rpm):
    return 30000/rpm

currSamples = Nsamples/len(rpms)

for rpm in rpms:
    T = rpm2period(rpm)
    Ton = int(dutyCycle*T/100)
    Toff = int((100-dutyCycle)*T/100)
    for i in range(currSamples):
        sleep_ms(Toff)
        pOut.on()
        sleep_ms(Ton)
        pOut.off()
