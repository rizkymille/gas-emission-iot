import socket
import tkinter as tk
import threading
import struct

import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

from PIL import ImageTk, Image

from datetime import datetime

if not hasattr(Image, 'Resampling'):  # Pillow<9.0
   Image.Resampling = Image

MODE = "TEST"
PORT = 60000  # Port to listen on (non-privileged ports are > 1023)
echoDataCount = 0

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
    co2 = decoded_payload[0]
    co = decoded_payload[1]
    temp = decoded_payload[2]

    return co2, co, temp

def listenData():
    global echoDataCount
    #SERVER = socket.gethostbyname(socket.gethostname())

    SERVER = 'localhost'

    ADDRESS = (SERVER, PORT)

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    server.bind(ADDRESS)

    server.listen()

    file = None

    while not stop_event.wait(1):
        conn, _ = server.accept()
        message = conn.recv(1024).decode("utf-8")
        # testing only
        if MODE == "TEST":
            decodedMsg = message.split('/')
            co2 = decodedMsg[0]
            co = decodedMsg[1]
            temp = decodedMsg[2]
            print(f"CO2: {co2}ppm, CO: {co}ppm, TEMP: {temp}°C")
        # implementation
        else:
            co2, co, temp = parseMsg(message)

        if MODE != "TEST":
            # get time info for logger
            if (echoDataCount == 0):
                nowDateTime = datetime.now().strftime("%d-%m-%Y_%H-%M-%S")
                file = open(f'SEMS_LOG_{nowDateTime}.csv', 'w')
                file.write('Time,CO2,CO,TEMP\n')
                echoDataCount =+ 1
            else:
                timeNow = datetime.now().strftime("%H:%M:%S")
                file.write(f'{timeNow},{co2},{co},{temp}\n') # write to logger

    if MODE != "TEST":
        file.close()



def quit(event):
    print("exiting...")
    stop_event.set()
    root.quit()

if __name__ == '__main__':
    root = tk.Tk()
    root.title("Smart Emission Monitoring System Universitas Indonesia")
    root.geometry("880x700")

    gbrUI = Image.open("Makara_of_Universitas_Indonesia.png")
    gbrUI = gbrUI.resize((75,75), Image.Resampling.LANCZOS)
    gbrUI = ImageTk.PhotoImage(gbrUI)

    root.columnconfigure(0, weight=1, minsize=20)
    root.rowconfigure(1, weight=1, minsize=20)
    root.configure(background='white')

    labeltxt = tk.Label(root, 
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

    stop_event = threading.Event()

    thread = threading.Thread(target=listenData)
    thread.daemon = True

    root.bind("<Escape>", quit)

    thread.start()

    root.mainloop()
        
