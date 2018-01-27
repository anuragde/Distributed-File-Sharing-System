/*
 * anuragde_proj1.cpp
 *
 *  Created on: Sep 29, 2016
 *      Author: anurag
 	References: Beej Guide and CSE 589 Recitation Slides
 	http://stackoverflow.com/questions/347949/how-to-convert-a-stdstring-to-const-char-or-char
 	Note: Put and Get functions are not implemented.
 */

#include "string"
#include "cstring"
#include "iostream"
#include "cstdlib"
#include "netdb.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <errno.h>
#include <stdlib.h>
#include <sys/select.h>

#define BACKLOG 10
#define STDIN_DESC 0 	
#include <ifaddrs.h>
using namespace std;;
#define MAXDATASIZE 1000 
int i=0;
char buf[MAXDATASIZE];
char *numClients1;
char* l_port_no;
string task1;
string task2[4];

struct server_ip_list {
		int client_status;
		int client_id;
		char client_name[100];
		char client_ip[100];
		char client_port[100];
		int client_sockid;
	};
	struct server_ip_list serverList[5];//As only 5 servers exist ,better not to hardcode
	struct server_ip_list client_list[5];
	struct server_ip_list serverListCopy[5];
string to_string(int i)
{
    stringstream ss;
    ss << i;
    return ss.str();
}

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
	return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


//Common Functions for Client and Server
int clientHelp()
{
	cout<<"\nThis is a distributed file sharing system application.The available commands for the user are as below:" <<endl;
	cout<<"CREATOR : Displays name, UBIT name and Email Id of the developer."<<endl;
	cout<<"DISPLAY : Displays IP address and port number of current system."<<endl;
	cout<<"QUIT : Close all connections and terminate current process."<<endl;
	cout<<"LIST : Displays list of current live connections"<<endl;
    cout<<"REGISTER <IP ADDRESS> <PORT> : Register with server at <IP ADDRESS> running on port <PORT>"<<endl;
    cout<<"CONNECT <DESTINATION> <PORT> : Connect to given destination <DESTINATION> server running on port <PORT>"<<endl;
    cout<<"GET <ID> <FILENAME> : Get file <filename> from server at given connection ID\n"<<endl;
    cout<<"PUT <ID> <FILENAME> : Send file <filename> to server at given connection ID\n"<<endl;
    cout<<"TERMINATE <ID> : Terminate connection with given connection ID\n"<<endl;
    cout<<"QUIT : Close all connections and terminates the current process\n"<<endl;
}



int serverHelp()
{
	cout<<"\nThis is a distributed file sharing system application.The available commands for the user are as below:"<<endl;
	cout<<"CREATOR : Displays name, UBIT name and Email Id of the developer"<<endl;
	cout<<"DISPLAY : Displays IP address and port number of current system"<<endl;
	cout<<"QUIT : Close all connections and terminates the current process"<<endl;
	return 0;
}

int creator()
{
	cout<<"\nNAME: ANURAG DEVULAPALLI\nUBIT NAME: ANURAGDE\nEMAIL ID: ANURAGDE@BUFFALO.EDU"<<endl;
	return 0;
}

int serverQuit()
{
    
}
//Checked below code


int display()
{
	struct ifaddrs *myaddress, *ifa;
	void *in_addr;
	char buf[64];
	char buf1[64]="127.0.0.1";
	if(getifaddrs(&myaddress) !=0)
	{
		cout<<"getifaddrss";
		exit(1);
	}

	for (ifa = myaddress; ifa != NULL; ifa = ifa->ifa_next)
	    {
	        if (ifa->ifa_addr == NULL)
	            continue;
	      
	       if((ifa->ifa_addr->sa_family)==AF_INET)
	        {
	            	struct sockaddr_in *s4 = (struct sockaddr_in *)ifa->ifa_addr;
	                in_addr = &s4->sin_addr;
	        }

	        else if ((ifa->ifa_addr->sa_family)==AF_INET6)
	            {
	                struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)ifa->ifa_addr;
	                in_addr = &s6->sin6_addr;
	            }

	     

	        if (!inet_ntop(ifa->ifa_addr->sa_family, in_addr, buf, sizeof(buf)))
	        {
	          //  cout<<"inet_ntop failed." << ifa->ifa_name;
	        }
	        else if(((ifa->ifa_addr->sa_family)==AF_INET) && (strcmp(buf,buf1))!=0)
	        {
	        	cout<<"Port Number:"<<l_port_no<<endl;
	            cout<<"IP Address:"<<buf<<endl;
	        }
	        else
	        {
	        	        }
	     }

	    freeifaddrs(myaddress);
	    return 0;
}
//Checked above code

