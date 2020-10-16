# -*- coding: utf-8 -*-
"""
Created on Thu Mar 12 16:57:31 2020

"""
from tkinter import *
import tkinter as tk
import tkinter.font as tkFont
from PIL import Image
import threading
import time
import Farmbot
from tkinter import messagebox
from serial import Serial


class MainInterface:
    def __init__(self,master):
        self.mainWindow=master
        
        #Titulo de la interfaz
        self.mainWindow.title("Welcome to Farmbot Uniandes GUI")
        
        #Tamaño de la interfaz
        self.mainWindow.geometry('870x550')
        
        #Crea el panel para el logo
        self.logoFrame=LogoFrame(self)
        self.logoFrame.grid(column=1,row=0)
        
        #Crea el panel de modos
        self.modesFrame=ModesFrame(self)
        self.modesFrame.grid(column=1,row=1)
        
        #Crea el panel para el automatico-manual
        self.autoManFrame=AutoManFrame(self)
        self.autoManFrame.grid(column=0,row=2)
        
        #Crea el manual los movimientos del farmbot
        self.movementFrame=MovementFrame(self)
        self.movementFrame.grid(column=1,row=2)
        
        #Crea el panel para los Leds
        self.ledsFrame=LedsFrame(self)
        self.ledsFrame.grid(column=1,row=3)
        
        #Crea el panel de los sensores higrometros
        self.sensorFrame=SensorFrame(self)
        self.sensorFrame.grid(column=2,row=2)
        
        #Varialbles del programa
        self.pararSensores = False
        self.comPort="COM2"
        self.baudRate=115200
        self.flag=0
        self.manualFlag=0
        self.autoFlag=0
        self.startFlag=0
        self.stopFlag=1
        
        #Crea un nuevo farmbot
        self.farmbot= Farmbot.Farmbot(self.comPort,self.baudRate)
        
        #incializa los threads de la interfaz
        self.threadAuto = threading.Thread(target= self.moveAutoFunc)
        self.threat_receive=threading.Thread(target= 
                                             self.farmbot.receiveMesagesContinuos)
        self.threadManual= threading.Thread(target= self.moveManualFunc)
        self.threadWaterPots= threading.Thread(target= self.farmbot.moveAuto)
        
        
    #Funciones para los botones

    def ClickStart(self):
        if self.startFlag ==0 and self.stopFlag==1:
            try:
                
                self.startFlag=1
                self.stopFlag=0
                print("Start")
                self.farmbot.initSerialPort()
                self.ledsFrame.cnvLedOff.create_oval(25,10,75,60,fill="#61605c") #Gris
                self.ledsFrame.cnvLedOn.create_oval(25,10,75,60,fill="#03FF00") #Verde
                self.ledsFrame.cnvLedManu.create_oval(25,10,75,60,fill="#FFFD00") #Amarillo
                self.pararSensores = True
                self.manualFlag =1
                self.autoFlag=0
                print("Reading")
                # Empieza el thread de leer mensajes
                if not self.threat_receive.is_alive():
                    self.threat_receive = threading.Thread(target= 
                                             self.farmbot.receiveMesagesContinuos)
                if not self.threadManual.is_alive():
                    self.threadManual=threading.Thread(target=self.moveManualFunc)
                self.threat_receive.start()
                self.threadManual.start()
            except Exception as ex:
                self.startFlag=0
                self.stopFlag=1
                self.farmbot.closeSerialPort()
                messagebox.showerror("Error Starting Farmbot",ex.__str__())
                
        else:
            print("Farmbot Already Started")
        
    def read_writeSensores(self):
        #leer sensores

        #Espera 30 seg para que el FarmBot regrese las respuestas de los sensores

        #Actualiza el valor de los sensores
        self.sensorFrame.subSensorFrame1.lblSensor11.config(
            text=str(int(self.farmbot.ValPins[96]*100/1023))+' %')
        self.sensorFrame.subSensorFrame2.lblSensor22.config(
            text=str(int(self.farmbot.ValPins[95]*100/1023))+' %')
        self.sensorFrame.subSensorFrame3.lblSensor33.config(
            text=str(int(self.farmbot.ValPins[94]*100/1023))+' %')
        self.sensorFrame.subSensorFrame4.lblSensor44.config(
            text=str(int(self.farmbot.ValPins[93]*100/1023))+' %')
            
        #Espera 30 seg para volver a leer los sensores
    def moveManualFunc(self):
        while(self.manualFlag):
            windowTemp=tk.Toplevel(self.mainWindow)
            tk.Label(windowTemp,text= "Updating Sensors").pack()
            self.farmbot.updateHumiditySensors()
            self.read_writeSensores()
            windowTemp.destroy()
            for i in range(60):
                if not self.manualFlag:
                    return
                time.sleep(30/60)
    def moveAutoFunc(self):
        if not self.threadWaterPots.is_alive() :
                self.threadWaterPots= threading.Thread(target= self.farmbot.moveAuto)
        self.threadWaterPots.start()
        while(self.autoFlag):
            #Update the values of sensors in the interface
            if not self.farmbot.watering:
                self.farmbot.updateHumiditySensors()
                self.read_writeSensores()

            time.sleep(10)#5s sleep
                
        
    def ClickStop(self):
        if self.stopFlag==0 and self.startFlag==1:
            self.startFlag=0
            self.stopFlag=1
            self.farmbot.endCommunication()
            self.threat_receive.join()
            self.farmbot.closeSerialPort()
            self.pararSensores = False
        
            self.ledsFrame.cnvLedOff.create_oval(25,10,75,60, fill="#FF0000")
            self.ledsFrame.cnvLedOn.create_oval(25,10,75,60, fill="#61605c")
            self.ledsFrame.cnvLedAuto.create_oval(25,10,75,60, fill="#61605c")
            self.ledsFrame.cnvLedManu.create_oval(25,10,75,60, fill="#61605c")
            self.ledsFrame.cnvLedWater.create_oval(25,10,75,60, fill="#61605c") 

            print("Stop")
        
    def ClickAuto(self):
        if self.manualFlag==1 and self.autoFlag==0 and self.startFlag==1:
            self.manualFlag=0
            self.autoFlag=1
            self.threadManual.join()
            self.ledsFrame.cnvLedAuto.create_oval(25,10,75,60, fill="#FFFD00")
            self.ledsFrame.cnvLedManu.create_oval(25,10,75,60, fill="#61605c")
            if not self.threadAuto.isAlive():
                self.threadAuto=threading.Thread(target= self.moveAutoFunc)
            self.threadAuto.start()
        print("Auto")
            
    def ClickManual(self):
        if self.manualFlag==0 and self.autoFlag==1 and self.startFlag==1:
            self.autoFlag=0
            self.manualFlag=1
            self.farmbot.moveManual()
            if self.threadAuto.is_alive():   
                self.threadAuto.join()
            if self.threadWaterPots.is_alive():    
                self.threadWaterPots.join()
            self.ledsFrame.cnvLedManu.create_oval(25,10,75,60, fill="#FFFD00")
            self.ledsFrame.cnvLedAuto.create_oval(25,10,75,60, fill="#61605c")
            if not self.threadManual.is_alive():
                self.threadManual=threading.Thread(target=self.moveManualFunc)
            self.threadManual.start()
        print("Manual")
        
    def ClickHome(self):
        if self.manualFlag ==1:
            self.farmbot.move()
            print("Home")
        else:
            print("Farmbot is in autoMode please go to Manual")
        
    def ClickWater(self):
        if(self.flag==0 and self.manualFlag ==1):    
            self.ledsFrame.cnvLedWater.create_oval(25,10,75,60, fill="#FFFD00")
            self.flag=1
            self.farmbot.water(1)
            print("Water")
        elif(self.flag==1 and self.manualFlag ==1):
            self.ledsFrame.cnvLedWater.create_oval(25,10,75,60, fill="#61605c")
            self.flag=0
            self.farmbot.water(0)
            print("No Water")
        elif(self.autoFlag):
            messagebox.showerror("Error Watering","Farmbot Must Be in manual mode")
            

    def ClickXup(self):
        print("xUp")
        if(self.autoFlag==1 and self.startFlag==1):
            messagebox.showerror("Error ","Farmbot Must Be in manual mode")
        elif(self.startFlag==0):
            messagebox.showerror("Error ","Farmbot Must Be started first")
        else:
            self.farmbot.move(x=self.farmbot.posX +100,
                              y=self.farmbot.posY,
                              z=self.farmbot.posZ)
    def ClickXdown(self):
        print("xDown")
        if(self.autoFlag==1 and self.startFlag==1):
            messagebox.showerror("Error ","Farmbot Must Be in manual mode")
        elif(self.startFlag==0):
            messagebox.showerror("Error ","Farmbot Must Be started first")
        else:
            self.farmbot.move(x=self.farmbot.posX -100,
                              y=self.farmbot.posY,
                              z=self.farmbot.posZ)
    def ClickYup(self):
        print("yUp")
        if(self.autoFlag==1 and self.startFlag==1):
            messagebox.showerror("Error ","Farmbot Must Be in manual mode")
        elif(self.startFlag==0):
            messagebox.showerror("Error ","Farmbot Must Be started first")
        else:
            self.farmbot.move(x=self.farmbot.posX,
                              y=self.farmbot.posY+100,
                              z=self.farmbot.posZ)
            
    def ClickYdown(self):
        print("yDown")
        if(self.autoFlag==1 and self.startFlag==1):
            messagebox.showerror("Error ","Farmbot Must Be in manual mode")
        elif(self.startFlag==0):
            messagebox.showerror("Error ","Farmbot Must Be started first")
        else:
            self.farmbot.move(x=self.farmbot.posX,
                              y=self.farmbot.posY-100,
                              z=self.farmbot.posZ)
    def ClickZup(self):
        print("zUp")
        if(self.autoFlag==1 and self.startFlag==1):
            messagebox.showerror("Error ","Farmbot Must Be in manual mode")
        elif(self.startFlag==0):
            messagebox.showerror("Error ","Farmbot Must Be started first")
        else:
            self.farmbot.move(x=self.farmbot.posX,
                              y=self.farmbot.posY,
                              z=self.farmbot.posZ+100)
    def ClickZdown(self):
        print("zDown")
        if(self.autoFlag==1 and self.startFlag==1):
            messagebox.showerror("Error ","Farmbot Must Be in manual mode")
        elif(self.startFlag==0):
            messagebox.showerror("Error ","Farmbot Must Be started first")
        else:
            self.farmbot.move(x=self.farmbot.posX ,
                              y=self.farmbot.posY,
                              z=self.farmbot.posZ-100)

