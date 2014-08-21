#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/unordered_map.hpp>
/** Configuration reader
 *
 *  A singleton class, created before main and destructed after it
 *
 */

using namespace boost;
using namespace boost::interprocess;
using namespace std;

class Configuration
{
    public:
        static void load(string const &filename);
        static inline int get(string const &key){scoped_lock<boost::mutex> lock(_mtx); return _config._values[key];};
        static inline void set(string const &key, int value){scoped_lock<boost::mutex> lock(_mtx); _config._values[key] = value;};

    protected:
    private:
        static Configuration _config;
        static boost::mutex _mtx;

        /** values map */
        typedef unordered_map<string, int> ConfigMap;
        ConfigMap _values;

        /** Default constructor */
        Configuration();

        /** Default destructor */
        virtual ~Configuration();
};

#endif // CONFIGURATION_H
