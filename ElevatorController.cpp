//
//  ElevatorController.cpp
//  CreditSwissElevator
//
//  Created by Dennis Melentyev on 8/21/14.
//  Copyright (c) 2014 ___DENNIS_MELENTYEV___. All rights reserved.
//

#include "ElevatorController.h"
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

ElevatorController ElevatorController::_controller;
boost::mutex ElevatorController::_mtx;

ElevatorController::ElevatorController()
{
}

ElevatorController::~ElevatorController()
{
}

void
ElevatorController::setup()
{
    scoped_lock<boost::mutex> lock(_mtx);

    // Create Elevators
    int elevators = Configuration::get("building.elevators");
    
    for (int i=1; i <= elevators; ++i)
    {
        Elevator el(string("elevator"+lexical_cast<std::string>(i)));
        _controller._elevators.push_back(el);
    }
    
    // Create stores
    int stores = Configuration::get("building.stores");
    
    for (int i=0; i < stores; ++i)
    {
        Store st(i);
        _controller._stores.push_back(st);
    }
}

void
ElevatorController::run()
{
    // Morning. People come to the office
    // Create destinations for people on a ground floor
    unsigned int store_capacity = Configuration::get("building.store.capacity");
    long long iteration=0L;
    
    for (unsigned char s=1; s < _controller._stores.size(); s++)
    {
        if(_controller._stores[0].setDestination(s, store_capacity) < store_capacity)
        {
            cerr << "Error scheduling " << store_capacity << " people for store " << (int) s << endl;
        }
    }
    
    while(_controller._stores[0].hasPassengers()
          || _controller._elevators[0].getCurrentState() != resting
          || _controller._elevators[1].getCurrentState() != resting
          || _controller._elevators[2].getCurrentState() != resting
          )
    {
        iteration++;
        cout << "#" << iteration
        << " el.1: "
        << ":" << (int) _controller._elevators[0].getCurrentStore()
        << ":" << (int) (_controller._elevators[0].capacity() - _controller._elevators[0].freePlaces())
        << " el.2: "
        << ":" << (int) _controller._elevators[1].getCurrentStore()
        << ":" << (int) (_controller._elevators[1].capacity() - _controller._elevators[0].freePlaces())
        << " el.3: "
        << ":" << (int) _controller._elevators[2].getCurrentStore()
        << ":" << (int) (_controller._elevators[2].capacity() - _controller._elevators[0].freePlaces())
        << endl;
        
        // Check elevator calls
        BOOST_FOREACH(Store &st, _controller._stores)
        {
            // Find nearest elevator that goes appropriate direction or is resting
            BOOST_FOREACH(Elevator &el, _controller._elevators)
            {
                if (el.serves(st.number())
                    && st.hasPassengers()
                    && (
                         el.getCurrentState() == resting
                        || (
                            el.getCurrentStore() < st.number()
                            && el.getCurrentState() == moving_up
                            && el.getMostDistantCall() < st.number()
                            )
                        || (
                            el.getCurrentStore() > st.number()
                            && el.getCurrentState() == moving_down
                            && el.getMostDistantCall() > st.number()
                            )
                        )
                    )
                {
                    el.setMostDistantCall(st.number());
                }
            }
        }
        
        // Change states
        BOOST_FOREACH(Elevator &el, _controller._elevators)
        {
            unsigned char store = el.getCurrentStore();
            switch (el.getCurrentState())
            {
                case moving_up:
                    // Serve passengers going to/from current store
                    if (el.serves(store)
                        && ( el.hasPassengersTo(store) || _controller._stores[store].hasPassengersUp()))
                    {
                        el.start(boarding);
                        
                    }
                    // handle empty, going up, no calls (reverse down for resting)
                    else if (el.freePlaces() == el.capacity() && el.getMostDistantCall() <= store)
                    {
                        el.start(moving_down);
                    }
                    // Keep moving same direction otherwise
                    else
                    {
                        el.keep(moving_up);
                    }
                    break;
                case moving_down:
                    // Serve passengers going to/from current store
                    if (el.serves(store)
                        && ( el.hasPassengersTo(store) || _controller._stores[store].hasPassengers()))
                    {
                        el.start(boarding);
                        
                    }
                    // handle empty, going down, a call comes from above with no calls below
                    else if (el.freePlaces() == el.capacity() && el.getMostDistantCall() > store)
                    {
                        el.start(moving_up);
                    }
                    // Start resting if it is a ground floor and there's nobody to serve
                    else if (0 == store)
                    {
                        el.start(resting);
                    }
                    // Keep moving same direction otherwise
                    else
                    {
                        el.keep(moving_down);
                    }
                    break;
                case boarding:
                    // unboard
                    _controller._stores[store].getPeopleFrom(el);
                    // if we were moving up - board more people going up (and vice versa)
                    _controller._stores[store].putPeopleInto(el);

                    if (el.boardingEnds())
                    {
                        // if there's no further calls and no passengers - schedule for ground floor resting
                        if (store == el.getMostDistantCall()
                            && el.freePlaces() == el.capacity())
                        {
                            el.setMostDistantCall(0);
                        }
                        
                        el.start( el.goingUp() ? moving_up : moving_down);
                    }
                    else
                    {
                        // keep(boarding) till the timer or start(moving_up/down)
                        el.keep(boarding);
                    }

                    break;
                case resting:
                    // start boarding if call from ground floor
                    if(_controller._stores[0].hasPassengersUp())
                    {
                        el.start(boarding);
                    }
                    // Check for calls on served stores
                    else if (el.getMostDistantCall() != 0)
                    {
                        el.start(moving_up);
                    }
                    // keep resting if none
                    else
                    {
                        el.keep(resting);
                    }

                    break;
            }
        }
    }
    
}