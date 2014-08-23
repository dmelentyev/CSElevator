#ifndef STORE_H
#define STORE_H

#include "Configuration.h"
#include "Elevator.h"

class Store
{
    public:
    Store(unsigned char store);
    virtual ~Store();
    unsigned int putPeopleInto(Elevator &elevator);
    unsigned int getPeopleFrom(Elevator &elevator);
    inline bool hasPassengers(){return (_min_dest != _max_dest);}
    inline bool hasPassengersUp(){return (_max_dest != _store);}
    inline bool hasPassengersDown(){return (_min_dest != _store);}
    inline unsigned int passengersTo(unsigned char store){assert(store <= _destinations.size()); return _destinations[store];}
    inline unsigned int addPassengersTo(unsigned char store, int passengers){assert(store <= _destinations.size()); _destinations[store] += passengers; return _destinations[store];}
    inline unsigned char number(){return _store;}
    inline unsigned char maxStore(){return _destinations.size()-1;}
    inline void callElevator(){}
    inline unsigned int setDestination(unsigned char store, unsigned int people)
    {
        if (_destinations.size() > store)
        {
            _destinations[store] = people;
            _max_dest = max(_max_dest, store);
            _min_dest = min(_min_dest, store);
            return people;
        }
        return 0;
    }

    protected:
    private:
    
    unsigned char _store;
    unsigned char _max_dest;
    unsigned char _min_dest;
    vector<unsigned int>  _destinations;

};

#endif // STORE_H
