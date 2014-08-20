#include <iostream>
#include "Configuration.h"

using namespace std;

int main()
{
    Configuration::load("config.cfg");

    cout << "Hello world!" << endl;
    return 0;
}
