# LittleChatRoom
A messaging application based on client-server model.
Anton Nefedenkov 
ain2108

Code Description:

First of all, my code does have some bugs. 
But most of them are fixed if the client logs out and logs back in.
The reading from pipes sometimes gets a little weird. 

Entirety of the code is written in C. It wasn't that bad of an idea, 
but writing the interpreter was a real pain. 
In terms of design, client is a brain dead program. It estabilishes a connection to 
the server and than forks. One process continously reads from the socket and 
dumps the output to stdout. The other process attempts to read commands 
from stdin and push them into the socket.

Client will only accept an <IP_ADDRESS>, like in the instructions. It is very easy to break the
client, the code is bad. Use command ifconfig to retrieve the ip adress of the server machine. 

Server is a good bit more complicated. To keep track of ip addresses, server
maintains a database of IP's and the time that the connection was attempted.
Usernames and passwords are also loaded into a separate database file. Processes
seek on this file and apply changes to the status of users (eg. logged in or not)

Every registered user has an associated named pipe, in which other processes can dump messages.

After forking for parallel connection, the process handling the client will fork again. The child
will be responsible of continously reading from user's FIFO and dumping data into the socket.
The parent will accept the commands from the users and run the interpreter. 

The timeout was implemented using a timebomb. If parent does not kill the child after certain amount
of seconds, the child will kill the parent and exit, effectively closing the connection with the user. 
Please notice, the due to time constraints, time out can only occur if the user is authenticated. 
I have no energy now to fix this hole.

Server reports connection status to stderr.


Development enviroment: emacs -nw
 

Instructions: 

client:
	make
	cd client/
	./Client <IP_ADRESS> <PORT>

server:
	make
	cd server/
	./Server <PORT>


Sample:
Run on the machine that will be the server, say @paris
	$ make
	$ cd server
	$ ./Server 8888

Go to the client machine, say @brussels
        $ make
	$ cd client
	$ ./Client 128.59.15.32 8888

From here on type commands and have fun. Server's interpreter can tolerate
some amount of mistakes in commands, but I believe there are still simple
ways how to crush it. (You can easily bufferoverflow 2048 bytes)

Additional functionalities:
	   none.
	
