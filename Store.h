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
#include "Person.h"

typedef vector<people_t> Calls;

class Store
{
    public:
    Store(store_t store);
    virtual ~Store();
    people_t putPeopleInto(Elevator &elevator);
    people_t getPeopleFrom(Elevator &elevator, counter_t iteration);
    inline bool hasPassengers() const 
	{
		return (hasPassengersUp() || hasPassengersDown());
	}
    inline bool hasPassengersUp() const 
	{
		return !_queue_up.empty();
	}
    inline bool hasPassengersDown() const 
	{
		return !_queue_down.empty();
	}
		
    inline people_t passengers__To(store_t store) const 
	{
		assert(store <= maxStore());
		try 
		{
			return 0; //destinations.at(store);
		}
		catch (out_of_range)
		{
			return 0;
		}
	}
		
    inline people_t addPassengers__To(store_t store, people_t p_in)
	{
		assert(store <= maxStore()); 
		//_destinations[store] += p_in; 
		return 0;//_destinations[store];
	}

    inline people_t substractPassengers__To(store_t store, people_t p_out)
	{
		assert(store <= maxStore()); 
		// _destinations[store] -= p_out; 
		return 0; //_destinations[store];
	}
		
    inline store_t storeNumber() const {return _store;}
		
    static inline store_t maxStore()
	{
		if (0 == _max_stores_cache)
		{
			_max_stores_cache = Configuration::get("building.stores")-1;
			assert(_max_stores_cache > 0);
		}
		return _max_stores_cache;
	}

	inline void updateStats(counter_t delta)
	{
		_max_time = (delta > _max_time) ? delta : _max_time;
		_min_time = (delta < _min_time) ? delta : _min_time;
		_tot_time += delta;
		_tot_people++;
	}
		
	inline void enqueuePassenger(Person const& person, counter_t iteration)
	{
		Person p = person;
		if (p.destination() > maxStore ())
			assert(p.destination() <= maxStore());
		
		if (p.destination() > storeNumber())
		{
			p.resetElevatorTimer(iteration);
			_queue_up.push_back(p);
		}
		else if (p.destination() < storeNumber())
		{
			p.resetElevatorTimer(iteration);
			_queue_down.push_back(p);
		}
		else 
		{
			_queue_staying.push_back(p);
		}
	}

	inline counter_t minTime() const {return _min_time;}
	inline counter_t maxTime() const {return _max_time;}
	inline counter_t avgTime() const {return (_tot_people > 0) ? _tot_time/_tot_people : 0;}
    protected:
    private:

	static store_t _max_stores_cache;
    store_t   _store;
    counter_t _max_time;
    counter_t _min_time;
    counter_t _tot_time;
    people_t  _tot_people;

	PeopleQueue _queue_up;
	PeopleQueue _queue_down;
	PeopleQueue _queue_staying;

};

std::ostream& operator<<(std::ostream& os, const Store& obj);

#endif // STORE_H
