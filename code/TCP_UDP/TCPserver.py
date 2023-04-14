from socket import * 
serverPort = 12000
# SOCK_STREAM refer to TCP socket 
# os resign TCP socket 
serverSocket = socket(AF_INET,SOCK_STREAM)
# bind porter with socket
serverSocket.bind(('',serverPort))
# serverSocket is welcome Socket waiting from client TCP connection 
serverSocket.listen(1)
print("Server is reading to receive")
while True :
    # create a new socket connection socket dedicated for client connection 
    connectionSocket,addr = serverSocket.accept()
    message = connectionSocket.recv(1024)
    modifiedMessage = message.decode().upper()
    print("receive:"+modifiedMessage)
    connectionSocket.send(modifiedMessage.encode())
    connectionSocket.close()