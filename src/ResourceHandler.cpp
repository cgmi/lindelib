#include "linde/ResourceHandler.h"

#include <iostream>
#include <fstream>

static std::string localPathToResource;

std::string ResourceHandler::getResource(const std::string &s)
{
	{
		if (localPathToResource.empty())
		{
			// local path not yet loaded
			std::ifstream stream("../resources.path");
			if (stream.is_open())
			{
				std::getline(stream, localPathToResource);
			}
			else
			{
				// no 'resources.path' file
				std::cout << "Could not find your 'resources.path' file!\n";
				return std::string();
			}
            stream.close();
		}
		return localPathToResource.append(s);
	}
}
