/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * Store.cpp
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

#include "Store.h"
#include <boost/foreach.hpp>

store_t Store::_max_stores_cache=0;

Store::Store(store_t store)
{
    _store = store;
	_max_time = _min_time = _tot_time = 0L;
    _tot_people = 0;
    _destinations.resize(maxStore(), 0);
}

Store::~Store()
{
}

people_t
Store::putPeopleInto(Elevator &elevator)
{
    people_t selected = 0, picked = 0;
    people_t how_many = elevator.freePlaces();
    bool     up = elevator.goingUp();
	store_t  current_dest;

	//Make sure elevator is at same store!
    assert (elevator.getCurrentStore() == storeNumber());

    for (current_dest = 0; current_dest <= maxStore() && (selected < how_many); current_dest++)
    {
		if (passengersTo(current_dest) > 0)
		{
		    if (   storeNumber() != current_dest 
		        && elevator.serves(current_dest) 
		        )
			{

				if (
				      (up && current_dest > storeNumber())
					|| (!up && current_dest < storeNumber())
					)
				{
				    picked = min(passengersTo(current_dest), how_many);
				    selected += picked;
				    how_many -= picked;
				    substractPassengersTo(current_dest, picked);
					elevator.boardPassengers(picked, current_dest);
				}
			}
		}
    }

	return selected;
}

people_t
Store::getPeopleFrom(Elevator &elevator)
{
    people_t picked = elevator.unboardPassengers();
    _destinations[_store] += picked;
    return picked;
}

std::ostream& operator<<(std::ostream& os, const Store& obj)
{
    os  << "s" << obj.storeNumber();
	for(store_t i; i < obj.maxStore (); i++)
	{
		os << ":" << hex << obj.passengersTo(i) << dec;
	}

	return os;
}
