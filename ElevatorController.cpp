/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ElevatorController.cpp
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

#include "ElevatorController.h"
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <iomanip>

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
        getElevators().push_back(el);
    }
    
    // Create stores
    int stores = Configuration::get("building.stores");
    
    for (int i=0; i < stores; ++i)
    {
        Store st(i);
        addStore(st);
    }
}

void
ElevatorController::run()
{
    // Morning. People come to the office
    // Create destinations for people on a ground floor
    store_t store_capacity = Configuration::get("building.store.capacity");
    counter_t iteration=0L;
	
    for (store_t s=0; s < getStore(0).maxStore(); s++)
    {
		getStore(0).addPassengersTo(s, store_capacity);
		cout << getStore(0) << endl;
    }
    
    while(getStore(0).hasPassengers()
          || getElevators()[0].getCurrentState() != resting
          || getElevators()[1].getCurrentState() != resting
          || getElevators()[2].getCurrentState() != resting
          )
    {
        // Check elevator calls
        for(store_t i=0; i < getStore(0).maxStore(); i++)
        {
			Store const &st = getStore(i);
            // Find nearest elevator that goes appropriate direction or is resting
            BOOST_FOREACH(Elevator &el, getElevators())
            {
                if (el.serves(st.storeNumber())
                    && st.hasPassengers()
                    && (
                         el.getCurrentState() == resting
                        || (
                            el.getCurrentStore() < st.storeNumber()
                            && el.getCurrentState() == moving_up
                            && el.getMostDistantCall() < st.storeNumber()
                            )
                        || (
                            el.getCurrentStore() > st.storeNumber()
                            && el.getCurrentState() == moving_down
                            && el.getMostDistantCall() > st.storeNumber()
                            )
                        )
                    )
                {
                    el.setMostDistantCall(st.storeNumber());
                }
            }
        }
        
        iteration++;

		double hours, minutes, seconds;

		hours = iteration/3600;
		minutes = (iteration - int(hours)*3600)/60;
		seconds = iteration - int(hours)*3600 - int(minutes)*60;
		
        cout << setw(2) << setfill('0') << int(hours) << ":"
			 << setw(2) << setfill('0') << int(minutes) << ":"
			 << setw(2) << setfill('0') << int(seconds) << " ";

		// Change states
        BOOST_FOREACH(Elevator &el, getElevators())
        {
			cout << "   " << el;
            store_t store = el.getCurrentStore();
            switch (el.getCurrentState())
            {
                case moving_up:
                    // Serve passengers going to/from current store
                    if (el.serves(store)
                        && ( el.hasPassengersTo(store) 
                            || (getStore(store).hasPassengersUp() 
                                && el.freePlaces ())))
                    {
                        el.start(boarding_up);
                        
                    }
                    // handle empty, last store up, has passengers in opposit direction
                    else if (el.serves(store)
                             && el.empty() 
                             && el.getMostDistantCall() == store
                             && getStore(store).hasPassengersDown())
                    {
                        el.start(boarding_down);
                    }
                    // handle empty, going up, no calls above (reverse down for resting or lower calls)
                    else if (el.empty() 
                             && el.getMostDistantCall() <= store)
                    {
						// If no calls - go down
						if (el.getMostDistantCall() == store)
						{
							el.setMostDistantCall(0);
						}
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
                        && ( el.hasPassengersTo(store) 
                            || getStore(store).hasPassengersDown()))
                    {
                        el.start(boarding_down);
                    }
                    // handle empty, last store down, has passengers in opposit direction
                    else if (el.serves(store)
                             && el.empty() 
                             && el.getMostDistantCall() == store
                             && getStore(store).hasPassengersUp())
                    {
                        el.start(boarding_up);
                    }
                    // handle empty, going down, a call comes from above with no calls below
                    else if (el.empty() 
                             && el.getMostDistantCall() > store)
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
                case boarding_up:
                case boarding_down:
                    // unboard
                    getStore(store).getPeopleFrom(el);
                    // if we were moving up - board more people going up (and vice versa)
                    getStore(store).putPeopleInto(el);

                    if (el.boardingEnds())
                    {
                        // if there's no further calls and no passengers - schedule for ground floor resting
                        if (store == el.getMostDistantCall()
                            && el.empty())
                        {
							if (store != 0)
							{
		                        el.setMostDistantCall(0);
			                    el.start(moving_down);
							}
							else 
							{
			                    el.start(resting);
							}
                        }
						else
						{
	                        el.start( el.goingUp() ? moving_up : moving_down);
						}
                    }
                    else
                    {
                        // keep(boarding) till the timer or start(moving_up/down)
                        el.keep(el.goingUp() ? boarding_up : boarding_down);
                    }

                    break;
                case resting:
                    // start boarding if call from ground floor
                    if(getStore(el.getCurrentStore()).hasPassengersUp())
                    {
                        el.start(boarding_up);
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
		cout << endl;
    }
		cout << "Done" << endl;   
}