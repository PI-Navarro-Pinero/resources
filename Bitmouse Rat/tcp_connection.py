#!/usr/bin/env python

import socket

connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # socket object
connection.connect(("10.0.2.5", 4444)) # connect the socket to a destination ([targetIP],[targetPORT])

connection.send("\n[+] Connection established.\n")

recieved_data = connection.recv(1024)
print(recieved_data)

connection.close()
