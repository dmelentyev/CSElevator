//
//  Elevator.cpp
//  CreditSwissElevator
//
//  Created by Dennis Melentyev on 8/21/14.
//  Copyright (c) 2014 ___DENNIS_MELENTYEV___. All rights reserved.
//

#include "Elevator.h"
#include <sstream>

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

unsigned char
Elevator::boardPassengers(unsigned char number, unsigned char dest_store)
{
	assert (serves(dest_store));
	
    unsigned char boarded = freePlaces() < number ? freePlaces() : number;
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

unsigned char
Elevator::unboardPassengers()
{
    unsigned char unboarded = _destinations[_state.current_store];
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
        case boarding:
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
            _state.elevatorIs = resting;
            break;
        case moving_up:
            _state.elevatorIs = moving_up;
            break;
        case moving_down:
            _state.elevatorIs = moving_down;
            break;
        case boarding:
            _state.boarding_timer = _stop_time;
            _state.elevatorIs = boarding;
            break;
    }
}

string const &
Elevator::prettyPrint(string &str)
{
	ostringstream os;
	
    os  << this
		<< ":" << (int) getCurrentStore()
        << ":" << ((getCurrentState () == resting) ? "*" : 
						(getCurrentState () == moving_up) ? "^" :
						(getCurrentState () == moving_down) ? "v" :
															  "B")
		<< (int) getMostDistantCall()
        << ":" 
		<< (int) (capacity() - freePlaces());
		str = os.str();
		return str;
}