class ModesFrame(tk.Frame):
    
    def __init__(self,mainWindow):
        super().__init__(mainWindow.mainWindow)
        #Estilos
        self.fontStyle = tkFont.Font(family="Lucida Grande", size=25)
        self.pixelVirtual = tk.PhotoImage(width=1, height=1)
        
        #Crea los botones del panel
        self.config(width=701,height=380)
        
        self.btnStart=tk.Button(self,text="Start",bg="green",fg="white",
                        height=70,
                        width=130,
                        image=self.pixelVirtual,
                        compound="c",
                        command=mainWindow.ClickStart)
        self.btnStop=tk.Button(self,text="Stop",bg="red",fg="white",
                        height=70,
                        width=130,
                        image=self.pixelVirtual,
                        compound="c",
                        command=mainWindow.ClickStop)
        self.btnHome=tk.Button(self,text="Home",bg="orange",fg="white",
                        height=70,
                        width=130,
                        image=self.pixelVirtual,
                        compound="c",
                        command=mainWindow.ClickHome)
        self.btnWater=tk.Button(self,text="Watering",bg="blue",fg="white",
                        height=70,
                        width=130,
                        image=self.pixelVirtual,
                        compound="c",
                        command=mainWindow.ClickWater)
        #Los agrega Horizontalmente
        self.btnStart.grid(column=0, row=0)
        self.btnStop.grid(column=1, row=0)
        self.btnHome.grid(column=2, row=0)
        self.btnWater.grid(column=3, row=0)

