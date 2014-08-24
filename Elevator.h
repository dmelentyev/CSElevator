//
//  Elevator.h
//  CreditSwissElevator
//
//  Created by Dennis Melentyev on 8/21/14.
//  Copyright (c) 2014 ___DENNIS_MELENTYEV___. All rights reserved.
//

#ifndef __CreditSwissElevator__Elevator__
#define __CreditSwissElevator__Elevator__

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


#endif /* defined(__CreditSwissElevator__Elevator__) */
