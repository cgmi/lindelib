#ifndef RESOURCEHANDLER_H
#define RESOURCEHANDLER_H

#include <string>

class ResourceHandler
{
	public:
	
		//Creates an absolute ressource location for a given file, depending on your local 'resources.path' file.
		// The path file should only contain your resource location as first line in this form: /.../.../resource-folder/
		static std::string getResource(const std::string &s);
	
	private:
	
		ResourceHandler() {};
		~ResourceHandler() {};
};

#endif // RESOURCEHANDLER_H