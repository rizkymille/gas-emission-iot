import asyncio
import struct
import codecs

from datetime import datetime

PORT = 60000  # Port to listen on (non-privileged ports are > 1023)

def hex_to_int32(hex):
    int = struct.unpack("!i", codecs.decode(hex, "hex"))
    return int[0]

class EchoServerProtocol(asyncio.Protocol):

    def connection_made(self, transport):
        peername = transport.get_extra_info('peername')
        print('Connection from {}'.format(peername))
        self.transport = transport

    def data_received(self, data):
        message = data.decode()
        #print('Data received: {!r}'.format(message))
        '''
        data 0-8: DEV_ADDR
        data 10-17: RSSI
        data 18-25: SNR
        data 25-etc: data
        '''
        
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

        datetime_obj = datetime.now()
        time = datetime_obj.strftime("%H:%M:%S")      

        print(f"Dev ADDR: {dev_addr}, RSSI: {rssi}, SNR: {snr}")
        print(f"CO2: {co2}ppm, CO: {co}ppm, Temperature: {temp} Celsius")
        file.write(f'{time},{co2},{co},{temp}\n')

async def main():
    # Get a reference to the event loop as we plan to use
    # low-level APIs.
    loop = asyncio.get_running_loop()

    file.write('Time,CO2,CO,TEMP\n')

    server = await loop.create_server(lambda: EchoServerProtocol(), '', PORT)

    async with server:
        await server.serve_forever()
        
    file.close()

datetime_obj = datetime.now()
date_time = datetime_obj.strftime("%d-%m-%Y_%H-%M-%S")        
file = open(f'LOG_{date_time}.csv', 'w')
asyncio.run(main())