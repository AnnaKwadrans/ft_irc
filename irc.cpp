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

#include <netinet/in.h> //<sys/types>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>
#include <vector>
#include <iostream>


int     main(void)
{
        int     serverFd = socket(AF_INET, SOCK_STREAM, 0);

        std::vector<struct pollfd> pollfds;
        /*
        crea un socket

        AF_INET         -> usar IPv4
        SOCK_STREAM     -> usar TCP (conexión fiable)
        0               -> usar el protocolo por defecto (TCP)
        */

        sockaddr_in addr;

        addr.sin_family = AF_INET;
        addr.sin_port = htons(6667); // 8080
        addr.sin_addr.s_addr = INADDR_ANY;
      /*
      2. Defining Server Address
      htons(): Converts port to network byte order.
      INADDR_ANY: Accept connections on any IP.

      sockaddr_in: It is the data type that is used to store the address of the socket.
      htons(): This function is used to convert the unsigned int from machine byte order
               to network byte order.
      INADDR_ANY: It is used when we don't want to bind our socket to any particular IP
               and instead make it listen to all the available IPs.
      */

        bind(serverFd,
                (sockaddr *)&addr,
                sizeof(addr));

         
         struct pollfd   serverPoll;

         serverPoll.fd = serverFd;
         serverPoll.events = POLLIN;
         serverPoll.revents = 0;
         pollfds.push_back(serverPoll);
         listen(serverPoll.fd, SOMAXCONN);

         char  buffer[1024];
         int clientFd = 0;

         while (true)
         {
            if (poll(&pollfds[0], pollfds.size(), -1) < 0)
               break;
      
            for (size_t i = 0; i < pollfds.size(); ++i)
            {
               if (pollfds[i].revents & POLLIN)
               {
                     if (pollfds[i].fd == serverFd)
                     {
                        // Nueva conexión
                        int clientFd = accept(serverFd, NULL, NULL);

                        // Añadir clientFd a pollfds
                        struct pollfd clientPoll;
                        clientPoll.fd = clientFd;
                        clientPoll.events = POLLIN;
                        clientPoll.revents = 0;

                        pollfds.push_back(clientPoll);
                     }
                     else
                     {
                        // Cliente ha enviado datos
                        int bytes = recv(pollfds[i].fd, buffer, sizeof(buffer), 0);
                        if (bytes > 0)
                           std::cerr << buffer << std::endl;
                        send(pollfds[i].fd, ":irc PRIVMSG ania :recibido\r\n", 30, 0);
                     }
               }
            }
               /*for (size_t i = 0; i < pollfds.size(); ++i)
               {*/
                  // if (serverPoll.revents & POLLIN)
                  // {
                  //    if (serverPoll.fd == fd)
                  //    {
                  //          // Nueva conexión
                  //          clientFd = accept(fd, NULL, NULL);
                     
                  //          // Añadir clientFd a pollfds
                  //    }
                  //    else
                  //    {
                  //          // Cliente ha enviado datos
                  //          recv(clientFd, buffer, sizeof(buffer), 0);
                  //          send(clientFd, "recibido", 9, 0);


                  //    }
                  // }
               /*}*/
         }        


        return (0);
}
