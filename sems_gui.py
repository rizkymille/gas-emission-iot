import tkinter as tk
import asyncio
import struct
import numpy as np

import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

from PIL import ImageTk, Image

from datetime import datetime

if not hasattr(Image, 'Resampling'):  # Pillow<9.0
   Image.Resampling = Image

MODE = "PROD" # "TEST" mode for DEBUGGING, "PROD" mode for implementation/production
SERVER = 'localhost'
PORT = 60000  # Port to listen on (non-privileged ports are > 1023)
echoDataCount = 0
file = None
KEEPDATA_THRESHOLD = 8

# hex to int decrypter
def hexToInt32(hex):
    return struct.unpack("!i", bytes.fromhex(hex))[0]

def parseMsg(msg):
    # slice data
    dev_addr = msg[:8]
    rssi = hexToInt32(msg[9:17])
    snr = hexToInt32(msg[17:25])/100
    payload = msg[25:len(msg)-3]
    decoded_payload = bytearray.fromhex(payload).decode()

    # slice message
    decoded_payload = decoded_payload.split('/')
    co2 = float(decoded_payload[0])
    co = float(decoded_payload[1])
    temp = float(decoded_payload[2])

    return np.array([co2, co, temp])

co2_arr = np.array([])
co_arr = np.array([])
timeNow_arr = np.array([])
temp_arr = np.array([])

class EchoServerProtocol(asyncio.Protocol):
    decodedMsg = None

    def connection_made(self, transport):
        '''
        peername = transport.get_extra_info('peername')
        print('Connection from {}'.format(peername))
        self.transport = transport
        '''
        pass

    def data_received(self, data):
        global echoDataCount, file
        global timeNow_arr, co2_arr, co_arr, temp_arr
        message = data.decode("utf-8")

        # testing only
        if MODE == "TEST":
            decodedMsg = message.split('/')
            for i in range(0, 2):
                decodedMsg[i] = float(decodedMsg[i])

            print(f"CO2: {decodedMsg[0]}ppm, CO: {decodedMsg[1]}ppm, TEMP: {decodedMsg[2]}°C")
        # implementation
        else:
            decodedMsg = parseMsg(message)

        co2 = decodedMsg[0]
        co = decodedMsg[1]
        temp = decodedMsg[2]
        timeNow = datetime.now().strftime("%H:%M:%S")

        co2_arr = np.append(co2_arr, co2)
        co_arr = np.append(co_arr, co)
        temp_arr = np.append(temp_arr, temp)
        timeNow_arr = np.append(timeNow_arr, timeNow)

        if np.size(timeNow_arr) > KEEPDATA_THRESHOLD:
            co2_arr = np.delete(co2_arr, 0)
            co_arr = np.delete(co_arr, 0)
            temp_arr = np.delete(temp_arr, 0)
            timeNow_arr = np.delete(timeNow_arr, 0)

            ax1.set_xlim(timeNow_arr[0], timeNow_arr[KEEPDATA_THRESHOLD-1])
            ax2.set_xlim(timeNow_arr[0], timeNow_arr[KEEPDATA_THRESHOLD-1])
            ax3.set_xlim(timeNow_arr[0], timeNow_arr[KEEPDATA_THRESHOLD-1])
            
        ax1.plot(timeNow_arr, co2_arr, 'r')
        ax2.plot(timeNow_arr, co_arr, 'b')
        ax3.plot(timeNow_arr, temp_arr, 'g')

        plt.autoscale()

        canvas.draw()

        if MODE != "TEST":
            # get time info for logger
            if (echoDataCount == 0):
                dateTimeNow = datetime.now().strftime("%d-%m-%Y_%H-%M-%S")
                file = open(f'SEMS_LOG_{dateTimeNow}.csv', 'w')
                file.write('Time,CO2,CO,TEMP\n')
                echoDataCount =+ 1
            else:
                file.write(f'{timeNow},{co2},{co},{temp}\n') # write to logger
        
        figure.canvas.flush_events()

async def update():
    while loop.is_running():
        root.update()
        await asyncio.sleep(0.01)

def quit(event):
    loop.stop()
    if MODE != "TEST":
        file.close()
    root.destroy()

if __name__ == '__main__':
    root = tk.Tk()
    root.title("Smart Emission Monitoring System Universitas Indonesia")
    root.geometry("900x700")
    root.configure(background='white')

    gbrUI = Image.open("Makara_of_Universitas_Indonesia.png")
    gbrUI = gbrUI.resize((75,75), Image.Resampling.LANCZOS)
    gbrUI = ImageTk.PhotoImage(gbrUI)

    header = tk.Label(root, 
                        text="Smart Emission Monitoring System Universitas Indonesia", 
                        image=gbrUI,
                        compound='left',
                        padx=20,
                        font=("Arial", 20),
                        anchor='center')
    header.grid(padx=40, sticky=tk.NSEW)
    header.configure(background='white')

    figure, (ax1, ax2, ax3, ax4) = plt.subplots(4,1)
    ax4.set_visible(False) # buffer axis because matplotlib is buggy when setting last axis limits
    figure.set_size_inches(5,8)
    figure.subplots_adjust(hspace=0.5)

    ax1.set_title("Kadar CO")
    ax2.set_title("Kadar CO2")
    ax3.set_title("Suhu")

    ax1.set_ylabel("ppm")
    ax2.set_ylabel("ppm")
    ax3.set_ylabel("°C")

    canvas = FigureCanvasTkAgg(figure, master=root)
    canvas.get_tk_widget().grid(row=1, ipadx=20, sticky=tk.NSEW)

    loop = asyncio.get_event_loop()
    tkTask = loop.create_task(update())
    tcpServer = loop.create_server(lambda: EchoServerProtocol(), SERVER, PORT)
    root.bind("<Escape>", quit)

    server = loop.run_until_complete(tcpServer)

    try:
        loop.run_forever()
    except KeyboardInterrupt:
        if MODE != "TEST":
            file.close()