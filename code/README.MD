Simple application for distributed file sharing among remote hosts.

Project description:

When launched, your process should work like a UNIX shell. It should accept incoming connections and at the same time provide a user interface that will offer the following command options:

1. HELP: Display information about the available user command options.

2. CREATOR: Display your (the students) full name, your UBIT name and UB email address.

3. DISPLAY: Display the IP address of this process, and the port on which this process is listening for
incoming connections.
NOTE: The IP should not be your “Local” address (127.0.0.1). It should be the actual IP of the host.

4. REGISTER <server IP> <port no>: This command is used by the client to register itself with the
server and to get the IP and listening port numbers of all other peers currently registered with the
server. The first task of every host is to register itself with the server by sending the server a TCP
message containing its listening port number. The server should maintain a list of the IP address and the
listening ports of all the registered clients. Let’s call this list as “Server-IP-List”. Whenever a new host
registers or a registered host exits, the server should update its Server-IP-List appropriately and then
send this updated list to all the registered clients. Hosts should always listen to such updates from the
server and update their own local copy of the available peers. Any such update which is received by the
host should be displayed by the client. The REGISTER command takes 2 arguments. The first
argument is the IP address of the server and the second argument is the listening port of the server. If
the host closes the TCP connection with the server for any reason then that host should be removed
from the “Server-IP-List” and the server should promptly inform all the remaining hosts.
NOTE: The REGISTER command works only on the client and not on the server. Registered clients
should always maintain a live TCP connection with the server.

5. CONNECT <destination> <port no>: This command is used to establish a connection between two
registered clients. The command establishes a new TCP connection to the specified <destination> at the
specified <port no>. The <destination> can either be an IP address or a hostname. (e.g., CONNECT
timberlake.cse.buffalo.edu 3456 or CONNECT 192.168.45.55 3456). The specified IP address should
be a valid IP address and listed in the Server-IP-List sent to the host by the server. Any attempt to
connect to an invalid IP or an IP address not listed by the server in its Server-IP-List should be rejected
and suitable error message displayed. Success or failure in connections between two peers should be
indicated by both the peers using suitable messages. Self-connections and duplicate connections should
be flagged with suitable error messages. Every client can maintain up-to 3 connections with its peers.
Any request for more than 3 connections should be rejected.

6. LIST: Display a numbered list of all the connections this process is part of. This numbered list will
include connections initiated by this process and connections initiated by other processes. The output
should display the hostname, IP address and the listening port of all the peers the process is connected
to. 

7. TERMINATE <connection id> This command will terminate the connection listed under the
specified number when LIST is used to display all connections. E.g., TERMINATE 2. In this example,
the connection with highgate should end. An error message is displayed if a valid connection does not
exist as number 2. If a remote machine terminates one of your connections, you should also display a
message.

8. QUIT Close all connections and terminate this process. When a host exits, the server unregisters the
host and sends the updated “Server-IP-List” to all the clients. Other hosts on receiving the updated list
from the server should display the updated list.

9. GET <connection id> <file> This command will download a file from one host specified in the
command.
E.g., if a command GET 2 file1 is entered for a process running on underground, then this process will
request file1 from highgate. The local machine will automatically accept the file and save it in the same
directory where your program is under the original name. When the download completes this process
will display a message indicating so. Also, the remote machine will display a message in its user
interface indicating that a file (e.g., a.txt) has been downloaded along with the hostname of the host
from which the file was downloaded. Upon completion, a success message is displayed. When a
download is occurring, a message should be displayed on the local machine. If the download fails for
some reason, an error message should be displayed on the remote machine and local machine, e.g., the
file that your local machine tries to download doesn't exist. Also the peer should serve files located in
your own directory to requesting peers. The peer should not serve files located in any other directory.
NOTE: GET command on a server should display an error message. No files should be downloaded
from the server.

10. PUT <connection id> <file name> For example, ‘PUT 3 /local/Fall_2016/lusu/a.txt’ will put the file
a.txt which is located in /local/Fall_2016/lusu/, to the host on the connection that has connection id 3.
An error message is displayed if the file was inaccessible or if 3 does not represent a valid connection
or this file doesn't exist. The remote machine will automatically accept the file and save it under the
original name in the same directory where your program is. When the upload is complete, this process
will display a message indicating so. Also, the remote machine will display a message in its user
interface indicating that a file (called a.txt) has been downloaded. When an upload is occurring, the user
interface of the uploading process will remain unavailable until the upload is complete. Upon
completion, a message is displayed. If the upload fails for some reason, an error message should be
displayed. When an upload is occurring, a message should be displayed on the remote machine when
the upload begins. If the upload fails for some reason, an error message should be displayed on the
remote machine. Also other peers should not be able to monopolize your resources, e.g., another peer
should not be able to fill up this peer's disk.
