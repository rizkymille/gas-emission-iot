import tkinter as tk
import asyncio
import sys
import numpy as np
from datetime import datetime
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from PIL import ImageTk, Image

PORT = 60000  # Port to listen on (non-privileged ports are > 1023)

msg_cnt = 0
index = 0
co2_plots = np.array([])
co_plots = np.array([])
temp_plots = np.array([])
x_plots = np.array([])

if not hasattr(Image, 'Resampling'):  # Pillow<9.0
   Image.Resampling = Image
   
plt.style.use('fast')

'''
def hex_to_int32(hex):
    return struct.unpack("!i", bytes.fromhex(hex))[0]
'''

def update_plots(data_plots, data, axes, color):
    np.append(data_plots, data)

    axes.plot(x_plots, data, color)

    axes.set_ylim([0, max(data+5, 30)])

async def handleData(reader):
    data = await reader.read()
    message = data.decode()

    '''
    data 0-8: DEV_ADDR
    data 10-17: RSSI
    data 18-25: SNR
    data 25-etc: data (CO:float/CO2:float/TEMP:float)
    '''
    # slice data
    dev_addr = message[:8]
    decoded_payload = bytearray.fromhex(message[25:len(message)-3]).decode()

    global msg_cnt
    if msg_cnt == 0:
        datetime_obj = datetime.now()
        date_time = datetime_obj.strftime("%d-%m-%Y_%H-%M-%S")        
        file = open(f'LOG_{date_time}.csv', 'w')
        msg_cnt += 1

    # slice message
    decoded_payload = decoded_payload.split('/')
    co2 = decoded_payload[0]
    co = decoded_payload[1]
    temp = decoded_payload[2]

    # get time info for logger
    datetime_obj = datetime.now()
    time = datetime_obj.strftime("%H:%M:%S")      

    # print data to terminal
    #print(f"Dev ADDR: {dev_addr}")
    #print(f"CO2: {co2}ppm, CO: {co}ppm, Temp: {temp}°C")
    
    file.write(f'{time},{co2},{co},{temp}\n') # write to logger

    plt.cla()

    global index, co_plots, co2_plots, temp_plots
    
    if (index > 10): 
        np.delete(co_plots, 0)
        np.delete(co2_plots, 0)
        np.delete(temp_plots, 0)
    
    np.append(x_plots, index)
    update_plots(co_plots, co, ax1, 'r')
    update_plots(co2_plots, co2, ax2, 'b')
    update_plots(temp_plots, temp, ax3, 'g')

    ax1.set_xlim([max(index-10, 0), index])
    ax2.set_xlim([max(index-10, 0), index])
    ax3.set_xlim([max(index-10, 0), index])
    
    # print(CO[i], CO2[i], TEMP[i])
    index += index

    plt.pause(0.1)
    root.update()


def draw_init(ax1, ax2, ax3):
    ax1.set_title("Kadar CO")
    ax2.set_title("Kadar CO2")
    ax3.set_title("Suhu")

    ax1.set_ylabel("ppm")
    ax2.set_ylabel("ppm")
    ax3.set_ylabel("°C")

async def _asyncio_thread():
    server = await asyncio.start_server(handleData, '', PORT)

    #addr = server.sockets[0].getsockname()
    #print(f'Serving on {addr}')

    async with server:
        await server.serve_forever()


def quit():
    print("exiting...")
    root.quit()

# Main window n GUI
if __name__ == "__main__" :
    root = tk.Tk()
    root.title("Sensor GUI Prototype")
    root.geometry("880x700")

    gbrUI = Image.open("Makara_of_Universitas_Indonesia.png")
    gbrUI = gbrUI.resize((75,75), Image.Resampling.LANCZOS)
    gbrUI = ImageTk.PhotoImage(gbrUI)

    root.bind('<Escape>', quit)

    root.columnconfigure(0, weight=1, minsize=20)
    root.rowconfigure(1, weight=1, minsize=20)
    root.configure(background='white')

    labeltxt = ttk.Label(root, 
                        text="Smart Emission Monitoring System Universitas Indonesia", 
                        image=gbrUI,
                        compound='left',
                        font=("Arial", 20),
                        anchor='w')
    labeltxt.grid(column=1, row=0, padx=40, pady=5, sticky=tk.W)
    labeltxt.configure(background='white')

    canvas = FigureCanvasTkAgg(plt.gcf(), master=root)
    canvas.get_tk_widget().grid(column=0, row=1,ipadx=20, ipady=10,columnspan=3, sticky=tk.NSEW)

    plt.gcf().subplots(3,1)
    plt.subplots_adjust(hspace=0.5)
    ax1, ax2, ax3 = plt.gcf().get_axes()
    draw_init(ax1, ax2, ax3)

    root.update()
    try:
        asyncio.run(_asyncio_thread())
    except KeyboardInterrupt:
        root.quit()
        sys.exit(0)