class SensorFrame(tk.Frame):
    def __init__(self,mainWindow):
        self.fontStyle = tkFont.Font(family="Lucida Grande", size=25)
        self.pixelVirtual = tk.PhotoImage(width=1, height=1)
        self.imageSensor = tk.PhotoImage(file="images/bla.png")
        self.imageSensor = self.imageSensor.subsample(20)
        super().__init__(mainWindow.mainWindow)
        
        #Crea el titulo
        self.lblTitle=tk.Label(self, text= "Hygrometer Sensors", width=25, bg = "deep sky blue", fg="white", relief="ridge")
        self.lblImagenS=tk.Label(self, image = self.imageSensor)
        
        #Crea los frames para los sensores y su lectura
        self.subSensorFrame1=SubSensorFrame1(self)
        self.subSensorFrame1.grid(column=0, row=1)
        
        self.subSensorFrame2=SubSensorFrame2(self)
        self.subSensorFrame2.grid(column=0, row=2)
        
        self.subSensorFrame3=SubSensorFrame3(self)
        self.subSensorFrame3.grid(column=0, row=3)
        
        self.subSensorFrame4=SubSensorFrame4(self)
        self.subSensorFrame4.grid(column=0, row=4)
        
        #Agrega los labels al main
        self.lblTitle.grid(column=0, row=0)
        self.lblImagenS.grid(column=0, row=5) 
        
