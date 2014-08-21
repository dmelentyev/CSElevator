#include "Configuration.h"
#include <fstream>

Configuration Configuration::_config;
boost::mutex Configuration::_mtx;

Configuration::Configuration()
{
    //ctor
}

Configuration::~Configuration()
{
    //dtor
}

void
Configuration::load(string const &filename){
    scoped_lock<boost::mutex> lock(_mtx);

    cout << "Reading from config file: " << filename << endl;
    ifstream configfile(filename);
    if (configfile.bad())
        cerr << "Unable to open file!" << endl;

    while (configfile.good()) {
        string key;
        int value = 0;

        configfile >> key >> value;

        if (!key.empty()) {
            _config._values[key] = value;
            cout << key << " " << value << endl;
        }
    }

    configfile.close();

}
