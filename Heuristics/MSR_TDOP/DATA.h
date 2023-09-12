#ifndef _DATA_H_
#define _DATA_H_

#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdio>
#include <stdlib.h> 
#include <math.h>
#include <map>
#include <stdlib.h>
#include <iomanip> 
#include <cstring>
#include <string>
#include <omp.h>


#include "AIRPORT.h"
#include "DESTINATION.h"
#include "FLIGHT.h"
#include "FLIGHTTS.h"
#include "PERIOD.h"
#include "TIMELINE.h"
#include "VARIABLE_Xijt.h"

using namespace std;

typedef struct
{
	int index; // Number of flight

	char dept[10]; // Departure airport name  
	char arr[10]; // Arrival airport name  

	int dept_Index; // Departure airport index 
	int arr_Index; // Arrival airport index

	double dep_D; // date of departure
	double arr_D; // date of arrival

	float price; // Price of passage

	vector<int> prev_Conn; // Predecessors
	vector<int> conn; // Connections
}flightsData;

typedef struct
{
	double t0; // Initial time
	int slack; // Slack of tolerance
	int n_Airp; // Number of airports

	vector<char*> airports; //Airports abbreviation
	vector<int> stay_Time; // Time of stay of in each place
	vector<int> prizes;

	int n_Flights; // Number of Flights
	int n_S; // Number of Flights leaving from the first airport
	flightsData* flights; // Flights data
}depot;

typedef struct 
{
	vector<DESTINATION*> *destinations;
	vector<FLIGHT*> *flights;
	map<string,  int> data;
	
	FLIGHTTS *tripSearch;
	TIMELINE *tl;
	AIRPORT *dep;

	int pmin;
	float max_Cost;
}modelStruct;

/**
	This class has all the data set about the instance.
*/
class DATA
{
	//..............................................................................................
	public:
		depot *info;
		modelStruct *mS;
		FILE *in_D; //Reading data
		
		
		float alpha; // Param alpha of
		int max_It; // Max iterations 
		int prob; // Value between 0 and 100 representing the probability of use the information of winning neuron of gng


		char arc[50];

		DATA(char arc[50], float alpha, int max_It){
			strcpy(this->arc, arc);
			this->max_It = max_It;
			this->alpha = alpha;
		}

		int check_index(char airp[10]) // OK
		{
			for(int i = 0; i < info->n_Airp; i++){
				if(strcmp(airp, info->airports[i])==0){
					return i;
				}
			}

			return -1;
		}


		int get_period(int val, int t0)
		{
			return (int)ceil( (val-t0)/14400.00 );
		}

		void insert_flight(int index, char dept[10], char arr[10], int dept_Ind, int arr_Ind, double dep_D, double arr_D, float price)
		{
			int ind = index-1;

			info->flights[ind].index = index;

			strcpy(info->flights[ind].dept, dept); 
			strcpy(info->flights[ind].arr, arr);

			info->flights[ind].dept_Index = dept_Ind;
			info->flights[ind].arr_Index = arr_Ind;

			info->flights[ind].dep_D = dep_D;
			info->flights[ind].arr_D = arr_D;

			info->flights[ind].price = price;
		}

