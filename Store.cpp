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
#include <iomanip>

store_t Store::_max_stores_cache=0;

Store::Store(store_t store)
{
    _store = store;
	_max_time = _min_time = _tot_time = 0L;
	_min_time = ~_min_time;
	_min_time = ((unsigned long)_min_time) >> 1;
    _tot_people = 0;
}

Store::~Store()
{
}

people_t
Store::putPeopleInto(Elevator &elevator)
{
	//Make sure elevator is at same store!
    assert (elevator.getCurrentStore() == storeNumber());

    people_t picked = 0;

	PeopleQueue tmpQueue;
	PeopleQueue *workingQueue = NULL;

	if (elevator.goingUp() && hasPassengersUp(elevator))
	{
		workingQueue = &_queue_up; 
	}
	else if (!elevator.goingUp() && hasPassengersDown(elevator))
	{
		workingQueue = &_queue_down; 
	}

	if (NULL != workingQueue)
	{
		while(elevator.freePlaces() > 0 && !workingQueue->empty())
		{
			Person &p = workingQueue->front();
			if (elevator.serves(p.destination()))
			{
				elevator.boardPassenger(p);
				picked++;
			}
			else
			{
				tmpQueue.push_back(p);
			}
			workingQueue->pop_front();
		}
		
		while (!tmpQueue.empty())
		{
			Person &p = tmpQueue.front();
			workingQueue->push_front(p);
			tmpQueue.pop_front();
		}
	}
	
	return picked;
}

people_t
Store::getPeopleFrom(Elevator &elevator, counter_t iteration)
{
	people_t picked = 0;
	while (elevator.hasPassengersTo(storeNumber()))
	{
		picked++;
		Person p = elevator.unboardPassenger();
		updateStats(iteration - p.elevatorTimer());
		enqueuePassenger(p, iteration);
	}
    return picked;
}

std::ostream& operator<<(std::ostream& os, const Store& obj)
{
	double hours, minutes, seconds;
    os  << "s" << setw(2) << setfill('0') << (int) obj.storeNumber() << " | ";
	counter2time (&hours, &minutes, &seconds, obj.minTime());
	os << setw(2) << setfill('0') << int(hours) << ":"
	   << setw(2) << setfill('0') << int(minutes) << ":"
	   << setw(2) << setfill('0') << int(seconds) << " ";
	
	counter2time (&hours, &minutes, &seconds, obj.maxTime());
	os << setw(2) << setfill('0') << int(hours) << ":"
	   << setw(2) << setfill('0') << int(minutes) << ":"
	   << setw(2) << setfill('0') << int(seconds) << " ";
	
	counter2time (&hours, &minutes, &seconds, obj.avgTime());
	os << setw(2) << setfill('0') << int(hours) << ":"
	   << setw(2) << setfill('0') << int(minutes) << ":"
	   << setw(2) << setfill('0') << int(seconds) << " ";
	
	os << " |";
	return os;
}
