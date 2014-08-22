#include "Store.h"

Store::Store(unsigned char store)
{
    _store = _max_dest = _min_dest = store;
    int stores = Configuration::get("building.stores");
    _destinations.resize(stores, 0);
}

Store::Store(const Store &other)
{
    _store = other._store;
    _max_dest = other._max_dest;
    _min_dest = other._min_dest;
    _destinations = other._destinations;
}

Store::~Store()
{
}

unsigned int
Store::putPeopleInto(Elevator &elevator)
{
    unsigned int selected = 0, picked = 0;
    unsigned int how_many = elevator.freePlaces();
    bool up = elevator.goingUp();
    int idx_inc = up?-1:1;
    unsigned char current_dest = up?_max_dest:_min_dest;
    
    while (current_dest >= up?_store+1:_min_dest
           && current_dest <= up?_max_dest:_store-1
           && selected < how_many)
    {
        
        scoped_lock<boost::mutex> lock(_mtx);
        if (_destinations[current_dest] > 0)
        {
            picked = min(_destinations[current_dest], how_many);
            selected += picked;
            how_many -= picked;
            _destinations[current_dest] -= picked;
            elevator.boardPassengers(picked, current_dest);
        }
        
        //Adjust min/max_dest
        if (_destinations[current_dest] == 0)
        {
            _min_dest = up?_min_dest:current_dest+idx_inc;
            _max_dest = up?current_dest+idx_inc:_max_dest;
        }
        
        // Move to the next destination
        current_dest += idx_inc;
    }
    
    return selected;
}

unsigned int
Store::getPeopleFrom(Elevator &elevator)
{
    unsigned int picked = elevator.unboardPassengers();
    _destinations[_store] += picked;
    return picked;
}
