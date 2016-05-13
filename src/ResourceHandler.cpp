#include "linde/ResourceHandler.h"

#include <iostream>
#include <fstream>

static std::string localPathToResource;

std::string ResourceHandler::getResource(const std::string &s)
{
    if (localPathToResource.empty())
    {
        // local path not yet loaded
        std::ifstream stream("../resources.path");
        if (stream.is_open())
        {
            std::getline(stream, localPathToResource);
            stream.close();
        }
        else
        {
            // no 'resources.path' file
            std::cerr << "Could not find your 'resources.path' file!\n";
            std::cin.get();
            return std::string();
        }

    }
    return localPathToResource + s;
}