fd_set master;    // master file descriptor list
fd_set read_fds;  // temp file descriptor list for select()
int fdmax;        // maximum file descriptor number
int sockfd,fd,new_fd;
struct sockaddr *cl_addr;
char  hostname[100];
char  service[20];


void startServer()//previously connect
{
	struct addrinfo hints,*res,*p;
	struct sockaddr_storage their_addr;
	char client_ip_addr[INET6_ADDRSTRLEN];
	int rv;
	int a=1;	
	memset(&hints,0,sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;	 // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;
	char client_port_no[MAXDATASIZE];
	
	char  service[20];
// <---------- GET ADDRESS INFO ------------>
	if ((rv = getaddrinfo(NULL,l_port_no, &hints, &res)) != 0) {
	cerr<<"getaddrinfo: "<<gai_strerror(rv)<<endl;
	return;
	}

// <---------- SOCKET & BIND ------------>	
	
	for(p = res; p != NULL; p = p->ai_next) 
	{
		sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol);
		if (sockfd== -1) {
		cerr<<"server: socket error"<<endl;
		continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
		close(sockfd);
		cerr<<"server: bind error"<<endl;
		continue;
		}
		break;
	}
	freeaddrinfo(res);

	if (p == NULL) {
	cerr<<"server: failed to bind\n";
	exit(1);
	}

// <---------- LISTEN ------------>
	if (listen(sockfd, BACKLOG) == -1) {
	cerr<<"listen"<<endl;
	exit(1);
	}
   
    return;
   } 
// <---------- END OF Start Server ------------>

char peer_no[600];
char pichle[600];


void registerAtServer(char data[100], struct server_ip_list serverList[5], int numClients)
	{
		char *temp;
		int counter=1;
//Reference strtok slides
		temp = strtok(data,"|");

		while( temp != NULL )
		{

			if(counter==2)
			strcpy(serverList[numClients].client_port,temp);
			counter =counter+ 1;
			temp = strtok(NULL,"|");		
		}
			serverList[numClients].client_id=(numClients+1);
			socklen_t len;
			struct sockaddr_storage addr;
			len = sizeof addr;
			//http://man7.org/linux/man-pages/man2/getpeername.2.html
			getpeername(serverList[numClients].client_sockid, (struct sockaddr*)&addr, &(len));

			if (addr.ss_family == AF_INET) {
				struct sockaddr_in *s = (struct sockaddr_in *)&addr;
				inet_ntop(AF_INET, &s->sin_addr, serverList[numClients].client_ip, sizeof serverList[numClients].client_ip);
			}

		//	inet_pton(AF_INET, serverList[numClients].client_ip, &cl_addr);
			//inet_ntop(cl_addr->sa_family,get_in_addr((struct sockaddr *)cl_addr),client_ip_addr, sizeof client_ip_addr);
		//	getnameinfo(cl_addr,sizeof cl_addr,hostname,100,service,100,0);
		//	strpy(serverList[numClients].client_name,hostname);
	   
	    return ;
	}


int nbytes;
	socklen_t sin_size;

