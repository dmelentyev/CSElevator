#include "Store.h"
#include <boost/foreach.hpp>

Store::Store(unsigned char store)
{
    _store = store;
	_max_time = _min_time = _tot_time = 0L;
    _tot_people = 0;

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
	unsigned char current_dest;

	//Make sure elevator is at same store!
    assert (elevator.getCurrentStore() == number());

    for (current_dest = 0; current_dest <= maxStore() && (selected < how_many); current_dest++)
    {
		if (passengersTo(current_dest) > 0)
		{
		    if (   number() != current_dest 
		        && elevator.serves(current_dest) 
		        )
			{

				if (
				      (up && current_dest > number())
					|| (!up && current_dest < number())
					)
				{
				    picked = min(passengersTo(current_dest), how_many);
				    selected += picked;
				    how_many -= picked;
				    addPassengersTo(current_dest, 0-picked);
				    elevator.boardPassengers(picked, current_dest);
				}
			}
		}
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

std::ostream& operator<<(std::ostream& os, const Store& obj)
{
    os  << "s" << obj.number();
	for(unsigned int i; i < obj.maxStore (); i++)
	{
		os << ":" << hex << obj.passengersTo (i) << dec;
	}

	return os;
}
