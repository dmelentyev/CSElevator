#include "Store.h"

Store::Store(unsigned char store)
{
    _store = _max_dest = _min_dest = store;
    int stores = Configuration::get("building.stores");
    _destinations.resize(stores, 0);
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
    unsigned char current_dest = up?maxStore():0;
	unsigned char temp_dest = current_dest;
    
    while (
           current_dest >= 0 && current_dest <= maxStore()
           && (selected < how_many)
		)
    {
        if (elevator.getCurrentStore() != current_dest && elevator.serves(current_dest))
		{
		    if (passengersTo(current_dest) > 0)
		    {
		        picked = min(passengersTo(current_dest), how_many);
		        selected += picked;
		        how_many -= picked;
		        addPassengersTo(current_dest, 0-picked);
		        elevator.boardPassengers(picked, current_dest);
		    }
		    
		    //Adjust min/max_dest if still has passengers to current_dest
		    if (passengersTo(current_dest) > 0)
		    {
				temp_dest = current_dest;
		    }
		}
        // Move to the next destination
        current_dest += idx_inc;
    }

	// Update min/max dests
    _min_dest = min(temp_dest, _store);
    _max_dest = max(temp_dest, _store);
    
	return selected;
}

unsigned int
Store::getPeopleFrom(Elevator &elevator)
{
    unsigned int picked = elevator.unboardPassengers();
    _destinations[_store] += picked;
    return picked;
}
