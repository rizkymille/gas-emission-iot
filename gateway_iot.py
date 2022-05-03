import asyncio
import struct
import codecs

from datetime import datetime

import pandas as pd
import sys
import matplotlib.pyplot as plt

import collections
import numpy as np

PORT = 60000  # Port to listen on (non-privileged ports are > 1023)

co2_plots = None
co_plots = None
temp_plots = None

# hex to int decrypter
def hex_to_int32(hex):
    int = struct.unpack("!i", codecs.decode(hex, "hex"))
    return int[0]

'''
live updating plot function. not tested yet
def update_plots(data_plots, data, label):
    data_plots.popleft()
    data_plots.append(data)
    plt.plot(data_plots)
    plt.scatter(len(data_plots)-1, data_plots[-1])
    if label != "Temp":
        unit = "ppm"
    else:
        unit = "C"
    plt.text(len(data_plots)-1, data_plots[-1]+2, f"{data_plots[-1]}{unit} {label}")
'''

class EchoServerProtocol(asyncio.Protocol):

    # print incoming connection
    def connection_made(self, transport):
        peername = transport.get_extra_info('peername')
        print('Connection from {}'.format(peername))

    # print incoming data
    def data_received(self, data):
        message = data.decode()

        '''
        data 0-8: DEV_ADDR
        data 10-17: RSSI
        data 18-25: SNR
        data 25-etc: data
        '''
        # slice data
        dev_addr = message[:8]
        rssi = hex_to_int32(message[9:17])
        snr = hex_to_int32(message[17:25])/100
        payload = message[25:len(message)-3]
        decoded_payload = bytearray.fromhex(payload).decode()

        # slice message
        decoded_payload = decoded_payload.split(',')
        co2 = int(decoded_payload[0])/100
        co = int(decoded_payload[1])/100
        temp = int(decoded_payload[2])/100

        # get time info for logger
        datetime_obj = datetime.now()
        time = datetime_obj.strftime("%H:%M:%S")      

        # print data to terminal
        print(f"Dev ADDR: {dev_addr}, RSSI: {rssi}, SNR: {snr}")
        print(f"CO2: {co2}ppm, CO: {co}ppm, Temperature: {temp} Celsius")
        
        file.write(f'{time},{co2},{co},{temp}\n') # write to logger

        '''
        live plotting, not used yet

        plt.cla()

        update_plots(co2_plots, co2, "CO2")
        update_plots(co_plots, co, "CO")
        update_plots(temp_plots, temp, "Temp")

        plt.pause(0.1)
        '''
        
async def main():
    # create asyncio loop object
    loop = asyncio.get_running_loop()

    # logger data title
    file.write('Time,CO2,CO,TEMP\n')

    '''
    live plotting, not used yet
    global co2_plots, co_plots, temp_plots

    co2_plots = collections.deque(np.zeros(10))
    co_plots = collections.deque(np.zeros(10))
    temp_plots = collections.deque(np.zeros(10))
    '''
    
    # create async object
    server = await loop.create_server(lambda: EchoServerProtocol(), '', PORT)

    async with server:
        # make async object loop forever
        await server.serve_forever()

    # plt.show()

if __name__ == '__main__':
    try:
        datetime_obj = datetime.now()
        date_time = datetime_obj.strftime("%d-%m-%Y_%H-%M-%S")        
        file = open(f'LOG_{date_time}.csv', 'w')
        asyncio.run(main())
    # show graph when exiting
    except KeyboardInterrupt:
        try:
            file.close()
            print('Logging stopped')
            data = pd.read_csv(f'LOG_{date_time}.csv')
            data.plot()
            plt.show()
        # finally exit program
        except KeyboardInterrupt:
            sys.exit(0)   


        
