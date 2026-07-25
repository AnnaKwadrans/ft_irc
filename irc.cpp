/*
Funciones b'asicas para crear un servidor TCP en Linux

socket()                <sys/socket.h>
   ↓
bind()
   ↓
listen()
   ↓
accept()
   ↓
recv() / send()
   ↓
close()
*/

#include <sys/socket.h>

int     main(void)
{

        int     fd = socket(AF_INET, SOCK_STREAM, 0);
        /*
        crea un socket

        AF_INET         -> usar IPv4
        SOCK_STREAM     -> usar TCP (conexión fiable)
        0               -> usar el protocolo por defecto (TCP)
        */

        sockaddr_in addr;

        addr.sin_family = AF_INET;
        addr.sin_port = htons(6667);
        addr.sin_addr.s_addr = INADDR_ANY;

        bind(serverSocket,
                (sockaddr *)&addr,
                sizeof(addr));



        return (0);
}