class SubSensorFrame1(tk.Frame):
            def __init__(self,SensorFrame):
                self.fontStyle = tkFont.Font(family="Lucida Grande", size=25)
                self.pixelVirtual = tk.PhotoImage(width=1, height=1)
                super().__init__(SensorFrame)
                
                #Crea los labels de los sensores
                self.lblSensor1=tk.Label(self, text= "Pot #1 :          ")
                self.lblSensor11=tk.Label(self, text= "0 %")
                
                #Agrega los labels al main
                self.lblSensor1.grid(column=0, row=0)
                self.lblSensor11.grid(column=1, row=0)
                
class SubSensorFrame2(tk.Frame):
            def __init__(self,SensorFrame):
                self.fontStyle = tkFont.Font(family="Lucida Grande", size=25)
                self.pixelVirtual = tk.PhotoImage(width=1, height=1)
                super().__init__(SensorFrame)
                
                #Crea los labels de los sensores
                self.lblSensor2=tk.Label(self, text= "Pot #2 :          ")
                self.lblSensor22=tk.Label(self, text= "0 %")
                
                #Agrega los labels al main
                self.lblSensor2.grid(column=0, row=0)
                self.lblSensor22.grid(column=1, row=0)
                
class SubSensorFrame3(tk.Frame):
            def __init__(self,SensorFrame):
                self.fontStyle = tkFont.Font(family="Lucida Grande", size=25)
                self.pixelVirtual = tk.PhotoImage(width=1, height=1)
                super().__init__(SensorFrame)
                
                #Crea los labels de los sensores
                self.lblSensor3=tk.Label(self, text= "Pot #3 :          ")
                self.lblSensor33=tk.Label(self, text= "0 %")
                
                #Agrega los labels al main
                self.lblSensor3.grid(column=0, row=0)
                self.lblSensor33.grid(column=1, row=0)
                
                
class SubSensorFrame4(tk.Frame):
            def __init__(self,SensorFrame):
                self.fontStyle = tkFont.Font(family="Lucida Grande", size=25)
                self.pixelVirtual = tk.PhotoImage(width=1, height=1)
                super().__init__(SensorFrame)
                
                #Crea los labels de los sensores
                self.lblSensor4=tk.Label(self, text= "Pot #4 :          ")
                self.lblSensor44=tk.Label(self, text= "0 %")
        
                #Agrega los labels al main
                self.lblSensor4.grid(column=0, row=0)
                self.lblSensor44.grid(column=1, row=0)
        

class LogoFrame(tk.Frame):
    def __init__(self,mainWindow):
        self.fontStyle = tkFont.Font(family="Lucida Grande", size=25)
        self.pixelVirtual = tk.PhotoImage(width=1, height=1)
        self.imageLogo = tk.PhotoImage(file="images/log.png")
        self.imageLogo = self.imageLogo.subsample(8)
        super().__init__(mainWindow.mainWindow)
        
        #Crea el label
        self.lblLogo = tk.Label(self, image = self.imageLogo)
        
        #Agrega el label a main
        self.lblLogo.grid(column=0, row=0)
        
