# Netdisk 

### A Simple File Transfer Server 

- ##### 基于线程池实现

- ##### 实现了ssh登录

- ##### 长短命令分离

- ##### 断点下载

- ##### 数据库记录用户操作

- ##### 支持大文件传输

### server

1. ##### `make`

2. ##### 生成服务器rsa

   `openssl genrsa -out server_rsa.key 3072`

   `openssl rsa -in server_rsa.key -pubout -out server_rsa_pub.key`

   ##### 将服务器公钥放入客户端

   `cp server_rsa_pub.key ../client/`

3. ##### 执行

   修改conf/文件夹下的配置文件，server.conf的配置参数分别为server IP地址、端口号、线程数目、队列最大长度；mysql.conf的配置参数分别为mysql服务器地址、用户名、密码、数据库名

   `./server`

### client

1. ##### `make`

2. ##### 执行

   修改conf/文件夹下的配置文件，client.conf的配置参数分别为server IP地址、端口号、线程数目、队列最大长度

   `./client`





