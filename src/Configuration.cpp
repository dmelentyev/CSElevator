#include "Configuration.h"
#include <fstream>

Configuration Configuration::_config;

Configuration::Configuration()
{
    //ctor
}

Configuration::~Configuration()
{
    //dtor
}

void
Configuration::load(std::string const &filename){
    scoped_lock<boost::mutex> lock(_mtx);

    std::string key;
    int value;

    std::ifstream configfile(filename);
    while (configfile.is_open() & !configfile.bad() & !configfile.eof()) {
        configfile >> key >> value;
        _config._values[key] = value;
    }

    configfile.close();

}

//Configuration &
//Configuration::instance(){
//    scoped_lock<mutex> lock(_mtx);
//    if (nullptr == _config) {
//        config = new
//    }
//    return *_config;
//}
