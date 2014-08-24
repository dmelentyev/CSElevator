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
