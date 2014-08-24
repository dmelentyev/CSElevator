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

class Store
{
    public:
    Store(unsigned char store);
    virtual ~Store();
    unsigned int putPeopleInto(Elevator &elevator);
    unsigned int getPeopleFrom(Elevator &elevator);
    inline bool hasPassengers() const 
	{
		return (hasPassengersUp() || hasPassengersDown());
	}
    inline bool hasPassengersUp() const 
	{
		int p;
		unsigned char s, maxstore = maxStore();
		
		for( s = _store+1, p = 0; s <= maxstore && p == 0; s++ )
		{
			p = passengersTo(s);
		}
		return (p != 0);
	}
    inline bool hasPassengersDown() const 
	{
		int p;
		unsigned char s;
		for( s = 0, p = 0; s < _store && p == 0; s++ )
		{
			p = passengersTo (s);
		}
		return (p != 0);
	}
    inline unsigned int passengersTo(unsigned char store) const 
	{
		if (store > maxStore()) 
			assert(store <= maxStore()); 
		return _destinations[store];
	}
    inline unsigned int addPassengersTo(unsigned char store, int passengers)
	{
		assert(store <= maxStore()); 
		_destinations[store] += passengers; 
		return _destinations[store];
	}
		
    inline unsigned char number() const {return _store;}
    inline unsigned char maxStore() const
	{
		if(_destinations.size() == 0 || _destinations.size() > 100)
		{
			assert(_destinations.size() > 0);
			assert(_destinations.size() <= 100);
		}
		unsigned char ret = _destinations.size()-1;
		return ret;
	}

    protected:
    private:
    
    unsigned char _store;
    long long _max_time;
    long long _min_time;
    long long _tot_time;
    unsigned char _tot_people;
    vector<unsigned int>  _destinations;

};

std::ostream& operator<<(std::ostream& os, const Store& obj);

#endif // STORE_H
