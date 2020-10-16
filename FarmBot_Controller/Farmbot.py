# -*- coding: utf-8 -*-
"""
Created on Thu Mar 12 16:16:52 2020

"""
import serial
import numpy as np
import Pots
import threading
import time
import signal
from serial import Serial


class Farmbot:
    def __init__(self,PcomPort,PbaudRate):
        self.comPort = PcomPort
        self.baudRate = PbaudRate
        self.posX=0
        self.posY=0
        self.posZ=0
        self.ValPins=np.zeros(97)
        self.watering=False
        self.pots = [Pots.Pots(0,65,60,183,193,96,1),
                     Pots.Pots(0,70,65,1683,193,95,2),
                     Pots.Pots(0,75,70,183,1757,94,3),
                     Pots.Pots(0,80,75,1683,1757,93,4)]
        self.leer=False
        self.potThreads=[threading.Thread(target=self.waterPot,args=(self.pots[0],)
                                          ,name="ThreadPot1"),
                         threading.Thread(target=self.waterPot,args=(self.pots[1],)
                                          ,name="ThreadPot2"),
                         threading.Thread(target=self.waterPot,args=(self.pots[2],)
                                          ,name="ThreadPot3"),
                         threading.Thread(target=self.waterPot,args=(self.pots[3],)
                                          ,name="ThreadPot4")]
        self.CommandFinished= False
        self.BoolmoveAuto=False
        
    def initSerialPort(self):
        self.serialClient= serial.Serial(self.comPort,self.baudRate)
        time.sleep(0.2)
        message= "F22 P2 V1\r\n"
        self.serialClient.write(bytes(message,'utf-8'))
        self.CommandFinished=False
        loopCounts=0
        maxCount=150
        while not (self.CommandFinished)and loopCounts<=maxCount:
            loopCounts+=1
            if(self.serialClient.in_waiting>0 and self.serialClient.out_waiting ==0):
                    respuesta= self.serialClient.readline().decode()
                    print("Receive ->"+respuesta)
                    self.readLine(respuesta)
            time.sleep(100/1000)
        
        print("Comunicacion Serial Iniciada")
        if loopCounts >= maxCount:
            raise TimeoutError("Farmbot didn't reply make sure i'ts connected")
    
    def closeSerialPort(self):
        self.serialClient.close()
        print("Comunicacion Serial Finalizada")
        
    def receiveMesagesContinuos(self):
        print("Started Reading")
        self.leer=True
        while(self.leer):
            if(self.serialClient.in_waiting>0 and self.serialClient.out_waiting ==0):
                respuesta= self.serialClient.readline().decode()
                print("Receive ->"+respuesta)
                self.readLine(respuesta)    
        time.sleep(10/1000)
        print("Finished Reading")        
    def waitCommandExecuted(self):
        self.CommandFinished=False
        loopCounts=0
        maxCount=600
        while not self.CommandFinished and loopCounts<=maxCount:
            loopCounts+=1
            time.sleep(100/1000)
        if loopCounts >= maxCount:
            raise TimeoutError("Farmbot didn't reply make sure i'ts connected")
            
    def endCommunication(self):
        self.leer=False        
        
    def water(self,w=0):
        if(w==1): #Debe encender water_pin
            message='F41 P8 V'+str(w)
        else:
            message= 'F41 P8 V'+str(w)
        print(message)
        message+="\r\n"
        self.serialClient.write(bytes(message,'utf-8'))
        self.waitCommandExecuted()
        
    def leerPin(self,pin):
        #Sensor matera 1 -> A0 -> pin 97
        #Sensor matera 2 -> A1 -> pin 96
        #Sensor matera 3 -> A2 -> pin 95
        #Sensor matera 4 -> A3 -> pin 94
        message='F42 P'+str(pin)
        print(message)
        message+="\r\n"
        self.serialClient.write(bytes(message, 'utf-8'))
        self.waitCommandExecuted()
        
    def escrbirPin(self,pin,value):
        mensaje='F41 P'+str(pin) +" V"+str(value)
        print(mensaje)
        mensaje+="\r\n"
        self.serialClient.write(bytes(mensaje, 'utf-8'))
        self.waitCommandExecuted()
        
    def moveManual(self):
        self.BoolmoveAuto=False
        # for thread_pot in self.potThreads:
            #print("Cheking Pot %d"%(i+1))
            # if thread_pot.:
            #     thread_pot.join()
        
    def moveAuto(self):
        #Para la matera 1 60<humidity<65
        #Para la matera 2 65<humidity<70
        #Para la matera 3 70<humidity<75
        #Para la matera 4 75<humidity<80
        self.BoolmoveAuto=True
        while(self.BoolmoveAuto):
            for pot in self.pots:
                if not self.watering:    
                    self.waterPot(pot)
            time.sleep(500/1000)#500 ms sleep
            
    def updateHumiditySensors(self):
        self.leerPin(97)
        self.leerPin(96)
        self.leerPin(95)
        self.leerPin(94)
        self.pots[0].setActualHumidity(self.ValPins[self.pots[0].pin]*100/1023)
        self.pots[1].setActualHumidity(self.ValPins[self.pots[1].pin]*100/1023)
        self.pots[2].setActualHumidity(self.ValPins[self.pots[2].pin]*100/1023)
        self.pots[3].setActualHumidity(self.ValPins[self.pots[3].pin]*100/1023)

    def waterPot(self, pot):
        #Actualiza el valor del pin del pot
        # print("Cheking Humidity of pot %s"%pot.potNumber)
        
        while(pot.checkHumidity()):
            self.watering=True
            print("Watering pot %s"%pot.potNumber)
            self.move(pot.x + 50 , pot.y + 48 , 0)
            self.water(1)
            self.move(pot.x + 1075, pot.y + 48 , 0)
            self.move(pot.x + 1075, pot.y + 593 , 0)
            self.move(pot.x + 50 , pot.y + 593 , 0)
            self.move(pot.x + 50 , pot.y + 1039, 0)
            self.move(pot.x + 1075, pot.y + 1039, 0)
            self.water(0)
            self.move(pot.x,pot.y,0)
        
            # self.leerPin(pot.pin)
            # time.sleep(5)
            # pot.setActualHumidity(self.ValPins[pot.pin]*100/1023)
            self.updateHumiditySensors()
            print("Humidity after waering %s"%(pot.humidityActual))
        self.watering=False

    def move(self,x=0, y=0, z=0):
        #Actualiza las pocisiones
        self.posX= x
        self.posY= y
        self.posZ= z
        if x<0:
            self.posX=0
        if y<0:
            self.posY=0
        if z<0:
            self.posZ=0
    
        if (x==0 and y==0 and z==0):#Es porque va para el home
            message="G0"
        else:#Se mueve a otra ubicacion
            message='G00 X'+str(x)+' Y'+str(y)+' Z'+str(z)
        print("send-> "+message)
        message+="\r\n"
        self.serialClient.write(bytes(message,'utf-8'))
        self.waitCommandExecuted()

            
    """
    Secuencia de respuestas del farmbot:
    R00 -> Ready
    R87 -> Locked
    R88 -> Config not Aproved
    R81 -> Report Endstops
    R82 -> Current Position (mm)
    R84 -> Encoder Position (mm)
    R85 -> Encoder Pulses (mm)
    R05 -> Axis State
    R71, R72, R73 Timeout each axis    
    R11,R12, R13 Homing complete each axis
    R08 Echo
    R09 Invalid
    R01 Started
    R04 Running
    R02 Finished successfully
    R03 Finished with error
    R06 Response of calibration
    R41 P V parameter and value
    R07 Movement retry
"""
    
    def readLine(self,message):
        # print(message)
        if(message.startswith('R82')):
            self.readPosition(message[4:])
        elif(message.startswith('R41')):
            self.readPinValue(message[4:])
        elif(message.startswith('R02')):
            self.CommandFinished=True
        elif(message.startswith('R01')):
            pass
            
    def readPosition(self,position):
        position=position.split(' ')
        self.x=position[0]
        self.y=position[1]
        self.z=position[2]
        self.x=float(self.x[1:])
        self.y=float(self.y[1:])
        self.z=float(self.z[1:])

    def readPinValue(self,pValue):
        pValue=pValue.split(' ')
        pin=pValue[0]
        value=pValue[1]
        pin=int(pin[1:])
        value=int(value[1:])
        self.ValPins[pin - 1]=value
        
    def readEncoder(self,pEncoder):
        pass
    def readEncoderPulse(self,pEncoderPulse):
        pass
    def readError(self,pError):
        pass
    