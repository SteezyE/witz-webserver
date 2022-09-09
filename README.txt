   witz-webserver
====================
-- prerequisites --
OS: Linux, macOS or Windows w/ MinGW

-- setup --
modify IP(v4) and PORT in witzserver.c (src folder)
optional: port forwarding (on router) to be globally reachable 

-- installation --
Linux/macOS: gcc witzserver.c -o witzserver 
Windows: gcc witzserver.c -o witzserver.exe -lws2_32

-- useage --
load website and receive random joke (german unix-fortune jokes)
e.g. curl IP:PORT
