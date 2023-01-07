import pandas as pd
import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from PIL import ImageTk, Image

if not hasattr(Image, 'Resampling'):  # Pillow<9.0
   Image.Resampling = Image
   
plt.style.use('fast')

csv_data = pd.read_csv('LOG_30-12-2022_17-54-08.csv')

x_vals = []
#CO
CO = csv_data['CO'].to_numpy()
#CO2
CO2 = csv_data['CO2'].to_numpy()
#TEMP
TEMP = csv_data['TEMP'].to_numpy()

y_vals = []
y_vals2 = []
y_vals3 = []


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

    ax1.plot(x_vals, y_vals,'r')
    ax2.plot(x_vals, y_vals2,'b')
    ax3.plot(x_vals, y_vals3,'g')

    ax1.set_title("Kadar CO")
    ax2.set_title("Kadar CO2")
    ax3.set_title("Suhu")

    ax1.set_ylabel("ppm")
    ax2.set_ylabel("ppm")
    ax3.set_ylabel("°C")

    xmin = max(i-5, 0)
    xmax = min(i+5, 10)
    
    ax1.set_xlim([xmin, xmax])
    ax2.set_xlim([xmin, xmax])
    ax3.set_xlim([xmin, xmax])

    ax1.set_ylim([max(CO[i]-10, 0), max(CO[i]+10, 10)])
    ax2.set_ylim([max(CO2[i]-10, 0), max(CO2[i]+10, 10)])
    ax3.set_ylim([max(TEMP[i]-5, 0), max(TEMP[i]+5, 30)])

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
    ani = FuncAnimation(plt.gcf(), animate, interval=1000, blit=False)
    root.mainloop()