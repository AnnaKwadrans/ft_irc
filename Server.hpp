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
    int                         port;
    std::string                 password;
    int                         socketFd;
    std::vector<struct pollfd>  pollfds;
    sockaddr_in                 addr;
    struct pollfd               serverPoll;


     



};

#endif
