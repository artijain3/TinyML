from bleak import BleakScanner, BleakClient
import csv, time
import struct
import asyncio
import os

directory = "/Users/artijain/Documents/18-848EmbeddedDL/TinyML" #TODO: change these to command line arguments later
filename = "reading.csv" #TODO: change these to command line arguments later

async def main():
    # Step 1: Scan for devices
    csv_file = open(os.path.join(directory, filename), mode='w', newline='')
    csv_writer = csv.writer(csv_file)
    csv_writer.writerow(['timestamp', 'accX', 'accY', 'accZ', 'gyrX', 'gyrY', 'gyrZ'])
    
    devices = await BleakScanner.discover()
    # for d in devices:
    #     print(d)

    # Step 2: Replace with your Arduino's name or address
    address = "456F4137-6A67-C601-C29C-4F56F64D353D"
    
    async with BleakClient(address) as client:
        print(f"Connected to {address}!")
        
        # Step 3: Replace with your characteristic UUID
        characteristic_uuid = "2A37"
        
        start_time = time.time()
        
        
        # Read or subscribe to the characteristic
        def callback(sender, data):
            current_time = time.time()
            timestamp = (current_time - start_time) * 1000 # in milliseconds
            ax, ay, az, gx, gy, gz = struct.unpack('ffffff', data)
            csv_writer.writerow([timestamp, ax, ay, az, gx, gy, gz])
            print(f"Data written to CSV: {timestamp}, {ax}, {ay}, {az}, {gx}, {gy}, {gz}")
            
            # received_data = data.decode('utf-8')
            # print(f"Received: {received_data}")
            

        await client.start_notify(characteristic_uuid, callback)

        try:
            while True:
                await asyncio.sleep(1)  # Listen for 1 seconds
        except asyncio.CancelledError:
            print("Stopping...")
            csv_file.close()
            print(f"File saved to {os.path.join(directory, filename)}")
        
        await client.stop_notify(characteristic_uuid)

asyncio.run(main())
