#include "Configuration.h"
#include <fstream>
#include <iostream>

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
    
    _config._values["elevator2.capacity"] = 20;
    
    _config._values["elevator3.capacity"] = 20;
    _config._values["elevator3.skip.from"] = 50;
    _config._values["elevator3.skip.to"] = 99;


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
    ifstream configfile(filename.c_str());

	if (!configfile.good())
    {
        cout << "Unable to open file '" << filename << "'" << endl;
        cout << "Loading defaults" << endl;
    }

    while (configfile.good())
    {
        string key;
        int value = 0;

        configfile >> key >> value;

        if (!key.empty())
        {
            _config._values[key] = value;
        }
    }

    configfile.close();

}

std::ostream& operator<<(std::ostream& os, const Configuration& obj)
{
	for(ConfigMap::const_iterator i = obj.values().begin(); i != obj.values().end(); i++)
	{
            os << (*i).first << " " << (*i).second << endl;
	}
	return os;
}

void counter2time(double *hours, double *minutes, double *seconds, counter_t iteration)
{
		*hours = iteration/3600;
		*minutes = (iteration - int(*hours)*3600)/60;
		*seconds = iteration - int(*hours)*3600 - int(*minutes)*60;
}


