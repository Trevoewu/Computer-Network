from socket import * 
serverPort = 12001
serverHost = 'localhost'
# AF_INET refer to use IPv4 SOCK_DGRAM refer to UPD
# os resign client porter
clientSocket = socket(AF_INET,SOCK_STREAM)
message = input('Input message:')
# built a TCP connection
clientSocket.connect((serverHost,serverPort))
# sent message 
clientSocket.send(message.encode())
# receive modified message from server
modifiedMessage = clientSocket.recv(1024)
# print received message
print("receiver from server: "+modifiedMessage.decode())
# close connection
clientSocket.close()