		void read_arquive() //OK
		{
			info = (depot*)malloc(sizeof(depot));
			mS = (modelStruct*)malloc(sizeof(modelStruct));

			mS->data = std::map<std::string, int>();

			char arch[50];
			map<int, PERIOD*> example;

			sprintf(arch, "%s.txt", arc);
			
			
			AIRPORT *air;
			DESTINATION *dest;
			FLIGHT *f;
			PERIOD period;	

			double t0;
			int slack, time_Stay;
			char dept[10], arr[10];
			int extra, prize;
			int dept_I, arr_I;
			int dest_Qt, i;	
			
			double arrival, departure;
			int n_Flights;
			float price;
		 
			in_D = fopen(arch, "r");

			if(in_D==NULL)
				exit(1);

			//Read the slack time and de initial time
			

			fscanf(in_D, "%d %lf", &slack, &t0);
			fscanf(in_D, "%d", &extra);


			// To adjust the upper bound of costs, change the value on line 180.	
			//Mod Max prize 2
			mS->pmin = 1;
			mS->max_Cost = 0.6*extra;			

			info->t0 = t0;
			info->slack = slack; //*24*60*60;
			info->n_S = 0;

			//Create a tuple for the airport of origin 
			
			info->airports.push_back(new char(10));
			fscanf(in_D, "%s", dept); // Saves airport name
			strcpy(info->airports[0], dept);
			

			mS->dep = new AIRPORT();

			mS->dep->set_code(dept);

			
			info->stay_Time.push_back(0);
			
			info->prizes.push_back(0);
			
			int p1, p2;


			
			//get the number of destinations
			fscanf(in_D, "%d", &dest_Qt); // How many other airports are add to visit
			
			//get each one of the destinations

			mS->destinations = new vector<DESTINATION*>();
			mS->flights = new vector<FLIGHT*>();


			for(i = 1; i <= dest_Qt; i++)
			{
				
				info->airports.push_back(new char(10));
				fscanf(in_D, "%s", dept);
				fscanf(in_D, "%d", &time_Stay);
				fscanf(in_D, "%d", &prize);
				fscanf(in_D, "%d", &p1);
				fscanf(in_D, "%d", &p2);
				
				p1 = get_period(p1, t0);
				p2 = get_period(p2, t0);

				strcpy(info->airports[i], dept);
				
				air = new AIRPORT();

				air->set_code(dept);

				dest = new DESTINATION(air, time_Stay, prize, p1, p2);
				info->prizes.push_back(prize);
			

				

				mS->data.insert(make_pair(dept, time_Stay));

				
				mS->destinations->push_back(dest);

			
				info->stay_Time.push_back(time_Stay);
			}




			

			mS->tripSearch = new FLIGHTTS();
			mS->tl = new TIMELINE();

			mS->tripSearch->set_slack_days(slack);
			mS->tripSearch->set_departure_date(t0);
			mS->tripSearch->set_origin(mS->dep);
			mS->tripSearch->set_dest((mS->data));
			mS->tripSearch->set_destinations((*(mS->destinations) ));
			mS->tripSearch->set_period_value(6);


			info->n_Airp = dest_Qt+1;

			
			fscanf(in_D, "%d", &n_Flights); // Get the number of flights

			info->n_Flights = n_Flights;

			info->flights = (flightsData*)malloc(n_Flights*sizeof(flightsData));;

		
			// Creating the Connections
			for(int j = 0; j < n_Flights; j++)
			{
				fscanf(in_D, "%d %s %s", &i, dept, arr);
				dept_I = check_index(dept);
				arr_I = check_index(arr);
				
				fscanf(in_D, "%lf %lf %f", &departure, &arrival, &price);


				//---------------------

				air = new AIRPORT();
				air->set_code(dept);

				f = new FLIGHT();
				f->set_from_where(air);

				air = new AIRPORT();
				air->set_code(arr);

				f->set_to_where(air);

				f->set_departure_time(departure);
				f->set_arrive_time(arrival);

				f->set_price(price);

				mS->flights->push_back(f);


				//---------------------



				if(dept_I == 0)
					info->n_S++;

				insert_flight(i, dept, arr, dept_I, arr_I, departure, arrival, price);
			}
			
			mS->flights->pop_back();

			dest = new DESTINATION(mS->dep, 0, 0, 0, 0);

			mS->destinations->resize(info->n_Airp);

			mS->destinations->push_back(new DESTINATION());
			

			for(int i = info->n_Airp; i > 0; i--)
				(*mS->destinations)[i] = (*mS->destinations)[i-1];

			(*mS->destinations)[0] = dest;

			
			mS->tl->create_time_line(mS->tripSearch->get_budget()+1);

			example = mS->tl->get_period_by_index();

			period.set_flights_by_period_1((*(mS->flights)), t0, example);

			(*(mS->destinations))[0]->set_max_dep(example.size());

			// ==============================================================================================
			// Reading the flight connections
			// ==============================================================================================

			int depf, arrf; // previous flight, next flight
			
			while(!feof(in_D))
			{
				fscanf(in_D, "%d %d", &depf, &arrf);

				info->flights[depf-1].conn.push_back(arrf); // a connection to arrf

				info->flights[arrf-1].prev_Conn.push_back(depf); // may be preceded by depf in a solution
			}

			
			fclose(in_D);
		}

		// Vector with sequence of airports, sequence of tickets, runtime, total time of travel, cost of the travel
		void save_archive(vector<int> sol_A, vector<int> sol_T, double time_R, double time_T, float cost)
		{
			char arqv[700];
			int i, s;

			sprintf(arqv, "%s_a_%.2f_mi_%d.out", arc, alpha, max_It);//, max_Size_GNG, max_Age_Edge, ep_N1, ep_Neig, err_Tax, lamb);

			in_D  =  fopen(arqv, "a+");

			// ------------------------------------------------------------------
			fprintf(in_D, "================================================================================\n");
			fprintf(in_D, "Sequence of airports: ");

			s = sol_A.size();
			for(i = 0; i < s; i++)
			{
				fprintf(in_D, "%s ", info->airports[ sol_A[i] ] );
			}
			fprintf(in_D, "\n");

			// ...................................
			fprintf(in_D, "Sequence of tickets: ");

			s = sol_T.size();
			for(i = 0; i < s; i++)
			{
				fprintf(in_D, "%d ", sol_T[i]);
			}
			fprintf(in_D, "\n");

			// ...................................
			fprintf(in_D, "Runtime (s)    |   Time of Travel    |     Cost of Travel\n");
			fprintf(in_D, "  %lf    |   %.0lf    |     %.0f\n", time_R, time_T, cost);
			
			// ------------------------------------------------------------------
			fclose(in_D);
		}	

		//  total time of travel, cost of the travel, local iteration, global iteration 
		void save_iterations(double time_T, float cost, int it_L, int it_G)
		{
			char arqv[700];
			int i, s;

			sprintf(arqv, "%s_a_%.2f_mi_%d_IT.out", arc, alpha, max_It);

			in_D  =  fopen(arqv, "a+");

			// ------------------------------------------------------------------
			fprintf(in_D, "Global Iteration   |   Local Iteration    |   Time of Travel    |     Cost of Travel\n");
			fprintf(in_D, "      %d          |        %d            |   %.0lf    |     %.0f\n", it_G, it_L, time_T, cost);
			
			// ------------------------------------------------------------------
			fclose(in_D);
		}

	//..............................................................................................	
	//private:
		
};
#endif