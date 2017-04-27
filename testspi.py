import pygame
import sys
import time
import numpy as np
import spidev
import picamera 

import os

spi = spidev.SpiDev()
spi.open(0,0)
spi.mode = 0b00
resp = spi.xfer2([15],250000,1,8)

