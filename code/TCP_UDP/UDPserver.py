from socket import * 
serverPort = 12000
# AF_INET refer to use IPv4 SOCK_DGRAM refer to UPD
serverSocket = socket(AF_INET,SOCK_DGRAM)
# bind porter with socket
serverSocket.bind(('',serverPort))
print("Server is reading to receive")
while True :
    message, clientAddress = serverSocket.recvfrom(2048)
    modifiedMessage = message.decode()
    print("receive:"+modifiedMessage)
    serverSocket.sendto(modifiedMessage.encode(),clientAddress)
