import serial 
import sys
import os
import time

FRAME_SIZE = 4

def crc32mpeg2(buf, crc=0xffffffff):
    for val in buf:
        crc ^= val << 24
        for _ in range(8):
            crc = crc << 1 if (crc & 0x80000000) == 0 else (crc << 1) ^ 0x104c11db7
    return crc

'''
Start update via uart
    Parameters:
        ser(serial handler)
        filename(binary file name)
    Returns: 
        nothing      
'''
def send_upd_init(ser, filename): 
    fileSize = os.path.getsize(filename)
    fileContent = open(filename,'rb').read()
    fileCrc = crc32mpeg2(fileContent)
    message = str("update " + str(fileSize)+ " " + str(fileCrc) + "\r")
    ser.write(message.encode('ascii'))

'''
Sends update message
    Parameters:
        ser(serial handler)
        file(file handle)
        packnum(int)
        filezise(int)
    Returns: 
        nothing      
'''
def send_upd_packet(ser, file, packnum, filesize):
    #initial message contents
    message = bytearray(b"\x00\x00\x00\x00")

    #set current position in file
    file.seek(packnum * FRAME_SIZE )

    #if filesize is bigger than frame size
    #sending maximum size message, otherwise
    #seinding what is left
    if filesize > FRAME_SIZE:
        message = file.read(FRAME_SIZE)
        filesize -= FRAME_SIZE
    else:
        message[10:] = file.read(filesize)
        filesize = 0
   
    ser.write(message)

'''
Receives message and send echo
    Parameters:
        device(str)
        baudrate(str)
        size(int)       
'''   
def receive_response(ser):
    received_message = ser.read()
    if received_message != b'y':
        print(received_message)
        print("Flash write fail")
        ser.close()
        sys.exit(rc)

'''
Transmits data between two uarts
    Parameters:
        device_1(str)
        filepath(str)
    Returns: 
        int        
''' 
 
if __name__ == "__main__":

    # parse arguments
    args = str(sys.argv[1:])
    args = args.replace("[","")
    args = args.replace("]","")
    args = args.replace(",","")
    args = args.replace("'","")
    args = args.split()

    rc = 0
    device_1 = args[0]
    baudrate = int(115200)
    timeout_uart = float(0.1)
    filename = args[1]
    binFile = open(filename, "rb")
    filesize = os.path.getsize(filename)
    print("Size of file: " + str(filesize))
    ser = serial.Serial(device_1, baudrate, timeout=timeout_uart)

    send_upd_init(ser,filename)
    time.sleep(1)
    if (filesize % (FRAME_SIZE)) == 0:
        packets_to_tx = int(filesize / (FRAME_SIZE))
    else:
        packets_to_tx = int(filesize / (FRAME_SIZE)) + 1
    packets_tx = int(0)
 
    for pack in range(packets_to_tx):
        packets_tx = packets_tx + 1
        print("Frames sent", packets_tx, "out of", packets_to_tx) 
        send_upd_packet(ser, binFile, pack, filesize)
        receive_response(ser)
        time.sleep(0.001)
    binFile.close()
    ser.close()
    sys.exit(rc)
