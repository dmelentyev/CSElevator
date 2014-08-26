/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.cpp
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

#include <iostream>
#include "Configuration.h"
#include "ElevatorController.h"
#include "version.h"

using namespace std;

int main(int ARGC, char** ARGV)
{
	{ /** Program version info banner */
		string progpath(ARGV[0]);
		size_t lastslash = progpath.find_last_of('/');
		cout << progpath.substr(lastslash+1) << " v" << AutoVersion::FULLVERSION_STRING << endl;
	}

	// Load configuration
	Configuration::load("config.cfg");

	// Setup ElevatorController
	ElevatorController::setup();

	// Start controller
	ElevatorController::run();

	return 0;
}
