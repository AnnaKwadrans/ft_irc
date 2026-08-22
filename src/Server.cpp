#include "../includes/Server.hpp"

Server::Server(std::string port, std::string password) : _port(std::atoi(port.c_str())), _password(password) {}

Server::~Server() {}

void    Server::initServer()
{
        _serverFd = socket(AF_INET, SOCK_STREAM, 0);

        _addr.sin_family = AF_INET;
        _addr.sin_port = htons(_port);
        _addr.sin_addr.s_addr = INADDR_ANY;


}

void    Server::runServer()
{
        bind(_serverFd, (sockaddr *)&_addr, sizeof(_addr));

        _serverPoll.fd = _serverFd;
        _serverPoll.events = POLLIN;
        _serverPoll.revents = 0;
        _pollfds.push_back(_serverPoll);
        listen(_serverPoll.fd, SOMAXCONN);



}