//
//  ElevatorController.h
//  CreditSwissElevator
//
//  Created by Dennis Melentyev on 8/21/14.
//  Copyright (c) 2014 ___DENNIS_MELENTYEV___. All rights reserved.
//

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
		static Store & getStore(unsigned char store){return _controller._stores[store];}
		static void addStore(Store const &st){_controller._stores.push_back(st);}
    
        vector<Elevator> _elevators;
        vector<Store>    _stores;
    
        ElevatorController();
        virtual ~ElevatorController();
    
};

#endif /* defined(__CreditSwissElevator__ElevatorController__) */
