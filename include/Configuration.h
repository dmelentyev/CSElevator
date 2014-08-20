#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/unordered_map.hpp>

using namespace boost;
using namespace boost::interprocess;

static mutex _mtx;

class Configuration
{
    public:
        static void load(std::string const &filename);
        static inline int get(std::string const &key){scoped_lock<mutex> lock(_mtx); return _config._values[key];};
        static inline void set(std::string const &key, int value){scoped_lock<mutex> lock(_mtx); _config._values[key] = value;};

    protected:
    private:
        static Configuration _config;
        /** values map */
        typedef unordered_map<std::string, int> ConfigMap;
        ConfigMap _values;

        /** Default constructor */
        Configuration();

        /** Default destructor */
        virtual ~Configuration();
};

#endif // CONFIGURATION_H
