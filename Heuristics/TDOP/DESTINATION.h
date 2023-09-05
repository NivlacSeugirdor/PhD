#ifndef _DESTINATION_H_
#define _DESTINATION_H_

#include <iostream>
#include <string>
#include "AIRPORT.h"
using namespace std;

// Check
class DESTINATION
{
	public:
		AIRPORT *destination;
		int daysStay;
		float prize;
		int min_arr;
		int max_dep;

		DESTINATION(){}

		DESTINATION(AIRPORT *airport, int daysStay, float prize, int min_arr, int max_dep)
		{
			this->daysStay = daysStay;
			this->destination = airport;	
			this->max_dep = max_dep;
			this->min_arr = min_arr;
			this->prize = prize;
		}



		int get_daysstay(){return daysStay;}

		AIRPORT* get_destination(){return destination;}

		float get_prize(){return prize;}
		
		int get_max_dep(){return max_dep;}
			
		int get_min_arr(){return min_arr;}





		void set_daysstay(int daysStay){this->daysStay = daysStay;}

		void set_destination(AIRPORT* destination){this->destination = destination;}

		void set_prize(float prize){this->prize = prize;}

		void set_max_dep(int max_dep){}
			
		void set_min_arr(){}
};

#endif