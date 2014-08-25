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
#include <cstdlib>
#include <time.h>

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

	vector<people_t> destinations(Store::maxStore()+1, 100);
	srand(time(NULL));
	// e.g. 100*100, skipping groung floor
	people_t max_people = (Store::maxStore()+1) * store_capacity;
	long s=0L;
	for (people_t p=0; p < max_people; p++)
	{
		s = rand() % Store::maxStore() +1; // 1..99
		if (0 == destinations[s])
		{
			long s1 = s;
			int inc = 1;

			//Search up or down?
			if (Store::maxStore() / 2 > s)
				inc = -1;
			
			for (; s < destinations.size() 
			    && s >= 0 
			    && destinations[s] == 0
			    && s != s1; s += inc)
			{
				if ((s+inc) == 0)
					s = destinations.size()-1;
				else if ((s+inc) == destinations.size())
					s = 0;
			}

			if (s == s1)
				break; // Done filling the initial queue
		}
		destinations[s]--;
		Person person;
		if (s > Store::maxStore())
			assert(s <= Store::maxStore());
		person.setDestination(s);
		getStore(0).enqueuePassenger(person, 0);
		getStore(0).updateStats(0);
	}

	bool elevatorsAreNotResting = true;
	
    do {
        // Check elevator calls
        for(store_t i=0; i < Store::maxStore(); i++)
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
                                && (el.freePlaces() || !el.skipWhenFull()))))
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
                    // handle empty, going up, no calls above (reverse down for lower calls)
                    else if (el.empty() 
                             && el.getMostDistantCall() < store)
                    {
                        el.start(moving_down);
                    }
                    // handle empty, going up, no calls above start resting
					else if (el.empty() 
                             && el.getMostDistantCall() == store)
                    {
						el.setMostDistantCall(el.restingStore());
						if (store < el.restingStore())
						{
							el.keep(moving_up);
						}
						else
						{
                    		el.start( (store > el.restingStore() ) ? moving_down : resting );
						}
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
                            || (getStore(store).hasPassengersDown()
                               && (el.freePlaces() || !el.skipWhenFull()))))
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
                    // Keep moving same direction otherwise
                    else if (el.empty() 
                             && el.getMostDistantCall() < store)
                    {
                        el.keep(moving_down);
                    }
                    // Start resting if it is a ground floor and there's nobody to serve
                    else if (el.empty() 
                             && el.restingStore() == store)
                    {
                        el.start(resting);
                    }
                    break;
                case boarding_up:
                case boarding_down:
                    // unboard
                    getStore(store).getPeopleFrom(el, iteration);
                    // if we were moving up - board more people going up (and vice versa)
                    getStore(store).putPeopleInto(el);

                    if (el.boardingEnds())
                    {
                        // if there's no further calls and no passengers - schedule for ground floor resting
                        if (store == el.getMostDistantCall()
                            && el.empty())
                        {
							if (store != el.restingStore ())
							{
		                        el.setMostDistantCall(el.restingStore ());
			                    el.start((store > el.restingStore ()) ? moving_down : moving_up);
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
                    else if(getStore(el.getCurrentStore()).hasPassengersDown())
                    {
                        el.start(boarding_down);
                    }
                    // Check for calls on served stores
                    else if (el.getMostDistantCall() != el.restingStore ())
                    {
                        el.start((el.getMostDistantCall() < el.restingStore ()) ? moving_down : moving_up);
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

		elevatorsAreNotResting=false;
		for (int i=0; i < getElevators().size(); i++)
		{
			elevatorsAreNotResting |= getElevators()[i].getCurrentState() != resting;
		}

    }
    while(getStore(0).hasPassengers() || elevatorsAreNotResting);
	cout << "Done" << endl;

	for(store_t s=0; s <= Store::maxStore(); s++)
	{
		cout << getStore(s) << endl;
	}
	cout << "Finished" << endl;
}