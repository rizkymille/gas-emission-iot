import pandas as pd
import tkinter as tk
import numpy as np
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from PIL import ImageTk, Image

if not hasattr(Image, 'Resampling'):  # Pillow<9.0
   Image.Resampling = Image
   
plt.style.use('fast')

csv_data = pd.read_csv('LOG_07-01-2023_16-16-36.csv')

x_vals = []
#CO
CO = csv_data['CO'].to_numpy()
print(CO)
CO[np.isnan(CO)] = 0
CO[np.isinf(CO)] = 0
#CO2
CO2 = csv_data['CO2'].to_numpy()
CO2[np.isnan(CO2)] = 0
CO2[np.isinf(CO2)] = 0
#TEMP
TEMP = csv_data['TEMP'].to_numpy()

y_vals = []
y_vals2 = []
y_vals3 = []

SECS_TO_MS = 1000


def animate(i):
    x_vals.append(i)
    try:
        y_vals.append(CO[i])
    except IndexError:
        return

    y_vals2.append(CO2[i])
    y_vals3.append(TEMP[i])
    
    # print(CO[i], CO2[i], TEMP[i])
    
    ax1, ax2, ax3 = plt.gcf().get_axes()

    plt.cla()

    ax1.set_title("Kadar CO")
    ax2.set_title("Kadar CO2")
    ax3.set_title("Suhu")

    ax1.set_ylabel("ppm")
    ax2.set_ylabel("ppm")
    ax3.set_ylabel("°C")

    xmin = max(i-10, 0)
    xmax = max(i, 10)
    
    ax1.set_xlim([xmin, xmax])
    ax2.set_xlim([xmin, xmax])
    ax3.set_xlim([xmin, xmax])

    ax1.plot(x_vals, y_vals,'r')
    ax2.plot(x_vals, y_vals2,'b')
    ax3.plot(x_vals, y_vals3,'g')

    plt.autoscale()
    '''
    ax1.set_ylim([max(np.amin(CO[max(i-10,0):i]), 0), max(np.amax(CO[max(i-10,0):i]), 10)])
    ax2.set_ylim([max(np.amin(CO2[max(i-10,0):i]), 0), max(np.amax(CO2[max(i-10,0):i]), 10)])
    ax3.set_ylim([max(np.amin(TEMP[i-10:i]), 0), max(np.amax(TEMP[max(i-10,0):i]), 30)])
    '''

def quit(event):
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
    #ani = FuncAnimation(plt.gcf(), animate, interval=2*SECS_TO_MS, blit=False)
    root.mainloop()