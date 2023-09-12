#ifndef _TIMELINE_H_
#define _TIMELINE_H_

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "AIRPORT.h"
#include "FLIGHT.h"
#include "PERIOD.h"
#include "DESTINATION.h"
#include "FLIGHTTS.h" //FlightTripSearch

using namespace std;

// Check
class TIMELINE
{
	public:
		AIRPORT depot;
		vector<FLIGHT> flights;
		int budget;

		TIMELINE(){}

		

		void create_time_line(int totalTimeLine)
		{
			int index = 1;
			int time = 0;
			int i = 1;

			for(; i<= totalTimeLine; i++)
			{
				PERIOD *period = new PERIOD();
				period->set_start_time_date(time+60);

				time+=14400;
				period->set_end_time_date(time);
				this->periodByIndex.insert( pair<int, PERIOD*>(index, period) );
				
				index++;
			}

			lastPeriod = this->periodByIndex[i-1]->get_start_time_date();
		}

		int get_last_period(){return lastPeriod;}

		map<int, PERIOD*> get_period_by_index(){return this->periodByIndex;}

		void set_period_by_index(map<int, PERIOD*> periodByIndex){this->periodByIndex = periodByIndex;}

	private:
		map<int, PERIOD*> periodByIndex;	
		int lastPeriod;
};

#endif