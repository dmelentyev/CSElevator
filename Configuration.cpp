#include "Configuration.h"
#include <fstream>

Configuration Configuration::_config;
boost::mutex Configuration::_mtx;

Configuration::Configuration()
{
    //ctor
    // Add meaningful defaults as per task requirements:
    _config._values["building.stores"] = 100;
    _config._values["building.store.capacity"] = 100;
    _config._values["building.elevators"] = 3;
    
    _config._values["elevator1.capacity"] = 20;
    _config._values["elevator1.skip.from"] = 1;
    _config._values["elevator1.skip.to"] = 49;
    _config._values["elevator1.skip.when.full"] = 0;
    _config._values["elevator1.speed"] = 1;
    _config._values["elevator1.stop.time"] = 5;
    
    _config._values["elevator2.capacity"] = 20;
    _config._values["elevator2.skip.when.full"] = 0;
    _config._values["elevator2.speed"] = 1;
    _config._values["elevator2.stop.time"] = 5;
    
    _config._values["elevator3.capacity"] = 20;
    _config._values["elevator3.skip.from"] = 1;
    _config._values["elevator3.skip.to"] = 50;


}

Configuration::~Configuration()
{
    //dtor
}

void
Configuration::load(string const &filename)
{
    scoped_lock<boost::mutex> lock(_mtx);

    cout << "Reading from config file: " << filename << endl;
    ifstream configfile(filename);
    if (configfile.bad())
    {
        cerr << "Unable to open file!" << endl;
    }

    while (configfile.good())
    {
        string key;
        int value = 0;

        configfile >> key >> value;

        if (!key.empty())
        {
            _config._values[key] = value;
            cout << key << " " << value << endl;
        }
    }

    configfile.close();

}
