/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * Person.h
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

#ifndef _PERSON_H_
#define _PERSON_H_

#include "Configuration.h"

class Person
{
	public:
		Person()
	{
		resetElevatorTimer(0);
		setDestination(0);
	}
		Person(Person const& other)
	{
		resetElevatorTimer(other.elevatorTimer());
		setDestination(other.destination());
	}
		virtual ~Person()
	{
	}
		inline store_t destination() const {return _destination;}
		inline counter_t enqueued() const {return _time_in_elevator;}
		inline void setDestination(store_t dest)
	{
		_destination = dest;
	}
		inline void resetElevatorTimer(counter_t iteration) {_time_in_elevator = iteration;}
		inline void incElevatorTimer() {_time_in_elevator++;}
		inline counter_t elevatorTimer() const {return _time_in_elevator;}

	protected:

	private:
		store_t    _destination;
		counter_t _time_in_elevator;
};

typedef deque<Person> PeopleQueue;

#endif // _PERSON_H_

