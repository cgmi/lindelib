#include "Chai.h"
#include <chaiscript/chaiscript_stdlib.hpp>

int main(int argc, char ** args)
{
    chaiscript::ChaiScript chai(chaiscript::Std_Lib::library());

    if (argc < 2)
    {
        std::cerr << "no script given as argument" << std::endl;
        return EXIT_FAILURE;
    }

    addLindeLibFunctions(chai);

    try
    {
        chai.eval_file(args[1]);
    }
    catch (const std::string & e)
    {
        std::cerr << e << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
