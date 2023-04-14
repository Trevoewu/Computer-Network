from socket import * 
serverPort = 12000
serverHost = 'localhost'
# AF_INET refer to use IPv4 SOCK_DGRAM refer to UPD
# os resign client porter
clientSocket = socket(AF_INET,SOCK_DGRAM)
message = input('Input message:')
# sent message to server
clientSocket.sendto(message.encode(),(serverHost,serverPort))
# receive modified message from server
modifiedMessage,serverAddress = clientSocket.recvfrom(2048)
# print received message
print(modifiedMessage.decode())
# close connection
clientSocket.close()