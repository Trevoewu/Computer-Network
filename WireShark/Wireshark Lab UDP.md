# Wireshark Lab: UDP

> 在本实验中，我们将快速了解 UDP 传输协议。 正如我们在本文第 3 章中所看到 的，UDP 是一种简化的协议。 在进行本实验之前，您可能需要重新阅读课本中的 第 3.3 节。 由于 UDP 简单易理解，您只需要一点花费时间就能做这个实验。

开始在 Wireshark 中捕获数据包，然后执行一些会导致主机发送和接收多个 UDP 数据包的操作。 您也可以什么也不做，仅执行 wireshark 捕获以便获取其他程序发 给您的 UDP 数据包。 有一种特殊情况:简单网络管理协议(SNMP - 请参阅课本 中的第 5.7 节)在 UDP 内部发送 SNMP 消息，因此您可能会在跟踪中找到一些 SNMP 消息(以及 UDP 数据包)。

停止数据包捕获后，设置数据包筛选器，以便 Wireshark 仅显示在主机上发送和接 收的 UDP 数据包。 选择其中一个 UDP 数据包并在详细信息窗口中展开 UDP 字 段。 如果您无法找到 UDP 数据包或无法在实时网络连接上运行 Wireshark，则可 以下载包含某些 UDP 数据包的数据包跟踪。

如果可能的话建议您使用 wireshark 的文件->打印功能将您跟踪回答数据包最小详 细结果打印出来，并且通过注释圈出。

![image-20230416142537709](/Users/trevorwu/Library/Application Support/typora-user-images/image-20230416142537709.png)

- .从跟踪中选择一个 UDP 数据包。 从此数据包中，确定 UDP 标头中有多少 字段。 (建议不要查看课本，直接根据您的数据包跟踪结果回答)，并为 这些字段命名

  4个字段: Source Port , Destination Port, Length, Checksum

- 通过查询 Wireshark 的数据包内容字段中显示的信息，确定每个 UDP 报头 字段的长度(以字节为单位)。

  Source Port:	2 Byte(0~65535)
  Destination Port:	2 Byte 
  Length: 2Byte
  Checksum: 2Byte

- 长度字段中的值是指的是什么?(此问题您可以参考课本)。 使用捕获的 UDP 数据包验证您的声明。

  头部字段的长度+ payload length, 

- UDP 有效负载中可包含的最大字节数是多少? (提示:这个问题的答案 可以通过你对上述 2 的回答来确定)

  65536 - 8 = 65528

- 最大可能的源端口号是多少? (提示:见 4 中的提示)

  65535

- UDP 的协议号是什么? 以十六进制和十进制表示法给出答案。 要回答这个 问题，您需要查看包含此 UDP 段的 IP 数据报的 Protocol 字段(参见书中的 图 4.13 和 IP 头字段的讨论)。

  ![image-20230416142906620](/Users/trevorwu/Library/Application Support/typora-user-images/image-20230416142906620.png)

  0x11 17

- 7.观察发送 UDP 数据包后接收响应的 UDP 数据包，这是对发送的 UDP 数 据包的回复，请描述两个数据包中端口号之间的关系。(提示:对于响应 UDP 目的地应该为发送 UDP 包的地址)

发送: ![image-20230416142818840](/Users/trevorwu/Library/Application Support/typora-user-images/image-20230416142818840.png)

回复: 

![image-20230416142835758](/Users/trevorwu/Library/Application Support/typora-user-images/image-20230416142835758.png)