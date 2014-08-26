/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * Elevator.h
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

#ifndef _ELEVATOR_H_
#define _ELEVATOR_H_

#include "Configuration.h"
#include "Person.h"

enum ElevatorStates {resting, moving_up, moving_down, boarding_up, boarding_down};

struct ElevatorStateData
{
	ElevatorStates elevatorIs;
	store_t   current_store;
	counter_t boarding_timer;
	people_t  passengers;
	store_t   most_distant_call;
};

class Elevator
{
	public:
		Elevator(string const &key);
		Elevator(const Elevator &other);
		virtual ~Elevator();
		inline bool goingUp() const {return (getCurrentState() != boarding_down && getCurrentState() != moving_down);}
		inline bool boardingEnds() const {return (_state.boarding_timer == 0);}
		inline bool skipWhenFull() const {return _skip_when_full;}
		inline people_t freePlaces() const {return _capacity - _state.passengers;};
		inline bool empty() const {return (0 == _state.passengers);};
		inline people_t capacity() const {return _capacity;};
		inline people_t passengers() const {return _state.passengers;};
		inline store_t restingStore() const {return _resting_store;}
		inline store_t getCurrentStore() const {return _state.current_store;}
		inline ElevatorStates getCurrentState() const {return _state.elevatorIs;}
		inline store_t getMostDistantCall() const {return _state.most_distant_call;}
		inline void setMostDistantCall(store_t store) 
	{
		if (_destinations.size() <= store)
			assert(_destinations.size() > store); 
		_state.most_distant_call = store;
	}
		inline bool serves(store_t store) const 
	{
		if (_destinations.size() <= store)
			assert(_destinations.size() > store); 
		return !(_skip_from <= store && _skip_to >= store );
	}
		inline bool hasPassengersTo(store_t store) const 
	{
		if (_destinations.size() <= store)
			assert(_destinations.size() > store); 
		return (_destinations[store].size() > 0);
	}

		void boardPassenger(Person const& p);
		Person const& unboardPassenger();
		void start(ElevatorStates new_state);
		void keep(ElevatorStates new_state);

	protected:
	private:

		store_t _skip_from; // inclusive
		store_t _skip_to;   // inclusive
		store_t _resting_store;
		people_t _capacity;
		counter_t _speed;
		counter_t _stop_time;
		bool      _skip_when_full;
		ElevatorStateData _state;


		vector<PeopleQueue>  _destinations;
};

std::ostream& operator<<(std::ostream& os, const Elevator& obj);


#endif /* defined(_ELEVATOR_H_) */
