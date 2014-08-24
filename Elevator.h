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

enum ElevatorStates {resting, moving_up, moving_down, boarding_up, boarding_down};

struct ElevatorStateData
{
    ElevatorStates elevatorIs;
    unsigned char current_store;
    unsigned char boarding_timer;
    unsigned char passengers;
    unsigned char most_distant_call;
};

class Elevator
{
public:
    Elevator(string const &key);
    Elevator(const Elevator &other);
    virtual ~Elevator();
    inline bool goingUp() const {return (getCurrentState() != boarding_down && getCurrentState() != moving_down);}
    inline bool boardingEnds() const {return (_state.boarding_timer == 0);}
    inline unsigned char freePlaces() const {return _capacity - _state.passengers;};
    inline bool empty() const {return (0 == _state.passengers);};
    inline unsigned char capacity() const {return _capacity;};
    inline unsigned char getCurrentStore() const {return _state.current_store;}
    inline ElevatorStates getCurrentState() const {return _state.elevatorIs;}
    inline unsigned char getMostDistantCall() const {return _state.most_distant_call;}
    inline void setMostDistantCall(unsigned char store) {assert(_destinations.size() > store); _state.most_distant_call = store;}
    inline unsigned char serves(unsigned char store) const {return !(_skip_from <= store && _skip_to >= store );}
    inline bool hasPassengersTo(unsigned char store) const {return (_destinations[store] > 0);}

    unsigned char boardPassengers(unsigned char number, unsigned char dest_store);
    unsigned char unboardPassengers();
    void start(ElevatorStates new_state);
    void keep(ElevatorStates new_state);
    
protected:
private:
    
    unsigned char _skip_from; // inclusive
    unsigned char _skip_to;   // inclusive
    unsigned char _capacity;
    unsigned char _speed;
    unsigned char _stop_time;
    bool          _skip_when_full;
    ElevatorStateData _state;
    
    
    vector<unsigned int>  _destinations;
};

std::ostream& operator<<(std::ostream& os, const Elevator& obj);


#endif /* defined(_ELEVATOR_H_) */
