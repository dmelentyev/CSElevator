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
    
    _destinations = other._destinations;

}

Elevator::Elevator(string const &key)
{
    // Initial state
    _state.elevatorIs = resting;
    _state.current_store = 0;
    _state.passengers = 0;
	_state.most_distant_call = 0;
	_state.boarding_timer = 0;
    _destinations.resize(Configuration::get(string("building.stores")), 0);

    // Configure an elevator
    int config_value = -1;
    config_value = Configuration::get(key + ".skip.from");
    _skip_from = (config_value < 0) ? Configuration::get("building.stores") : config_value;
    config_value = Configuration::get(key + ".skip.to");
    _skip_to   = (config_value < 0) ? Configuration::get("building.stores") : config_value;
    config_value = Configuration::get(key + ".skip.when.full");
    _skip_when_full = (config_value < 0) ? false : (0 != config_value);
    config_value = Configuration::get(key + ".capacity");
    _capacity = (config_value < 0) ? 20 : config_value;
    config_value = Configuration::get(key + ".speed");
    _speed = (config_value < 0) ? 1 : config_value;
    config_value = Configuration::get(key + ".stop.time");
    _stop_time = (config_value < 0) ? 5 : config_value;
}

Elevator::~Elevator()
{
}

people_t
Elevator::boardPassengers(people_t number, store_t dest_store)
{
	assert (serves(dest_store));
	
    people_t boarded = freePlaces() < number ? freePlaces() : number;
    _destinations[dest_store] += boarded;
    _state.passengers += boarded;
    
    if ((dest_store > _state.current_store && _state.most_distant_call < dest_store)
        || (dest_store < _state.current_store && _state.most_distant_call > dest_store)
        )
    {
        setMostDistantCall(dest_store);
    }

    return boarded;
}

people_t
Elevator::unboardPassengers()
{
    people_t unboarded = _destinations[_state.current_store];
    _destinations[_state.current_store] = 0;
    _state.passengers -= unboarded;
    return unboarded;
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
		<< setw(2) << (int) (obj.capacity() - obj.freePlaces())
        << " |"; 
		return os;
}
