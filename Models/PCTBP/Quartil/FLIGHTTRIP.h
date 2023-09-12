#ifndef _FLIGHTTRIP_H_
#define _FLIGHTTRIP_H_

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "AIRPORT.h"
#include "FLIGHT.h"
using namespace std;

// Check
class FLIGHTTRIP
{
	public:
		AIRPORT depot;
		vector<FLIGHT> flights;
		int budget;

		FLIGHTTRIP(){budget = -1;}

		AIRPORT get_depot(){return depot;} 

		void set_depot(AIRPORT depot){this->depot = depot;}

		vector<FLIGHT> get_flights(){return flights;}

		void set_flights(vector<FLIGHT> flights){this->flights = flights;}

		int get_budget(){return budget;}

		void set_budget(int budget){this->budget = budget;}
};

#endif