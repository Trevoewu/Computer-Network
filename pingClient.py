from socket import * 
import time
serverPort = 12000
serverHost = 'localhost'
# AF_INET refer to use IPv4 SOCK_DGRAM refer to UPD
# os resign client porter
clientSocket = socket(AF_INET,SOCK_DGRAM)
n = 1
while n <= 10:
# sent message to server
  clientSocket.settimeout(1)
  message = "ping"
  sendTime = time.time()
  clientSocket.sendto(message.encode(),(serverHost,serverPort))
  try:
    modifiedMessage,serverAddress = clientSocket.recvfrom(2048)
    rtt = time.time() - sendTime;
    print("sequence %d: replay from %s RTT=%.3fs"%(n,serverHost,rtt))
  except Exception as e:
    print('sequence %d Request timed out'%(n))
  n = n+1  
# receive modified message from server
# print received message
# close connection
clientSocket.close()