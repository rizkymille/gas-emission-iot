import asyncio
import struct
import codecs

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
        print(message[:len(message)-3])
        print(f"Dev ADDR: {dev_addr}, RSSI: {rssi}, SNR: {snr}")
        print(f"Payload: {payload}")

async def main():
    # Get a reference to the event loop as we plan to use
    # low-level APIs.
    loop = asyncio.get_running_loop()

    server = await loop.create_server(lambda: EchoServerProtocol(), '', PORT)

    async with server:
        await server.serve_forever()


asyncio.run(main())