import pandas as ps
import struct
from datetime import datetime
import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from PIL import ImageTk, Image

if not hasattr(Image, 'Resampling'):  # Pillow<9.0
   Image.Resampling = Image
   
plt.style.use('fast')

PORT = 60000  # Port to listen on (non-privileged ports are > 1023)

co2_plots = None
co_plots = None
temp_plots = None

# hex to int decrypter
def hex_to_int32(hex):
    int = struct.unpack("!i", bytes.fromhex(hex))[0]
    return int

x_vals = []
y_vals = []
y_vals2 = []
y_vals3 = []


def animate(i):
    x_vals.append(i)
    y_vals.append(CO[i])
    y_vals2.append(CO2[i])
    y_vals3.append(TEMP[i])
    
    print(CO[i], CO2[i], TEMP[i])
    
    ax1, ax2, ax3 = plt.gcf().get_axes()

    ax1.clear()
    ax2.clear()
    ax3.clear()
    
    ax1.plot(x_vals, y_vals,'r')
    ax2.plot(x_vals, y_vals2,'b')
    ax3.plot(x_vals, y_vals3,'g')
    
    ax1.set_xlim([i-5,i+5])
    ax2.set_xlim([i-5,i+5])
    ax3.set_xlim([i-5,i+5])
    ax1.set_ylim([0,3])
    ax2.set_ylim([0,3])
    ax3.set_ylim([0,40])


# Main window n GUI
if __name__ == "__main__" :
    datetime_obj = datetime.now()
    date_time = datetime_obj.strftime("%d-%m-%Y_%H-%M")    

    # write log data    
    file = open(f'LOG_{date_time}.csv', 'w')

    root = tk.Tk()
    root.title("Sensor GUI Prototype")
    root.geometry("880x700")

    gbrUI = Image.open("Makara_of_Universitas_Indonesia.png")
    gbrUI = gbrUI.resize((75,75), Image.Resampling.LANCZOS)
    gbrUI = ImageTk.PhotoImage(gbrUI)

    root.columnconfigure(0, weight=1, minsize=20)
    root.rowconfigure(1, weight=1, minsize=20)
    root.configure(background='white')

    label = ttk.Label(root, image=gbrUI)
    label.grid(column=0, row=0, sticky=tk.W, padx=30, pady=30)
    label.image = gbrUI
    label.configure(background='white')

    label2 = ttk.Label(root, text="Smart Emission Monitoring System Universitas Indonesia",font=("Arial", 20),anchor='w')
    label2.grid(column=1, row=0, padx=20, pady=5, sticky=tk.W)
    label2.configure(background='white')

    canvas = FigureCanvasTkAgg(plt.gcf(), master=root)
    canvas.get_tk_widget().grid(column=0, row=1,ipadx=20, ipady=10,columnspan=3, sticky=tk.NSEW)

    plt.gcf().subplots(3,1)
    ani = FuncAnimation(plt.gcf(), animate, interval=1000, blit=False)

    try:
        root.mainloop()
    except KeyboardInterrupt:
        sys.exit(0)