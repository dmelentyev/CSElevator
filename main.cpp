#include <iostream>
#include "Configuration.h"
#include "version.h"

using namespace std;

int main(int ARGC, char** ARGV)
{
    { /** Program version info banner */
        string progpath(ARGV[0]);
        size_t lastslash = progpath.find_last_of('/');
        cout << progpath.substr(lastslash+1) << " v" << AutoVersion::FULLVERSION_STRING << endl;
    }

    Configuration::load("config.cfg");

    return 0;
}
