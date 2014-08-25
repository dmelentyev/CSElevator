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
    int elevators = Configuration::get("building.elevators", 3);
	int cfg_val;
	cfg_val = Configuration::get("building.emulation.start", 8*3600);
	_controller._emulation_start_time = (cfg_val < 0) ? 8*3600 : cfg_val;
	cfg_val = Configuration::get("building.print.elevator", 0);
	_controller._print_elevator = (cfg_val < 0) ? false : (cfg_val > 0);
    cfg_val = Configuration::get("building.emulation.stop", 17*3600);
	_controller._emulation_stop_time = (cfg_val < 0) ? 17*3600 : cfg_val;
	_controller._iteration = _controller._emulation_start_time;
	
    for (int i=1; i <= elevators; ++i)
    {
        Elevator el(string("elevator"+lexical_cast<std::string>(i)));
        getElevators().push_back(el);
    }
    
    // Create stores
    int stores = Configuration::get("building.stores", 100);
    
    for (int i=0; i < stores; ++i)
    {
        Store st(i);
        addStore(st);
    }

	
    // Morning. People come to the office
    // Create destinations for people on a ground floor
    store_t store_capacity = Configuration::get("building.store.capacity", 100);
    counter_t iteration = emulationStartTime();

	vector<people_t> destinations(Store::maxStore()+1, 100);
	
	srand(time(NULL));

	// e.g. 100*100
	people_t max_people = (Store::maxStore()+1) * store_capacity;
	long s=0L;
	const int  max_diff = 60;
	vector<people_t> time_diffs(max_diff,0);
	for (people_t p=0; p < max_people; p++)
	{
		long coin = rand() % (1<<(max_diff/2));
		int diff = 0;
		for (int i=1; i<(max_diff/2);i++)
		{
			// Summ of all 1's
			diff += ((coin & (1<<i)) >> i);
		}

		time_diffs[max_diff-diff*2]++;
	}
	
	int diff = 0;
	counter_t min_iteration = iteration;
	for (people_t p=0; p < max_people; p++)
	{
		s = rand() % (Store::maxStore()); // 0..99
		if (0 == destinations[s])
		{
			for (s=Store::maxStore(); s >= 0 && destinations[s] == 0 ; s--);

			// If still not found => break
			if (destinations[s] == 0)
				break;
		}
		destinations[s]--;
		Person person;
		if (s > Store::maxStore())
			assert(s <= Store::maxStore());
		
		person.setDestination(s);
		while (time_diffs[diff] == 0 && diff < time_diffs.size()) 
			diff++;
		
		counter_t delta = iteration+(diff-max_diff/2)*60; // + rand() % 60;
		min_iteration = min(min_iteration, delta);
		person.resetElevatorTimer(delta);
		_controller._lobby.push_back(person);
		time_diffs[diff]--;
	}

	_controller._iteration = min_iteration; // Sync system time with first person in queue.
	cout << "*********************************" << endl;
	cout << Configuration::instance() << endl;
	cout << "*********************************" << endl;
	cout << "People to add: " << (int) max_people
		 << ", Added: " << _controller._lobby.size()
		 << endl; 
}

void
ElevatorController::run()
{
	bool elevatorsAreNotResting = true;
	
    do {

		if(printElevator())
		{
			double hours, minutes, seconds;
			counter2time (&hours, &minutes, &seconds, currentIteration());
			cout << setw(2) << setfill('0') << int(hours) << ":"
				 << setw(2) << setfill('0') << int(minutes) << ":"
				 << setw(2) << setfill('0') << int(seconds) << " "
				 << setw(6) << int(getStore(0).passengerQueueSize());
		}
		// If people are coming to lobby
		while (!_controller._lobby.empty() 
		    && _controller._lobby.front().enqueued() == currentIteration ())
		{
			Person p = _controller._lobby.front();
			getStore(0).enqueuePassenger(p, currentIteration());
			getStore(0).updateStats(0);
			_controller._lobby.pop_front();
		}

        // Check elevator calls
        for(store_t i=0; i < Store::maxStore(); i++)
        {
			Store &st = getStore(i);
			// Schedule people for exit
			st.scheduleForExit(currentIteration());
			
            // Find nearest elevator that goes appropriate direction or is resting
            BOOST_FOREACH(Elevator &el, getElevators())
            {
                if (el.serves(st.storeNumber())
                    && st.hasPassengers(el)
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
        
        incIteration();

		// Change states
        BOOST_FOREACH(Elevator &el, getElevators())
        {
			if(printElevator())
			{
				cout << "   " << el;
			}
			
            store_t store = el.getCurrentStore();
            switch (el.getCurrentState())
            {
                case moving_up:
                    // Serve passengers going to/from current store
                    if (el.serves(store)
                        && ( el.hasPassengersTo(store) 
                            || (getStore(store).hasPassengersUp(el) 
                                && (el.freePlaces() || !el.skipWhenFull()))))
                    {
                        el.start(boarding_up);
                        
                    }
                    // handle empty, last store up, has passengers in opposit direction
                    else if (el.serves(store)
                             && el.empty() 
                             && el.getMostDistantCall() == store
                             && getStore(store).hasPassengersDown(el))
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
                            || (getStore(store).hasPassengersDown(el)
                               && (el.freePlaces() || !el.skipWhenFull()))))
                    {
                        el.start(boarding_down);
                    }
                    // handle empty, last store down, has passengers in opposit direction
                    else if (el.serves(store)
                             && el.empty() 
                             && el.getMostDistantCall() == store
                             && getStore(store).hasPassengersUp(el))
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
                    else if (el.getMostDistantCall() < store)
                    {
                        el.keep(moving_down);
                    }
                    // Go restinging if we are at MostDistantCall, empty and no calls
                    else if (el.empty() 
                             && el.getMostDistantCall() == store
                             && el.restingStore() != store)
                    {
						el.setMostDistantCall(el.restingStore());
						if(el.getMostDistantCall() < store)
                    		el.keep(moving_down);
						else
                    		el.start(moving_up);
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
                    getStore(store).getPeopleFrom(el, currentIteration());
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
                    if(getStore(el.getCurrentStore()).hasPassengersUp(el))
                    {
                        el.start(boarding_up);
                    }
                    else if(getStore(el.getCurrentStore()).hasPassengersDown(el))
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
		if(printElevator())
		{
			cout << endl;
		}

		elevatorsAreNotResting=false;
		for (int i=0; i < getElevators().size(); i++)
		{
			elevatorsAreNotResting |= getElevators()[i].getCurrentState() != resting;
		}
    }
    while( !getStore(0).passengerQueueEmpty() || elevatorsAreNotResting || currentIteration() < emulationStopTime());
	if(printElevator())
	{
		cout << "Done" << endl;
	}

	for(store_t s=0; s <= Store::maxStore(); s++)
	{
		cout << getStore(s) << endl;
	}
	cout << "Finished" << endl;
}