class LedsFrame(tk.Frame):
    def __init__(self,mainWindow):
        self.fontStyle = tkFont.Font(family="Lucida Grande", size=25)
        self.pixelVirtual = tk.PhotoImage(width=1, height=1)
        super().__init__(mainWindow.mainWindow)
        
        #Crea el canvas para dibujar los circulos
        self.cnvLedOn=tk.Canvas(self, width=100, height=60)
        self.cnvLedOff=tk.Canvas(self, width=100, height=60)
        self.cnvLedAuto=tk.Canvas(self, width=100, height=60)
        self.cnvLedManu=tk.Canvas(self, width=100, height=60)
        self.cnvLedWater=tk.Canvas(self, width=100, height=60)
        
        #Crea los circulos de cada LED
        self.cnvLedOn.create_oval(25,10,75,60, fill="#61605c")
        self.cnvLedOff.create_oval(25,10,75,60, fill="#FF0000")
        self.cnvLedAuto.create_oval(25,10,75,60, fill="#61605c")
        self.cnvLedManu.create_oval(25,10,75,60, fill="#61605c")
        self.cnvLedWater.create_oval(25,10,75,60, fill="#61605c")
        
        #Crear los Labels
        self.lblOn=tk.Label(self, text= "On")
        self.lblOff=tk.Label(self, text= "Off")
        self.lblAuto=tk.Label(self, text= "Automatic")
        self.lblManu=tk.Label(self, text= "Manual")
        self.lblWater=tk.Label(self, text= "Watering")

        #Agrega los labels al main
        self.lblOn.grid(column=0, row=0)
        self.lblOff.grid(column=1, row=0)
        self.lblAuto.grid(column=2, row=0)
        self.lblManu.grid(column=3, row=0)
        self.lblWater.grid(column=4, row=0)     
        
        #Agrega los LEDs al main
        self.cnvLedOn.grid(column=0, row=1)
        self.cnvLedOff.grid(column=1, row=1)
        self.cnvLedAuto.grid(column=2, row=1)
        self.cnvLedManu.grid(column=3, row=1)
        self.cnvLedWater.grid(column=4, row=1)


class MovementFrame(tk.Frame):
    def __init__(self,mainWindow):
        self.fontStyle = tkFont.Font(family="Lucida Grande", size=25)
        self.pixelVirtual = tk.PhotoImage(width=1, height=1)
        self.imageUp= tk.PhotoImage(file="images/ArrowUp.png")
        self.imageDown=tk.PhotoImage(file="images/ArrowDown.png")
        super().__init__(mainWindow.mainWindow)
        
        #Crear Botones 
        self.btnXup=tk.Button(self,
                              image= self.imageUp,
                              command=mainWindow.ClickXup)
        self.btnXdown=tk.Button(self,
                                image= self.imageDown,
                                command=mainWindow.ClickXdown)
        self.btnYup=tk.Button(self,
                              image= self.imageUp,
                              command=mainWindow.ClickYup)
        self.btnYdown=tk.Button(self,
                                image= self.imageDown,
                                command=mainWindow.ClickYdown)
        self.btnZup=tk.Button(self,
                              image= self.imageUp,
                              command=mainWindow.ClickZup)
        self.btnZdown=tk.Button(self,
                                image= self.imageDown,
                                command=mainWindow.ClickZdown)
        
        #Crear Labels
        self.lblX=tk.Label(self, text= "X Movement")
        self.lblY=tk.Label(self, text= "Y Movement")
        self.lblZ=tk.Label(self, text= "Z Movement")
        
        #Agregar botones al main
        
        self.lblX.grid(column=0, row=0)
        self.lblY.grid(column=1, row=0)
        self.lblZ.grid(column=2, row=0)
        self.btnXup.grid(column=0, row=1)
        self.btnXdown.grid(column=0, row=2)
        self.btnYup.grid(column=1, row=1)
        self.btnYdown.grid(column=1, row=2)
        self.btnZup.grid(column=2, row=1)
        self.btnZdown.grid(column=2, row=2)
        
        

class AutoManFrame(tk.Frame):
    def __init__(self,mainWindow):
        super().__init__(mainWindow.mainWindow)
        self.fontStyle = tkFont.Font(family="Lucida Grande", size=25)
        self.pixelVirtual = tk.PhotoImage(width=1, height=1)
        
        #Crea los botones del panel
        self.btnAuto=tk.Button(self,text="Automatic Mode",bg="black",fg="white",
                        height=70,
                        width=130,
                        image=self.pixelVirtual,
                        compound="c",
                        command=mainWindow.ClickAuto)
        self.btnManual=tk.Button(self,text="Manual Mode",bg="black",fg="white",
                         height=70,
                         width=130,
                         image=self.pixelVirtual,
                         compound="c",
                         command=mainWindow.ClickManual)
        self.btnAuto.grid(column=0, row=0)
        self.btnManual.grid(column=0, row=1)
    
        
        
    
    
   

#Crear Ventana Principal
mainWindow= Tk()
gui=MainInterface(mainWindow)
mainWindow.mainloop()




