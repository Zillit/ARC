import spidev

spi = spidev.SpiDev()
spi.open(0,0)
spi.mode = 0b00


while True:
    val = input('d eller s: ')
    if val == 'd':
         degrees = input('Hur många grader: ')
         resp = spi.xfer2([int(degrees)],250000,1,8)    
    if val == 's':
        speed = input('Hur snabbt: ')
        resp = spi.xfer2([int(speed)+128+30],250000,1,8)
    val = None
