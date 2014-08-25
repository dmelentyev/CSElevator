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
    inline people_t passengerQueueSize() const 
    {
	    return ( _queue_up.size() + _queue_down.size() );
    }

    inline bool passengerQueueEmpty() const 
    {
	    return ( _queue_up.empty() && _queue_down.empty() );
    }

    inline bool hasPassengers(Elevator const& el) const 
	{
		return (hasPassengersUp(el) || hasPassengersDown(el));
	}
    inline bool hasPassengersUp(Elevator const& el) const 
	{
		if (!_queue_up.empty())
		{
			PeopleQueue::const_iterator i = _queue_up.begin();
			for(; i!=_queue_up.end(); i++)
				if (el.serves( (*i).destination()))
					return true;
		}
		return false;
	}
    inline bool hasPassengersDown(Elevator const& el) const 
	{
		if (!_queue_down.empty())
		{
			PeopleQueue::const_iterator i = _queue_down.begin();
			for(; i!=_queue_down.end(); i++)
				 if (el.serves((*i).destination()))
					return true;
		}
		return false;
	}
		
    inline store_t storeNumber() const {return _store;}
		
    static inline store_t maxStore()
	{
		if (0 == _max_stores_cache)
		{
			_max_stores_cache = Configuration::get("building.stores", 100)-1;
			assert(_max_stores_cache > 0);
		}
		return _max_stores_cache;
	}

	inline void updateStats(counter_t delta)
	{
		_max_time = max(delta, _max_time);
		_min_time = min(delta, _min_time);
		_tot_time += delta;
		_tot_people++;
	}
		
	inline void scheduleForExit(counter_t iteration)
	{
		// If people are coming from office
		while (!_queue_staying.empty() 
		    && _queue_staying.front().enqueued() == iteration)
		{
			Person p = _queue_staying.front();
			enqueuePassenger(p, iteration);
			_queue_staying.pop_front();
		}
	}
		
	inline void enqueuePassenger(Person const& person, counter_t iteration)
	{
		Person p = person;
		if (p.destination() > maxStore ())
			assert(p.destination() <= maxStore());
		
		p.resetElevatorTimer(iteration);
		if (p.destination() > storeNumber())
		{
			_queue_up.push_back(p);
		}
		else if (p.destination() < storeNumber())
		{
			_queue_down.push_back(p);
		}
		else 
		{
			// Schedule for exit in 9 hours. 8 working hours + 1 hour lunch
			p.setDestination(0);
			p.resetElevatorTimer(iteration + 9*3600);
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
