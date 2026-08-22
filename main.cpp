#include <stdexcept>
#include <iostream>
#include "includes/Server.hpp"

int     main(int argc, char **argv)
{
        (void)argv;
        try
        {
                if (argc != 3)
                        throw std::runtime_error("usage: ./ircserv <port> <password>");
                
                // init server
                Server  serv(argv[1], argv[2]);
                serv.initServer();
                serv.runServer();

                // run server
        }
        catch(const std::exception& e)
        {
                std::cerr << e.what() << '\n';
        }

        return (0);
}
