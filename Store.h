/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * Store.h
 * Copyright (C) 2014 Dennis Melentyev <dennis.melentyev@gmail.com>
 *
 * CreditSwissElevator is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CreditSwissElevator is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef STORE_H
#define STORE_H

#include "Configuration.h"
#include "Elevator.h"

typedef unordered_map<store_t, people_t> Calls;

class Store
{
    public:
    Store(store_t store);
    virtual ~Store();
    people_t putPeopleInto(Elevator &elevator);
    people_t getPeopleFrom(Elevator &elevator);
    inline bool hasPassengers() const 
	{
		return (hasPassengersUp() || hasPassengersDown());
	}
    inline bool hasPassengersUp() const 
	{
		int p;
		store_t s, maxstore = maxStore();
		
		for( s = _store+1, p = 0; s <= maxstore && p == 0; s++ )
		{
			p = passengersTo(s);
		}
		return (p != 0);
	}
    inline bool hasPassengersDown() const 
	{
		int p;
		store_t s;
		for( s = 0, p = 0; s < _store && p == 0; s++ )
		{
			p = passengersTo (s);
		}
		return (p != 0);
	}
    inline people_t passengersTo(store_t store) const 
	{
		if (store > maxStore()) 
			assert(store <= maxStore()); 
		return _destinations[store];
	}
		
    inline people_t addPassengersTo(store_t store, people_t p_in)
	{
		assert(store <= maxStore()); 
		_destinations[store] += p_in; 
		return _destinations[store];
	}
		
    inline people_t substractPassengersTo(store_t store, people_t p_out)
	{
		assert(store <= maxStore()); 
		_destinations[store] -= p_out; 
		return _destinations[store];
	}
		
    inline store_t storeNumber() const {return _store;}
		
    static inline store_t maxStore()
	{
		if (0 == _max_stores_cache)
		{
			_max_stores_cache = Configuration::get("building.stores");
			assert(_max_stores_cache > 0);
		}
		return _max_stores_cache;
	}

    protected:
    private:

	static store_t _max_stores_cache;
    store_t _store;
    counter_t _max_time;
    counter_t _min_time;
    counter_t _tot_time;
    people_t _tot_people;
	vector<people_t> _destinations;
//    Calls  _destinations;

};

std::ostream& operator<<(std::ostream& os, const Store& obj);

#endif // STORE_H
