//
//  Elevator.cpp
//  CreditSwissElevator
//
//  Created by Dennis Melentyev on 8/21/14.
//  Copyright (c) 2014 ___DENNIS_MELENTYEV___. All rights reserved.
//

#include "Elevator.h"

Elevator::Elevator(string const &key)
{
    // Initial state
    m_current_store = 0;
    m_passengers = 0;
    m_resting = true;
    
    // Configure an elevator
    int config_value = -1;
    config_value = Configuration::get(key + ".skip.from");
    m_skip_from = (config_value < 0) ? Configuration::MAX_STORES : config_value;
    config_value = Configuration::get(key + ".skip.to");
    m_skip_to   = (config_value < 0) ? Configuration::MAX_STORES : config_value;
    config_value = Configuration::get(key + ".skip.when.full");
    m_skip_when_full = (config_value < 0) ? false : (0 != config_value);
    config_value = Configuration::get(key + ".capacity");
    m_capacity = (config_value < 0) ? 20 : config_value;
    config_value = Configuration::get(key + ".speed");
    m_speed = (config_value < 0) ? 1 : config_value;
    config_value = Configuration::get(key + ".stop.time");
    m_stop_time = (config_value < 0) ? 5 : config_value;
}

Elevator::~Elevator()
{
}

unsigned char
Elevator::registerPassengers(unsigned char number, unsigned char dest_store)
{
    unsigned char boarded = freePlaces() < number?freePlaces():number;
    m_destinations[dest_store] += boarded;
    m_passengers += boarded;

    return boarded;
}

unsigned char
Elevator::unregisterPassengers()
{
    unsigned char unboarded = m_destinations[m_current_store];
    m_destinations[m_current_store] = 0;
    m_passengers -= unboarded;
    return unboarded;
}

void
Elevator::step()
{
    
}
