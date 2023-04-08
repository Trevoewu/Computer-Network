#import socket module 
from socket import * 
import sys # In order to terminate the program 

serverSocket = socket(AF_INET, SOCK_STREAM) 
#Prepare a sever socket 
#Fill in start 
serverPort = 80
serverSocket.bind(('',serverPort))
serverSocket.listen(1)
#Fill in end 
while True: 
    #Establish the connection 
    print('Ready to serve...') 
    connectionSocket,addr = serverSocket.accept()
      #Fill in start              #Fill in end           
    try: 
        message =  connectionSocket.recv(1024).decode() #Fill in start          #Fill in end                
        filename = message.split()[1]               
        print ('receive file: '+filename)   
        f = open(filename[1:],'r')                         
        outputdata = f.read() #Fill in start       #Fill in end                    
        #Send one HTTP header line into socket  
        #Fill in start 
        connectionSocket.send("HTTP/1.1 200 OK\r\n\r\n".encode())
        #Fill in end                 
        #Send the content of the requested file to the client 
        for i in range(0, len(outputdata)):            
            connectionSocket.send(outputdata[i].encode()) 
        connectionSocket.send("\r\n".encode()) 
        connectionSocket.close() 
    except IOError: 
        #Send response message for file not found 
        #Fill in start     
        connectionSocket.send('404 NOT FOUND\r\n\r\n'.encode())  
        #Fill in end 
        #Close client socket 
        #Fill in start 
        serverSocket.close()
        #Fill in end             
serverSocket.close() 
sys.exit()#Terminate the program after sending the corresponding data    