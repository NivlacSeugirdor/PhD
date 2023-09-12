#ifndef _FLIGHTTS_H_
#define _FLIGHTTS_H_

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include "AIRPORT.h"
#include "DESTINATION.h"

using namespace std;

// Check
class FLIGHTTS
{
	public:
		//Methods to provide informations
		int get_budget(){return this->budget;}//Gives the number of periods

		int get_departure_date(){return this->departureDate;}//Give the information about when the trip was started

		map<string, int> get_dest(){return this->dest;}//Provides the destinations to be selected

		vector<DESTINATION*> get_destinations(){return this->destinations;} //Return the selected destinations (?)

		AIRPORT* get_origin(){return this->origin;}//Return the first airport 

		int get_period_value(){return this->periodValue;}//Shows the weight to calculate the number of periods

		int get_slack_days(){return this->slackDays;}//Return the number of days of tolerance



		//Methods to update the informations
		void set_departure_date(int departureDate){this->departureDate = departureDate;}

		void set_dest(map<string, int> dest){this->dest = dest;}

		void set_destinations(vector<DESTINATION*> destinations){this->destinations = destinations;}

		void set_origin(AIRPORT *origin){this->origin = origin;}

		void set_period_value(int periodValue)
		{
			this->periodValue = periodValue;
			this->set_budget();
		}

		void set_slack_days(int slackDays){this->slackDays = slackDays;}



	private:
		int departureDate;//Day 0 of the trip
		AIRPORT *origin;//Origin airport and last destiny too
		map<string, int> dest;//Hash with the destinations to be selected (?)   
		vector<DESTINATION*> destinations; //Vector with the list of selected destinations (?)

		//Values by the program:
		int budget; //Quantity of periods!!!!
		int slackDays; //The tolerance
		int periodValue; //That thing is a weight to calculate the number of periods. Start with value 1. (?) 

		void set_budget()
		{
			this->budget = slackDays*periodValue; 

			int s = destinations.size();

			for(int i = 0; i < s; i++)
			{
				this->budget = this->budget + (destinations[i]->get_daysstay() + slackDays)*periodValue;
			}

			//cout << "Budget: " << this->budget << "\n";
		}



};

#endif