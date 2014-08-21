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

class Elevator
{
public:
    Elevator(string const &key);
    virtual ~Elevator();
    inline bool goingUp() const {return m_up;};
    inline unsigned char freePlaces() const {return m_capacity - m_passengers;};
    unsigned char registerPassengers(unsigned char number, unsigned char dest_store);
    unsigned char unregisterPassengers();
    void step();
    
protected:
private:
    
    unsigned char m_current_store;
    unsigned char m_skip_from; // inclusive
    unsigned char m_skip_to;   // inclusive
    unsigned char m_capacity;
    unsigned char m_speed;
    unsigned char m_stop_time;
    bool          m_skip_when_full;
    bool          m_up;
    bool          m_resting;
    unsigned char m_passengers;
    
    unsigned int  m_destinations[Configuration::MAX_STORES] = {0};
    boost::mutex  m_mtx;
};

#endif /* defined(__CreditSwissElevator__Elevator__) */
