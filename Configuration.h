#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// #include <boost/interprocess/sync/scoped_lock.hpp>
// #include <boost/thread/mutex.hpp>
#include <boost/unordered_map.hpp>
#include <iomanip>

/** Configuration reader
 *
 *  A singleton class, created before main and destructed after it
 *
 */

typedef unsigned char store_t;
typedef unsigned int  people_t;
typedef long          counter_t;

using namespace boost;
//using namespace boost::interprocess;
using namespace std;

typedef unordered_map<string, int> ConfigMap;

class Configuration
{
    public:
        static void load(string const &filename);
        static inline int get(string const &key, int default_value)
        {
//            scoped_lock<boost::mutex> lock(_mtx);
            ConfigMap::iterator iter = _config._values.find(key);
            if (_config._values.end() == iter) 
			{
				set(key, default_value);
			}
			else 
			{
				default_value = iter->second;
			}
			return default_value; 
        }
    
        static inline void set(string const &key, int value)
        {
//            scoped_lock<boost::mutex> lock(_mtx);
            _config._values[key] = value;
        }

        static inline ConfigMap const &values()
        {
//            scoped_lock<boost::mutex> lock(_mtx);
            return _config._values;
        }

        static inline Configuration const& instance()
        {
            return _config;
        }

    protected:
    private:
        static Configuration _config;
//        static boost::mutex _mtx;

        /** values map */
        ConfigMap _values;

        /** Default constructor */
        Configuration();

        /** Default destructor */
        virtual ~Configuration();
};

void counter2time(double *hours, double *minutes, double *seconds, counter_t iteration);
std::ostream& operator<<(std::ostream& os, const Configuration& obj);

#endif // CONFIGURATION_H
