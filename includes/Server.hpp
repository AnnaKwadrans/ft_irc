#ifndef SERVER_HPP
# define SERVER_HPP

# include <netinet/in.h> //<sys/types>
# include <sys/socket.h>
# include <poll.h>
# include <unistd.h>
# include <vector>
# include <iostream>
# include <string>

class Server
{
    private:

        int                         _port;
        std::string                 _password;
        int                         _serverFd;
        std::vector<struct pollfd>  _pollfds;
        sockaddr_in                 _addr;
        struct pollfd               _serverPoll;


     public:

        Server(std::string port, std::string password);
        ~Server();
        
        void    initServer();
        void    runServer();



};

#endif