//Read commands from user and perform task for server
int server_program()
{
	int numClients=0;
	char client_data[MAXDATASIZE];
	//initialise serveriplist
	for (int k=0;k<5;k++)	
	{
		serverList[k].client_sockid=-1;
		serverList[k].client_status=0;
	}
	startServer();
	FD_ZERO(&master);   
    FD_SET(STDIN_DESC, &master);
    FD_SET(sockfd, &master);
 	fdmax=sockfd;
	cout<<"[PA1]$";	

	for(;;)
	{
	/*	cout<<fdmax<<endl;	
	cout<<sockfd<<endl;
		cout<<fd<<endl;

		cout<<"for"<<endl;*/
		read_fds=master;

		if(select(fdmax+1,&read_fds,NULL,NULL,NULL)<0)
			{
				cerr<<"Select"<<endl;
				return -1;	
			}
		else{
		for(fd=0;fd<=fdmax;fd++)
		{
			if(FD_ISSET(fd,&read_fds))
			{
				if(fd == STDIN_DESC)
				{
					getline(cin,task1);
				    istringstream iss(task1);
				    int i = 0;
				    do
				    {
				        string sub;
				        iss >> sub;
				    	task2[i]=sub;
				        i++;
				    } while (iss);

				    if (task2[0]=="HELP")
						serverHelp();
					else if (task2[0]=="CREATOR")
						creator();
				 	else if (task2[0]=="DISPLAY")
						display();
					else if (task2[0]=="QUIT")
					{
						int k;
					    for(k = 0;k < numClients ; k++){
					        close(k);
					    }                        
						cout<<"All connections closed.Existing."<<endl;
						exit(0);
					}	

					else
						cout<<"Invalid Command by user. Please type HELP command for available commands."<<endl;
				}
				
				else if(fd==sockfd)
				{
										cout<<"Received connection from client "<<endl;

					sin_size = sizeof(struct sockaddr_in);

					new_fd = accept(sockfd, (struct sockaddr *)cl_addr, &sin_size);

					if (new_fd == -1) 
					{
						cerr<<"accept error"<<endl;
					}
					
					else
					{
						FD_SET(new_fd, &master); // add to master set
                        if (new_fd > fdmax) 
							fdmax = new_fd;// keep track of the max
                  		
                  		//inet_ntop(cl_addr->sa_family,get_in_addr((struct sockaddr *)cl_addr),client_ip_addr, sizeof client_ip_addr);
                    	//cout<<"server: got connection from "<<client_ip_addr<<endl;
                    	//getnameinfo(cl_addr,sizeof client_ip_addr,hostname,100,service,100,0);
                    	
                    	serverList[numClients].client_sockid=new_fd;


                  		if ((nbytes=recv(new_fd,client_data,MAXDATASIZE-1,0)) <= 0)
                  		{
						cout<<"receive error";
						}
                        else
                        {
			    	                        if((strstr(client_data,"REGISTER")!=NULL))
	                        {
	                        			 registerAtServer(client_data, serverList, numClients);


												strcpy(pichle, "NULL");
												//cout<<numClients<<endl;
												numClients += 1;

											 //Send updated server_ip_list to all connected clients
												for(int i=0; i < numClients; i++)
												{
												string t=to_string(serverList[i].client_id);
												//string str1(serverList[i].client_name);
												string str2(serverList[i].client_ip);
												string str3(serverList[i].client_port);
												string master1=t+"|"+"highgate"+"|"+str2+"|"+str3+";";
												const char *x=master1.c_str();
												strcpy(peer_no,x);

												if(strcmp(pichle,"NULL")==0)
												    strcpy(pichle,peer_no);
												else
												    strcat(pichle, peer_no);
												}
												for(int j=0; j < numClients;j++)
												{
													if (send(serverList[j].client_sockid,pichle, strlen(pichle), 0) < 0)
												    cout<<"send"<<endl;	
											    }


	                        	cout<<"Number of clients registered "<<numClients<<endl;
	                        }
	                            
	                    	new_fd = -1;
		   				}

                           
                       	}
                    }
                
                              	
                    //<-------------->new_fd is not modified hence receiving data from existing connection-------------------------->
                    else{

                            int i;
                            for(i=0; i < numClients; i++)
                            {
								if (serverList[i].client_sockid == fd)
								{
								nbytes = recv(serverList[i].client_sockid, client_data, 1000, 0);	
									if(nbytes==-1)
									{
		                        		cerr<<"recv error";
										exit (EXIT_FAILURE);
									}
									else 
									{
										
											cout<<"Client disconnected"<<endl;
											FD_CLR(serverList[i].client_sockid, &master);
											close(serverList[i].client_sockid);    
											serverList[i].client_sockid=-1;
											serverList[i].client_status=0;

											for (i ; i < numClients - 1 ; i++){
											    strcpy(serverList[i].client_ip,serverList[i+1].client_ip);
											    strcpy(serverList[i].client_port,serverList[i+1].client_port);
										        strcpy(serverList[i].client_name,serverList[i+1].client_name);
										        serverList[i].client_id=serverList[i+1].client_id;
												serverList[i].client_sockid = serverList[i+1].client_sockid;
												serverList[i].client_status = serverList[i+1].client_status;
										    }
											
											numClients=numClients-1;
	
											cout<<"New Client list\n";
										    if(numClients == 0){
										      cout<<"No Connected clients"<<endl;
										    }
										    else{
										        cout<<"ID             HOSTNAME             IP ADDRESS          PORT"<<endl;
										        for(i; i < numClients; i++)
										        	cout<<serverList[i].client_id<<"             "<<serverList[i].client_name<<"             "<<serverList[i].client_ip<<"          "<<serverList[i].client_port<<endl;
										    }

											char peer_no1[600];
											for(int i=0; i < numClients; i++){
											string t=to_string(serverList[i].client_id);
											//string str1(serverList[i].client_name);
											string str2(serverList[i].client_ip);
											string str3(serverList[i].client_port);
											string master1=t+"|"+"highgate"+"|"+str2+"|"+str3+";";
											const char *x=master1.c_str();
											strcpy(peer_no1,x);
											cout<<peer_no1<<endl;	
											  					
											}

											for(int j=0; j < numClients;j++){
											if (send(serverList[j].client_sockid,peer_no1, strlen(peer_no1), 0) < 0)
				    							cout<<"send"<<endl;	
		
	    }                                 
									}
									
									
								}
                     	    }
                       	}
              	}   } 
            }
        }
    }
               


	int registerClient(string task1, struct server_ip_list client_list[5])
  {
	int numbytes;
	struct addrinfo hints,*res,*p;
	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	char s[INET6_ADDRSTRLEN];
	int rv;
	int a=1;	
	memset(&hints,0,sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;	 // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;
	const char *ip_node = task2[1].c_str();
	const char *port_node = task2[2].c_str();
	char client_ip_addr[INET6_ADDRSTRLEN];
	char  hostname[20];
	char  service[20];
	struct sockaddr_in *cl_addr;
	char client_data[MAXDATASIZE];
// <---------- GET ADDRESS INFO ------------>
	if ((rv = getaddrinfo(ip_node,port_node, &hints, &res)) != 0) 
	{
		cerr<<"getaddrinfo: "<<gai_strerror(rv)<<endl;
		return 1;
	}

	for(p = res; p != NULL; p = p->ai_next) 
	{
		sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol);
		if (sockfd== -1)
		{
		cerr<<"client: socket"<<endl;
		continue;
		}
	//	((struct sockaddr_in *)p)->sin_port=atoi(l_port_no);

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
		{
			cerr<<"client: connect"<<endl;
			close(sockfd);
			continue;
		}
		//send(sockfd,l_port_no,strlen(l_port_no),0);
		//cout<<htons(((struct sockaddr_in *)(p->ai_addr))->sin_port)<<endl;
		break;
	}

	if (p == NULL) 
	{
		cerr<<"Client: failed to connect\n";
		exit(1);
	}
//The inet_ntop() function shall convert a numeric address into a text string suitable for presentation. The af argument shall specify the family of the address. connect_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
	client_list[0].client_id='1';
	strcpy(client_list[0].client_ip,task2[1].c_str());
	strcpy(client_list[0].client_port,task2[2].c_str());
	client_list[0].client_sockid=sockfd;
	client_list[0].client_status=1;


	getnameinfo((struct sockaddr *)cl_addr,sizeof client_ip_addr,hostname,100,service,100,0);
	cout<<"Client is connected to "<<s<<endl;
	string sid(task2[0]);
	string sid1 = "REGISTER";
	string port_no(l_port_no);
	string masterdata=sid1+"|"+port_no;
	const char *masterdata2=masterdata.c_str();
	if ((send(sockfd, masterdata2, masterdata.length()+1, 0)==-1))
		cerr<<"Send error"<<endl;
	strcpy(client_list[0].client_name,hostname);
	return 0;
}


