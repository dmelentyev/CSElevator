/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * Elevator.cpp
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

#include "Elevator.h"
#include <sstream>
#include <iomanip>

Elevator::Elevator(const Elevator &other)
{
	_state = other._state;
	_skip_from = other._skip_from; // inclusive
	_skip_to = other._skip_to;   // inclusive
	_capacity = other._capacity;
	_speed = other._speed;
	_stop_time = other._stop_time;
	_skip_when_full = other._skip_when_full;
	_resting_store = other._resting_store;

	_destinations = other._destinations;

}

Elevator::Elevator(string const &key)
{
	// Initial state
	_state.elevatorIs = resting;
	_state.passengers = 0;
	_state.most_distant_call = 0;
	_state.boarding_timer = 0;
	_destinations.resize(Configuration::get("building.stores", 100));

	// Configure an elevator
	int config_value = -1;
	config_value = Configuration::get(key + ".skip.from", 100);
	_skip_from = (config_value < 0) ? Configuration::get("building.stores", 100) : config_value;
	config_value = Configuration::get(key + ".skip.to", 100);
	_skip_to   = (config_value < 0) ? Configuration::get("building.stores", 100) : config_value;
	config_value = Configuration::get(key + ".skip.when.full", 1);
	_skip_when_full = (config_value < 0) ? true : (0 != config_value);
	config_value = Configuration::get(key + ".capacity", 100);
	_capacity = (config_value < 0) ? 20 : config_value;
	config_value = Configuration::get(key + ".speed", 1);
	_speed = (config_value < 0) ? 1 : config_value;
	config_value = Configuration::get(key + ".stop.time", 5);
	_stop_time = (config_value < 0) ? 5 : config_value;
	config_value = Configuration::get(key + ".resting.store", 0);
	_resting_store = (config_value < 0) ? 0 : config_value;

	_state.current_store = _resting_store;
}

Elevator::~Elevator()
{
}

void
Elevator::boardPassenger(Person const &p)
{
	assert(serves(p.destination()));
	assert(freePlaces() > 0);

	_destinations[p.destination()].push_back(p);
	_state.passengers++;

	if ((p.destination() > _state.current_store && _state.most_distant_call < p.destination())
	    || (p.destination() < _state.current_store && _state.most_distant_call > p.destination())
	    )
	{
		setMostDistantCall(p.destination());
	}
}

Person const&
Elevator::unboardPassenger()
{
	Person const &p = _destinations[_state.current_store].front();
	_destinations[_state.current_store].pop_front();
	_state.passengers--;
	return p;
}

void
Elevator::keep(ElevatorStates new_state)
{
	assert (getCurrentState () == new_state);

	switch(new_state)
	{
		case resting:
			break;
		case moving_up:
			assert (_state.current_store < _state.most_distant_call ); 
			_state.current_store++;
			break;
		case moving_down:
			assert (_state.current_store > _state.most_distant_call ); 
			_state.current_store--;
			break;
		case boarding_up:
		case boarding_down:
			assert (_state.boarding_timer > 0 ); 
			_state.boarding_timer--;
			break;
	}
}

void
Elevator::start(ElevatorStates new_state)
{
	switch(new_state)
	{
		case resting:
		case moving_up:
		case moving_down:
			// Do nothing, just change the state after the switch 
			break;

		case boarding_up:
		case boarding_down:
			// Start boarding timer
			_state.boarding_timer = _stop_time -1;
			break;
	}
	_state.elevatorIs = new_state;
}

std::ostream& operator<<(std::ostream& os, const Elevator& obj)
{
	os  << "| "
		<< setw(2) << (int) obj.getCurrentStore()
		<< ":" << ((obj.getCurrentState () == resting) ? "**" : 
		           (obj.getCurrentState () == moving_up) ? "/\\" :
		           (obj.getCurrentState () == moving_down) ? "\\/" :
		           (obj.getCurrentState () == boarding_down) ? "Bv" :
			           "B^")
		<< setw(2) << (int) obj.getMostDistantCall()
		<< ":" 
		<< setw(2) << (int) obj.passengers()
		<< " |"; 
	return os;
}
