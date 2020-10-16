# -*- coding: utf-8 -*-
"""
Created on Thu May 28 01:47:48 2020
"""


class Pots:
    
    def __init__(self,p_humidityA,p_humidityU,p_humidityD,p_x,p_y,p_pin,p_potNumber):
        
        self.humidityActual = p_humidityA
        self.humidityLimUp = p_humidityU
        self.humidityLimDown = p_humidityD
        self.x = p_x
        self.y = p_y
        self.pin=p_pin
        self.potNumber= p_potNumber
        
    def setActualHumidity(self,p_humidityA):
        self.humidityActual = p_humidityA
        # print(self.humidityActual)

    def checkHumidity(self):      
        if(self.humidityActual < self.humidityLimDown):
            return True
        else:
            return False
        print(self.humidityActual)
        
    