char client_ip_addr[INET6_ADDRSTRLEN];


//Read commands from user and perform task for client

int client_program(){
	int numServers = 0;
	startServer();
	FD_ZERO(&master);   
    FD_SET(STDIN_DESC, &master);
    FD_SET(sockfd, &master);
 	fdmax=sockfd;
	int numClients = 1;
	for (int k=0;k<5;k++)	
	{
		client_list[k].client_sockid=-1;
		client_list[k].client_status=0;
	}


	for (int k=0;k<5;k++)	
	{
		serverListCopy[k].client_sockid=-1;
		serverListCopy[k].client_status=0;
	}
			cout<<"[PA1]$";	

	for(;;)
			
	{

		read_fds=master;
		if(select(fdmax+1,&read_fds,NULL,NULL,NULL)<0)
			{
				cerr<<"Select";
			exit(EXIT_FAILURE);	
					}
		
		for(fd=0;fd<=fdmax;fd++)
		{
			//cout<<fd<<endl;
			//cout<<sockfd<<endl;

			if(FD_ISSET(fd,&read_fds))
			{
				if(fd==STDIN_DESC)
				{
					getline(cin,task1);
				    istringstream iss(task1);
				    int i = 0;
				    do
				    {
				        string sub;
				        iss >> sub;
				    	task2[i]=sub;
				        i++;
				    } while (iss);

				    if (task2[0]=="HELP")
						clientHelp();
					else if (task2[0]=="CREATOR")
						creator();
					else if (task2[0]=="DISPLAY")
						display();
					else if (task2[0]=="REGISTER")
					{
						registerClient(task1, client_list);
                        if(client_list[0].client_sockid > fdmax){
                            fdmax=client_list[0].client_sockid;
                        }
						FD_SET(client_list[0].client_sockid, &master);
				    }
				    else if(task2[0].compare("CONNECT")==0)
				    {
                    	cout<<"Connect,Put,Get are not been implemented.Please try other commands"<<endl;
                        
                    }
                    else if(task2[0].compare("PUT")==0)
				    {
                    	cout<<"Connect,Put,Get are implemented.Please try other commands"<<endl;
                        
                    }
                    else if(task2[0].compare("GET")==0)
				    {
                    	cout<<"Connect,Put,Get are implemented.Please try other commands"<<endl;
                        
                    }
                    else if(task2[0]== "LIST")
                    {
                        //List all active connections with a client.
                         if(numClients == 0){
										      cout<<"IP list contains no details"<<endl;
										    }
										    else{
										        cout<<"ID             HOSTNAME             IP ADDRESS          PORT"<<endl;
										        for(i; i < numClients; i++)
										        	cout<<client_list[i].client_id<<"             "<<client_list[i].client_name<<"             "<<client_list[i].client_ip<<"          "<<client_list[i].client_port<<endl;
										    }

                    }

                    else if(task2[0]== "TERMINATE")
                    {                      
                        if((task2[1].compare("1")==0))
                        {
                            cout<<"Cannot terminate connection with server";
                        }
                        else
                        {
							 cout<<"Connection does not exist";
                        }		
                    }
                    else if((task2[0]== "QUIT")){
                       int m;
						    for(m = 0;m < numClients ; m++){
						        close(m);
						    }                        
							cout<<"All the connections are closed."<<endl;
							exit(0);
                    }
                    else{
                            cout<<"Invalid Command. Type HELP command for valid commands.\n";
                        
                    }
                }
             
                    else if(client_list[0].client_sockid == fd)
                    {
	                    char server_data[MAXDATASIZE];
	                    if((nbytes = recv(sockfd, server_data, 1000, 0)) < 0)
	                    	cout<<"Error in receiving";
	                    

						char updated_data[500][500];
						char *temp;
						int counter=0;
						temp = strtok(server_data,  ";");
						cout<<"Updated Server IP List"<<endl;
 				       cout<<"ID             HOSTNAME             IP ADDRESS          PORT"<<endl;
						
							while(temp != NULL)
							    {
							    	cout<<temp<<endl;

		    						strcpy(updated_data[counter], temp);
		    						counter += 1;
		    						//if((strcmp(arg, NULL)==0))
		    						if((strcmp(temp, " ")==0))
		    						{
		    						    break;
		    						}
		    					temp = strtok(NULL, ";");
		    					}

						char *temp1;
						int counter1;
						numServers = 0;

		    				for(int n=0; n < counter; n++)
		    				{
		    					temp1 = strtok(updated_data[i], "|");
		    					counter1=0;
		    					while(temp1 != NULL)
		    					{
		    				      	if(counter1 == 0)
		    							serverListCopy[n].client_id=numServers+1;
	    						    if(counter1 == 1)
	    						       	strcmp(serverListCopy[n].client_name,temp1);
	    						    if(counter1 == 2)
	    						        strcmp(serverListCopy[n].client_ip,temp1);
		    					    if(counter1 == 3)
		       						    strcmp(serverListCopy[n].client_port,temp1);    							    
		    						 
		    							counter1 += 1;
		    							temp1 = strtok(NULL, "|");
		    					}
		    					numServers =numServers + 1;
					        }
                }	
	}

}}}



//Start of Program
int main(int argc, char* argv[])
	{
	l_port_no=argv[2];
	string	node_type(1,*argv[1]);

	if(argc < 3)
	{
		cout << "Kindly pass the correct arguments.Enter arguments as \"s\" for Server and \"c\" for Client, followed by the listening port no."<<endl;
		return 1;
	}
	//Need to remove bug for arg as s2s instead of s

	if(node_type=="s")
	{
		cout << "[PA1]$ The Application is running as Server.Type \"Help\" for avaiable user commands"<<endl;
		server_program();	
	}

	else if(node_type=="c")
	{
		cout << "[PA1]$ The Application is running as Client.Type \"Help\" for avaiable user commands"<<endl;
		client_program();
	}

	else
	{
		cout << "Error!,Kindly pass the correct arguments.Enter arguments as \"s\" for Server and \"c\" for Client, followed by the port no."<<endl;
		exit (EXIT_FAILURE);
	}

}
//End of Checked
