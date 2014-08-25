/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ElevatorController.h
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

#ifndef __CreditSwissElevator__ElevatorController__
#define __CreditSwissElevator__ElevatorController__

#include "Configuration.h"
#include "Elevator.h"
#include "Store.h"

class ElevatorController
{
    public:
        static void setup();
        static void run();
    
    protected:
    private:
        static ElevatorController _controller;
        static boost::mutex _mtx;

		static vector<Elevator> & getElevators(){return _controller._elevators;}
		static Store & getStore(store_t store){return _controller._stores[store];}
		static void addStore(Store const &st){_controller._stores.push_back(st);}
		static counter_t emulationStartTime()
		{
			return _controller._emulation_start_time;
		}
		static counter_t emulationStopTime()
		{
			return _controller._emulation_stop_time;
		}
    
		static counter_t currentIteration()
		{
			return _controller._iteration;
		}
    
		static void incIteration()
		{
			_controller._iteration++;
		}
    
		static bool printElevator()
		{
			return _controller._print_elevator;
		}
    
		counter_t _emulation_start_time;
		counter_t _emulation_stop_time;
		counter_t _iteration;
		bool      _print_elevator;
		PeopleQueue _lobby;
    
        vector<Elevator> _elevators;
        vector<Store>    _stores;
    
        ElevatorController();
        virtual ~ElevatorController();
};

#endif /* defined(__CreditSwissElevator__ElevatorController__) */
