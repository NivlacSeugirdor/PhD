#include <ilcplex/ilocplex.h>
#include <ilconcert/ilomodel.h>
#include <iostream>
#include <string>
#include <cstring>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <map>
#include <math.h> 
#include <ctime>
#include <cfloat>
#include <algorithm>

#include "Chrono.h"
#include "AIRPORT.h"
#include "DESTINATION.h"
#include "FLIGHT.h"
#include "FLIGHTTS.h"
#include "PERIOD.h"
#include "TIMELINE.h"
#include "VARIABLE_Xijt.h"
ILOSTLBEGIN
using namespace std;

/**
*	Model paper of Mariá, Leandro and Jacques (JORS - 04/12/2016)
*	Model paper Mariá and Horácio
*   Implemented on Java by Katia Nakamura and to C++ by Calvin Rodrigues
*	New features 10/03/2021 by Calvin Rodrigues 
*					- A new FO considering penalties for does not visit some of presented locations;
*					- How to choose the day to arrive on an location.
*/



typedef struct chained{
	int qtd;
 	char city[4];
 	float* cost;
 	struct chained *under;
}chainedCity;


int compare_string(string s1, string s2)
{
	int check = -1;

	if(s1.size() == s2.size())
	{
		for(int i = 0; i < s2.size(); i++)
		{
			if(tolower(s1[i]) == tolower(s2[i])){check = 0;}
			else
			{
				check = -1;
				break;
			}
		}
	}

	return check;
}

void update_max_cost_idv(vector<string> city, vector<float> *cost, string comp, float val)
{
	for(int i = 0; i < city.size(); i++)
	{
		if(compare_string(city[i], comp)==0)
		{
			if((*cost)[i] < val)
				(*cost)[i] = val;
		}
	}
}

//using vector is giving problem and I really don`t know why, so... 
float* hell_oh_fucking_hell(int qtd, float *oldCostsVec, float newCost)
{
	float* costs = (float*)malloc(sizeof(float)*qtd);


	int i, j;

	for(i = 0; i < qtd; i++)
		costs[i] = 0;


	if(qtd >= 3)
	{
		for(i = 0; i < qtd-1 && oldCostsVec[i] < newCost; i++)
		{
			costs[i] = oldCostsVec[i];
		}

		costs[i] = newCost;

		for(; i < qtd-1; i++)
		{
			costs[i+1] = oldCostsVec[i];
		}
		
		free(oldCostsVec);
	}else
	{
		if(qtd == 1){costs[0] = newCost;}
		else if(qtd == 2)
		{
			if(oldCostsVec[0] > newCost)
			{
				costs[0] = newCost;
				costs[1] = oldCostsVec[0];
			}else
			{
				costs[0] = oldCostsVec[0];
				costs[1] = newCost;
			}
			free(oldCostsVec);
		}else{
			if(oldCostsVec[0] > newCost)
			{
				costs[0] = newCost;
				costs[1] = oldCostsVec[0];
			}else if(oldCostsVec[1] > newCost)
			{
				costs[0] = oldCostsVec[0];
				costs[1] = newCost;
				costs[2] = oldCostsVec[1];
			}else
			{
				costs[0] = oldCostsVec[0];
				costs[1] = oldCostsVec[1];
				costs[2] = newCost;
			}
			free(oldCostsVec);
		}
	}		


	
	return costs;
}

void save_costs(chainedCity* base, string city, float cost)
{
	chainedCity* aux = base;
	int stop = 0;
	
	while(aux!=NULL && !stop)
	{
		if(compare_string(aux->city, city)==0)
		{
			aux->qtd++; 

			aux->cost = hell_oh_fucking_hell(aux->qtd, aux->cost, cost);

			stop = 1;
		}
		aux = aux->under;
	}	
}



//Method to save the model 
static void exportLP(char arq[], IloCplex cplex) 
{
	char fn[80];
	strcpy(fn, arq);
	strcat(fn, "_model_.lp");

	cplex.exportModel(fn);
} 

int get_period(int val , int t0)
{
	return (int)ceil( (val-t0)/14400.00 );
}

void print_iteractions(char arc[], int solve, clock_t start, clock_t end, IloCplex cplex, int it)
{
	FILE *fp;
	char fn[50];//name of the archive to be read (fn = file's name)
	strcpy(fn, "Tabela_it_outputs.ods");

	fp = fopen(fn, "a+");

	fprintf(fp, "%s & ", arc); //Archive`s name
	fprintf(fp, "%lf & ", (double)(end-start));


	if(solve)
	{
		//fprintf(fp, "Solved\n");
		//fprintf(fp, "Best value & %lf & ", cplex.getBestObjValue());
		fprintf(fp, "%lf & ", cplex.getBestObjValue()); 

		if(cplex.getObjValue() > DBL_MIN)
		{
			fprintf(fp, "%lf & %lf\n", cplex.getObjValue(), (cplex.getObjValue() - cplex.getBestObjValue())/cplex.getObjValue() );
		}else
		{
			fprintf(fp, " - & - \n");
		}
	}else
	{
		//fprintf(fp, "Not Solved\n");
		//fprintf(fp, "Best value & %lf & Objective value & - & GAP & - \n", cplex.getBestObjValue());
		if(cplex.getBestObjValue() > DBL_MIN)
		{
			fprintf(fp, "%lf & - & - \n", cplex.getBestObjValue());
		}else
		{
			fprintf(fp, "- & - & - \n");
		}
	}

	fclose(fp);
}

//Must to adapt it to save the best result. 
static void print_best_value(char arc[], vector<DESTINATION*> destinations, IloCplex cplex, IloIntVar ****xijtk, vector<VARIABLE_Xijt*> varVec, IloIntVar di[], double interval, IloIntVar yi[], vector<float> penalty) 
{
	FILE *fp;

	char fn[50];//name of the archive to be read (fn = file's name)
	strcpy(fn, arc);
	strcat(fn, "_outputs.txt");

	int size = varVec.size();
	int i;

	fp = fopen(fn, "a+");

	fprintf(fp, "Time on seconds: %lf\n", interval);
	int ct[destinations.size()];
	int cities = 0;
	double pen = 0;
	double prize = 0;
	double cost = 0;

	for(i = 0; i < size; i++)
	{
		if(cplex.getValue( xijtk[ varVec[i]->get_i() ][ varVec[i]->get_j() ][ varVec[i]->get_t() ][varVec[i]->get_k()]) > 0.5)
		{
			fprintf(fp, "x[%d][%d][%d][%d] - Price: %f - Dep %lf - Arr %lf\n", varVec[i]->get_i(), varVec[i]->get_j(), varVec[i]->get_t(), varVec[i]->get_k(), varVec[i]->get_price(), varVec[i]->get_dep(), varVec[i]->get_arr()); 	
			//fprintf(fp, "x[%d][%d][%d][%d] - %d - Price: %f - Dep %lf - Arr %lf\n", varVec[i]->get_i(), varVec[i]->get_j(), varVec[i]->get_t(), varVec[i]->get_k(), (int)cplex.getValue(xijtk[varVec[i]->get_i()][varVec[i]->get_j()][varVec[i]->get_t()][varVec[i]->get_k()]), varVec[i]->get_price(), varVec[i]->get_dep(), varVec[i]->get_arr()); 	

			
			ct[varVec[i]->get_i()] = varVec[i]->get_j();
		

			prize += destinations[varVec[i]->get_i()]->get_prize();
			cost += varVec[i]->get_price();
			cities++;
		}
	}

	pen = (double)cplex.getObjValue() - cost;
	
	
	for(i = 0; i < destinations.size(); i++)
	{
		fprintf(fp, "di[%d]  - %d - Penalty - %f\n", i, (int)cplex.getValue(di[i]), penalty[i]);		
	}


	fprintf(fp, "Cost whith penalty = %.2lf - Cost = %.2lf - Prize = %.2lf Penalty = %.2lf\n", (double)cplex.getObjValue(), cost, prize, pen);

	fclose(fp);


	fp = fopen("table_results.txt", "a+");



	fprintf(fp, "%s & %.2lf & %.2lf & %.2lf & %.2lf & %d & 0 ", arc, (double)cplex.getObjValue(), cost, pen, prize, cities);

	for(i = 0; i < destinations.size(); )
	{
		if(ct[i] < destinations.size()) 
			fprintf(fp, "%d ", ct[i]);
		i = ct[i];
	}	

	fprintf(fp, "0\n");



	fclose(fp);
} 

//Method to read the input data - ok
void read_archive(char arc[], vector<DESTINATION*> *destinations, vector<FLIGHT*> *flights, map<string, int> *data, 
																		FLIGHTTS *tripSearch, TIMELINE *tl, AIRPORT *depot, int *gamma, int *pmin, int *ratio, vector<float> *cost)
{
	FILE *fp;

	char fn[50];//name of the archive to be read (fn = file's name)
	
	DESTINATION *dest;
	AIRPORT *air;
	FLIGHT *f;
	PERIOD period;
	map<int, PERIOD*> example;
	
	int qtd, index;
	char dept[4], arr[4];
	int slack, timeStay, destQt;
	int s1, s2;
	int prize, ga, pm;
	int p1, p2;
	double price;
	int t0;
	int arrival, departure;
	double maxPrize = 0, maxCost = 0; 


	vector<string> city;
	chainedCity* base = (chainedCity*)malloc(sizeof(chainedCity));

	base->under = NULL;
	base->qtd = 0;
	base->cost = NULL;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------



	strcpy(fn, arc);
	strcat(fn, "_conv2.txt");


	fp = fopen(fn, "r");
	
	if(fp == NULL)
		exit(1);

	fscanf(fp, "%d %d", &slack, &t0);// time of tolerance, initial time
	fscanf(fp, "%d", &pm); //Minimal prize
	fscanf(fp, "%s", dept); // name of initial airport
	fscanf(fp, "%d", &destQt); // quantity of airports to visit
	depot->set_code(dept); 
    
	city.push_back("origin");

	city.push_back(dept);

	strcpy(base->city, dept);
	

	city.pop_back();

	cost->push_back(0);

	*gamma = ga;
	*pmin = pm*1;

	chainedCity* aux = base;

	//Get the names and the time to stay in each airport
	for(int i = 0; i < destQt; i++)
	{
		aux->under = (chainedCity*)malloc(sizeof(chainedCity));
		aux->under->under = NULL;
		aux->under->qtd = 0;
		aux->under->cost = NULL;

		aux = aux->under;

		fscanf(fp, "%s", dept);
		fscanf(fp, "%d", &timeStay);
		fscanf(fp, "%d", &prize);

		city.push_back(dept);
		cost->push_back(0);
	
	

		air = new AIRPORT();
		
		if(prize > maxPrize)
			maxPrize = prize;

		fscanf(fp, "%d", &p1);
		fscanf(fp, "%d", &p2);

		p1 = get_period(p1, t0);
		p2 = get_period(p2, t0);

		air->set_code(dept);

		

		strcpy(aux->city, dept);

		dest = new DESTINATION(air, timeStay, prize, p1, p2);
		data->insert(pair<string, int>(dept, timeStay));
		destinations->push_back(dest);
	}

	city.push_back("origin");
	cost->push_back(0);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------

	tripSearch->set_slack_days(slack);
	tripSearch->set_departure_date(t0);
	tripSearch->set_origin(depot);
	tripSearch->set_dest((*data));
	tripSearch->set_destinations((*destinations));
	tripSearch->set_period_value(6);

	fscanf(fp, "%d", &qtd);
	//    //cout << qtd << "\n";



	for(int i = 0; i < qtd; i++)
	{
		f = new FLIGHT();

		fscanf(fp, "%d", &index);
		fscanf(fp, "%s", dept);
		fscanf(fp, "%s", arr);
		fscanf(fp, "%d %d %lf", &departure, &arrival, &price);

		save_costs(base, arr, price);

		if(price > maxCost)
			maxCost = price;

		air = new AIRPORT();
		air->set_code(dept);

		f->set_from_where(air);
		
		air = new AIRPORT();
		air->set_code(arr);

		f->set_to_where(air);

		f->set_departure_time(departure);
		f->set_arrive_time(arrival);

		f->set_price(price);

		flights->push_back(f);
	}

	

	flights->pop_back();


	aux = base;

	int i = 0;
	int val;

	while(aux!=NULL)
	{
		if(i>0)
		{
			val = (int)floor( (aux->qtd+1)/4);

			if( (aux->qtd+1)%4!=0)
			{
				(*cost)[i] = aux->cost[val];
			}else
			{
				(*cost)[i] = floor( (aux->cost[val] + aux->cost[val + 1])/2  );
			}
			free(aux->cost);
		}
		
		aux = aux->under;
		i++;
	}


	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	dest = new DESTINATION(depot, 0, 0, 0, 0);

	destinations->resize(destinations->size());

	destinations->push_back(new DESTINATION());
	
	for(int i = (destinations->size()-1); i > 0; i--)
		(*destinations)[i] = (*destinations)[i-1];

	(*destinations)[0] = dest;

	tl->create_time_line(tripSearch->get_budget()+1);

	example = tl->get_period_by_index();

	period.set_flights_by_period_1((*flights), t0, example);

	(*destinations)[0]->set_max_dep(example.size());

	*ratio = maxPrize;//maxCost/maxPrize;

	fclose(fp);
}

static int travel_periods(FLIGHTTS tripSearch, vector<FLIGHT*> fromItoJ, int k)
{
	int arriveTime = (int)ceil((fromItoJ[k]->get_arrive_time() - tripSearch.get_departure_date())/14400.00); // 1 day equals to 6 periods of 14400 sec each
	int departureTime = (int)ceil((fromItoJ[k]->get_departure_time()  - tripSearch.get_departure_date())/14400.00);
	return (arriveTime - departureTime);
}


vector<FLIGHT*> merge(vector<FLIGHT*> f, int l, int m, int r)
{
	int i, j, k;
	int n1, n2;
	n1 = m - l + 1;
	n2 = r-m;


	vector<FLIGHT*> L, R;



	for( i = 0; i < n1; i++)
		L.push_back(f[l+i]);


	for(i = 0; i < n2; i++)
		R.push_back(f[m+1+i]);

	i = 0;
	j = 0;
	k = l;

	while(i < n1 && j < n2)
	{
		if(L[i]->get_price() <= R[j]->get_price())
		{
			f[k] = L[i];
			i++;
		}else
		{
			f[k] = R[j];
			j++;
		}
		k++;
	}

	while(i < n1)
	{
		f[k] = L[i];
		i++;
		k++;
	}

	while(j < n2)
	{
		f[k] = R[j];
		j++;
		k++;
	}

	return f;
}

vector<FLIGHT*>merge_sort(vector<FLIGHT*> f, int n)
{
	int currSize, leftStart;
	int mid, rightEnd;

	

	for(currSize = 1; currSize < n; currSize = 2*currSize)
	{
		for(leftStart = 0; leftStart < n-1; leftStart+= 2*currSize)
		{
			mid = min(leftStart+currSize-1, n-1);
			rightEnd = min(leftStart + 2*currSize -1, n-1);

			f = merge(f, leftStart, mid, rightEnd);
		}
	}

	return f;
}


int main(int argc, char *argv[])
{
	//Class
	vector<DESTINATION*> destinations;
	vector<FLIGHT*> flights;
	vector<VARIABLE_Xijt*> varVec;
	vector<FLIGHT*> fromItoJ;
	map<string, int> data;
	FLIGHTTS tripSearch;
	VARIABLE_Xijt* varXijt;
	TIMELINE tl;
	AIRPORT depot;
	FLIGHT* fly;

    //cout.unsetf(ios::scientific);
	
	//Timer
	ChronoReal cl;
   	double solvF = 0, readCons;

	//commom variables
	int slackPeriods, stayDays;
	int dSize, tlSize, travelTime, varVSize;
	char buffer[15];
	int gamma;
	int pmin;
	int i, j, t, f;// i, j, t for x_ijt, f to flights
	int M = 100000, gijt;
	int s1, s2;
	string cpy, cpy2;
	int min_xfiNt;
	int ***xijtTq; // <<<< quantity of ticket in each ijt

	//double opt = atof(argv[2]);
	int ratio;
	vector<float> cost;


	cl.start();
	//Getting data
	read_archive(argv[1], &destinations, &flights, &data, &tripSearch, &tl, &depot, &gamma, &pmin, &ratio, &cost);
	//Updating values for quantity of destinations, size of the time line and the measuring of how many time of slack period
	dSize = destinations.size();
	tlSize = tl.get_period_by_index().size();
	slackPeriods = tripSearch.get_period_value()*tripSearch.get_slack_days();
	
	fromItoJ.reserve(dSize);

	

	xijtTq = (int***)malloc(sizeof(int**)*(dSize+1));

	for(i = 0; i <= dSize; i++)
	{
		xijtTq[i] = (int**)malloc(sizeof(int*)*(dSize+1));

		for(j = 0; j <= dSize; j++) // + k
		{
			xijtTq[i][j] = (int*)malloc(sizeof(int)*(tlSize));

			for(t = 0; t < tlSize; t++)
				 xijtTq[i][j][t] = 0;
		}
	}

	

	for(i = 0; i < dSize; i++) // check
	{
		for(j = 0; j <= dSize; j++) // check
		{
            
            for(t = 0; t < tlSize; t++) // check
			{    
				fromItoJ.clear(); // check

				if(j == dSize) // check
				{

					if(tl.get_period_by_index()[t+1] != NULL) // check 
 					{

 						if(!tl.get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
 						{

	 						cpy = destinations[i]->get_destination()->get_code();
	 						
	 						if(tl.get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check 
	 						{
	 							
	 							s1 = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();

	 							for(f = 0; f < s1; f++) // check
	 							{
	 								fly = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f]; // check

		 							cpy2 = fly->get_to_where()->get_code(); // check

		 							if( compare_string(cpy2, destinations[0]->get_destination()->get_code()) == 0) // check
		 							{	
		 								fromItoJ.push_back(fly); 
		 							}
	 							}
	 						}
 						}
					}
					xijtTq[i][j][t] = fromItoJ.size();
				}else if(i!=j)
				{
					if(tl.get_period_by_index()[t+1] != NULL) // check
					{

						if(!tl.get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
	 					{

	 						cpy = destinations[i]->get_destination()->get_code(); 
	 						
	 						if(tl.get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check 
	 						{


	 							s1 = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();
	 							
	 							for(f = 0; f < s1; f++) // check
	 							{
	 								fly = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];

	 								cpy2 = fly->get_to_where()->get_code();

	 								if( compare_string(cpy2, destinations[j]->get_destination()->get_code()) == 0) // check
		 							{
		 								fromItoJ.push_back(fly);
		 							}
	 							}
	 						}
						}
					}
					xijtTq[i][j][t] = fromItoJ.size();
				}
			}
		}
	}

		

	//cplex
	IloEnv env;
	IloModel mod(env);
	IloTimer timer(env);
	IloIntVar di[dSize+1];
	IloIntVar ****xijtk; //****xijt
	IloIntVar *yi;
	IloObjective fo;
	IloExpr _xijt_i(env), _xijt_j(env), sum_txijt(env), ob(env);
	IloExpr sum_pyi(env), sum_yi(env); 

	ob = 0*_xijt_i;

		

	//Initializing some variables of cplex
	try
	{

		xijtk = (IloIntVar****)malloc(sizeof(IloIntVar***)*(dSize+1));
		yi = (IloIntVar*)malloc(sizeof(IloIntVar)*(dSize+1));

		for(i = 0; i <= dSize; i++)
		{
			xijtk[i] = (IloIntVar***)malloc(sizeof(IloIntVar**)*(dSize+1));

			sprintf(buffer, "y_%d", i);
			yi[i] = IloIntVar(env, 0, 1, buffer);

			for(j = 0; j <= dSize; j++) // + k
			{
				xijtk[i][j] = (IloIntVar**)malloc(sizeof(IloIntVar*)*(tlSize)); 
				

				for(t = 0; t < tlSize; t++)
				{

					xijtk[i][j][t] = (IloIntVar*)malloc(sizeof(IloIntVar)*(xijtTq[i][j][t]));
					

					for(int k = 0; k < xijtTq[i][j][t]; k++)
					{
						sprintf(buffer, "x_%d_%d_%d_%d", i, j, t, k);
						xijtk[i][j][t][k] = IloIntVar(env, 0, 1, buffer);  
						
					}
					     
				}
			}
		}


		for(i = 0; i <= dSize; i++)
		{
			sprintf(buffer, "t_%d", i+1);
			di[i] = IloIntVar(env, 0, tlSize);
			di[i].setName(buffer);
		}
		
        // =====================================================================================================================
		/**
		*	Flow-based model
		*	FO construction
		*
		*	FO = Sum_{i\in N}Sum_{j\in N}Sum_{t\in T}Sum_{k\in P_{ijt}} c_{ij}^{tk}x_{ij}^{tk}
		*
		*	For this expression the three loops ahead will represent:
		*		- 1st loop (i):	the airports of departure; 
		*		- 2nd loop (j):	the airports of arrival;
		*		- 3rd loop (t): the period.
		*
		*   So, if:
		*		- i == j, there is nothing to do. T
		*		- i != j, add the cheapest from i to j on period t in expression;
		*		- j == |N|, check if there is a passage from i to the origin on period t.
		*				. If yes, add the cheapest passage in expression;
		*				. Otherwise, do nothing.
		*  ->rewrite
		*  An observation about this construction of FO:
		*	- The selection of only the cheapests passages reduce the number of possible 
		*	combinations to be verified. But this selection also allow the lost of possible better
		*	alternatives.
		*/
			
	    for(i = 0; i < dSize; i++) // check
		{			
			for(j = 0; j <= dSize; j++) // check
			{
                
                for(t = 0; t < tlSize; t++) // check
				{    
					fromItoJ.clear(); // check

					if(j == dSize) // check
					{

						if(tl.get_period_by_index()[t+1] != NULL) // check 
	 					{

	 						if(!tl.get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
	 						{

		 						cpy = destinations[i]->get_destination()->get_code();
		 						
		 						if(tl.get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check 
		 						{
		 							
		 							s1 = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();

		 							for(f = 0; f < s1; f++) // check
		 							{
		 								fly = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f]; // check

			 							cpy2 = fly->get_to_where()->get_code(); // check

			 							if( compare_string(cpy2, destinations[0]->get_destination()->get_code()) == 0) // check
			 							{	
			 								fromItoJ.push_back(fly); 
			 							}
		 							}
		 						}
	 						}
						}
						 
						//Sort -> get the smallest value
						fromItoJ = merge_sort(fromItoJ, fromItoJ.size());

						// Introduce the available flights for i to j in period t
						for(int k = 0; k < xijtTq[i][j][t]; k++)
						{
							travelTime = travel_periods(tripSearch, fromItoJ, k);
			    			
			    			gijt = travelTime;

							ob = ob + ((int)fromItoJ[k]->get_price() )*xijtk[i][j][t][k];
							
							varXijt = new VARIABLE_Xijt(i, j, t, k); // check

							varXijt->set_price(fromItoJ[k]->get_price()); // check
							varXijt->set_vars(fromItoJ[k]->get_from_where()->get_name(), fromItoJ[k]->get_to_where()->get_name(), fromItoJ[k]->get_departure_time(), fromItoJ[k]->get_arrive_time());
							varVec.push_back(varXijt); // check
						}
					}else if(i!=j)
					{
						if(tl.get_period_by_index()[t+1] != NULL) // check
						{

							if(!tl.get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
		 					{

		 						cpy = destinations[i]->get_destination()->get_code(); 
		 						
		 						if(tl.get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check 
		 						{


		 							s1 = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();
		 							
		 							for(f = 0; f < s1; f++) // check
		 							{
		 								fly = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];

		 								cpy2 = fly->get_to_where()->get_code();

		 								if( compare_string(cpy2, destinations[j]->get_destination()->get_code()) == 0) // check
			 							{
			 								fromItoJ.push_back(fly);
			 							}
		 							}
		 						}
							}
						}

						
						//Sort -> get the smallest value
						fromItoJ = merge_sort(fromItoJ, fromItoJ.size());
						
						// Introduce the available flights for i to j in period t

						for(int k = 0; k < xijtTq[i][j][t]; k++)
						{
							travelTime = travel_periods(tripSearch, fromItoJ, k);

							stayDays = destinations[j]->get_daysstay();

							gijt = travelTime + (stayDays*tripSearch.get_period_value()); // check

							ob = ob + ((int)fromItoJ[k]->get_price() )*xijtk[i][j][t][k];// + K[j]*lijtk[i][j][t][k];
							
							varXijt = new VARIABLE_Xijt(i, j, t, k); // check

							varXijt->set_price(fromItoJ[k]->get_price()); // check
							varXijt->set_vars(fromItoJ[k]->get_from_where()->get_name(), fromItoJ[k]->get_to_where()->get_name(), fromItoJ[k]->get_departure_time(), fromItoJ[k]->get_arrive_time());
							varVec.push_back(varXijt); // check
						}
					}
				}
			}
			
			ob = ob + (1-yi[i])*floor( ((cost[i])*(1 + destinations[i]->get_prize()))/(ratio*1.0 ));

			cost[i] = floor( ((cost[i])*(1 + destinations[i]->get_prize()))/(ratio*1.0 ));
		}
		

        varVSize = varVec.size();

		fo = IloObjective(env, IloMinimize(env, ob)); // Objective function

		mod.add(fo); // add it to the model

		/***  Restrictions ***/


		// redo the 2 first restrictions

		/**
		*	2nd: Sum_{t} Sum_{j} Sum_{k} x_ijt <= 1, for all i
		*
		*	These restrictions require a departure on each airport only once.
	    */ 

		for(i = 0; i < dSize; i++) // check -> airports to depart
	    {
	    	_xijt_i = IloExpr(env);
	    	for(j = 0; j < varVSize; j++) // check -> verified flights
	    	{
	    		if(varVec[j]->get_i() == i ) // check
	    		{
	    			_xijt_i = _xijt_i + xijtk[i][varVec[j]->get_j()][varVec[j]->get_t()][varVec[j]->get_k()]; // check
	    		}
	    	}
	    	
			sprintf(buffer, "rest_1_i_%d", i);
			_xijt_i.setName(buffer);

	    	mod.add(_xijt_i == yi[i]);	        
	    }


		/**
	    *	3rd: Sum_{t} Sum_{i} Sum_{k} x_ijtk <= 1, for all j
		*	
		*	These restrictions require an arrival from each airport only once.
		*/
		_xijt_j = IloExpr(env);
		
		for(j = 1; j <= dSize; j++) // check -> airports to arrive
	    {
	    	for(i = 0; i < varVSize; i++) // check -> verified flights
	    	{
	    		if(varVec[i]->get_j() == j) // check
	    		{
	    			_xijt_j = _xijt_j + xijtk[varVec[i]->get_i()][varVec[i]->get_j()][varVec[i]->get_t()][varVec[i]->get_k()]; // check
	    		}
	    	}

	    	sprintf(buffer, "rest_2_j_%d", j);
			_xijt_j.setName(buffer);

	    	mod.add(_xijt_j == yi[j]);

	        _xijt_j = IloExpr(env);
	    }


	    /**
	    *	4th: Sum_{i}Sum_{t} y_{it}p_{i} >= p_{min}
	    */

	    sum_pyi = IloExpr(env);

	    for(i = 0; i < dSize; i++)
	    {
	    	sum_pyi = sum_pyi + yi[i]*destinations[i]->get_prize();
	    }

	    mod.add(sum_pyi >= pmin);

	    /**
		*	5th: Sum_{j} Sum_{t} (Sum_{k} t*x_{ijtk} <= d_{i}, i\in N
		*		Estabilish a relation between x[i][j][t] and the integer variable d_{i} of departure time by the sum of t periods.
	    */

	    for(i = 0; i < dSize; i++) // check
	    {
	    	sum_txijt = IloExpr(env);
	    	for(j = 0; j < varVSize; j++) // check 
	    	{
	    		if(varVec[j]->get_i() == i && varVec[j]->get_j() != i ) // check
	    		{
		    		sum_txijt = sum_txijt + (varVec[j]->get_t()+1)*xijtk[i][varVec[j]->get_j()][varVec[j]->get_t()][varVec[j]->get_k()]; // check
		    	}
	    	}
	    	sprintf(buffer, "rest_5_i_%d", i);
			sum_txijt.setName(buffer);

	    	mod.add(sum_txijt == di[i]);
	    }
	    

	    /**
		*	6th: d_i + M*x^{tk}_{ij} - d_j <= M - (g^{tk}_{ij}+1) --> [d_{jk} >= d_{i} - (1-x^{tk}_{ij})*M], i,j\in N, t\in T & k in P_{ijt}
		*		Ensure that the next arrival period must be ever greater or equal to the departure plus travel time of predecessor. 		
	    */

	    for(i = 0; i < dSize; i++) // check
	    {
	    	
	    	for(j = 0; j <= dSize; j++) // check
	    	{
	    		
	    		for(t = 0; t < tlSize; t++) // check
	    		{
	    			fromItoJ.clear();
	    			if(j == dSize) // check
	    			{	
	    				
	    				if(tl.get_period_by_index()[t+1] != NULL) // check
			    		{
			    			
			    			if(!tl.get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
			    			{
			    				
			    				cpy = destinations[i]->get_destination()->get_code();
			    				
			    				
			    				if(tl.get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check
				    			{
				    				
				    				s1 = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();

									
									for(f = 0; f < s1; f++) // check
									{
										fly = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];
										
										cpy2 = fly->get_to_where()->get_code();
										
										if( compare_string(cpy2, destinations[0]->get_destination()->get_code()) == 0) // check
										{
											fromItoJ.push_back(fly);
										}
									}
				    			}		
			    			}
		    			}

		    			//Sort -> get the smallest value
		    			fromItoJ = merge_sort(fromItoJ, fromItoJ.size());

		    			for(int k = 0; k < xijtTq[i][j][t]; k++)
			    		{
			    			travelTime = travel_periods(tripSearch, fromItoJ, k);
			    			
			    			gijt = travelTime;
			    			
			    			//mod.add(di[i]-di[j] + M*xijtk[i][j][t][k] <= (M-(gijt)));
			    			mod.add(di[j] >= di[i] + gijt - (1-xijtk[i][j][t][k])*M); 
			    		}
			    	
		    		}else if(i!=j) // check
		    		{
		    			if(tl.get_period_by_index()[t+1] != NULL) // check
		    			{
		    				if(!tl.get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
		    				{
		    					cpy = destinations[i]->get_destination()->get_code(); 
							
								if(tl.get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check
								{
									s1 = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();

									for(f = 0; f < s1; f++) // check
									{
										fly = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];

										cpy2 = fly->get_to_where()->get_code();

										if( compare_string(cpy2, destinations[j]->get_destination()->get_code()) == 0) // check
										{
											fromItoJ.push_back(fly);
										}
									}
								}   
		    				} 		
						}

						//Sort -> get the smallest value
						fromItoJ = merge_sort(fromItoJ, fromItoJ.size());

						//ATRIBUTE
						for(int k = 0; k < xijtTq[i][j][t]; k++)
						{
							//cout << " Voo de " << i << " para " << j << " periodo " << t << " Partida " << fromItoJ[0]->get_departure_time() << " Chegada " << fromItoJ[0]->get_arrive_time() << " Valor " << fromItoJ[0]->get_price() << "\n"; 
							travelTime = travel_periods(tripSearch, fromItoJ, k);

							stayDays = destinations[j]->get_daysstay();

							gijt = travelTime + (stayDays*tripSearch.get_period_value()); // check
							
							//mod.add(di[i]-di[j] + M*xijtk[i][j][t][k] <= (M-(gijt))); // check
							mod.add(di[j] >= di[i] + gijt - (1-xijtk[i][j][t][k])*M);
						}
		    		}

	    		}
	    	}
	    }

	    /**
		*	7th: d[j] - d[i] + tlSize*x^{tk}_{ij} <= tlSize + g^{tk}_{ij} + slackPeriods
		*		
		*
	    */

	    for(i = 0; i < dSize; i++) // check
	    {
	    	
	    	for(j = 0; j <= dSize; j++) // check
	    	{

	    		for(t = 0; t < tlSize; t++) // check
	    		{
	    			fromItoJ.clear();

	    			if(j == dSize) // check
	    			{
	    				if(tl.get_period_by_index()[t+1] != NULL) // check
	    				{
    						if(!tl.get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
    						{
    							cpy = destinations[i]->get_destination()->get_code();

		    					if( tl.get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0 ) // check
		    					{
		    						s1 = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();

		 							for(f = 0; f < s1; f++) // check
		 							{
		 								fly = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];

			 							cpy2 = fly->get_to_where()->get_code(); 

			 							if( compare_string(cpy2, destinations[0]->get_destination()->get_code()) == 0) // check
			 							{
			 								fromItoJ.push_back(fly);
			 							}
		 							}
		    					}
  							}

	    					//Sort -> get the smallest value
	    					fromItoJ = merge_sort(fromItoJ, fromItoJ.size());


	    					for(int k = 0; k < xijtTq[i][j][t]; k++) // check
				    		{	
				    			//ADD
				    			travelTime = travel_periods(tripSearch, fromItoJ, k);

				    			gijt = travelTime;
				    			
				    			//mod.add(di[j] - di[i] + xijtk[i][j][t][k]*tlSize <= tlSize + gijt + slackPeriods);
				    			mod.add(di[j] <= di[i] + gijt + slackPeriods + (1-xijtk[i][j][t][k])*tlSize);
				    		}
	    				}
	    			}else if(i!=j)
			    	{
			    		if(tl.get_period_by_index()[t+1] != NULL) // check
	    				{
    						if(!tl.get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
    						{
								cpy = destinations[i]->get_destination()->get_code();

			    				if(tl.get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check
			    				{
			    					s1 = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();

		 							for(f = 0; f < s1; f++) // check
		 							{
		 								fly = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];

			 							cpy2 = fly->get_to_where()->get_code();

			 							if( compare_string(cpy2, destinations[j]->get_destination()->get_code()) == 0) // check
			 							{
			 								fromItoJ.push_back(fly);
			 							}
		 							}
			    				}
    						}
    					}

			    		//Sort -> get the smallest value
	    				fromItoJ = merge_sort(fromItoJ, fromItoJ.size());

	    				for(int k = 0; k < xijtTq[i][j][t]; k++) // check
			    		{
			    			travelTime = travel_periods(tripSearch, fromItoJ, k);

			    			stayDays = destinations[j]->get_daysstay();

			    			gijt = travelTime + (stayDays*tripSearch.get_period_value());
			    			
			    			//mod.add(di[j] - di[i] + tlSize*xijtk[i][j][t][k] <= tlSize + gijt + slackPeriods); // check
			    			mod.add(di[j] <= di[i] + gijt + slackPeriods + (1-xijtk[i][j][t][k])*tlSize);
			    		
			    		}
			    	}
			    }

		    }

	    }
 
	     /**
		*	8th:  d_{i} \leq (T - \bar{g}_{i}), i\in \mathcal{N}
	    */

	    min_xfiNt = tlSize;

	    for(i = 1; i < dSize; i++) // check
	    {

	    	for(t = 0; t < tlSize; t++) // check
	    	{

	    		fromItoJ.clear();
	    		if(tl.get_period_by_index()[t+1] != NULL) // check
	    		{	
	    			if(!tl.get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
	    			{
			    	
		    			cpy = destinations[i]->get_destination()->get_code();

		    			if(tl.get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check
						{
							s1 = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();

							for(f = 0; f < s1; f++) // check
							{
								fly = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];

								cpy2 = fly->get_to_where()->get_code();

								if( compare_string(cpy2, destinations[0]->get_destination()->get_code()) == 0) // check
								{
									fromItoJ.push_back(fly);
								}
							}
						}
	    			}
		    		
		    		//Sort -> get the smallest value
		    		fromItoJ = merge_sort(fromItoJ, fromItoJ.size());
	    			//atribute
		    		/*if(fromItoJ.size() > 0) // check
		    		{
		    			
		    			travelTime = travel_periods(tripSearch, fromItoJ, k);
		    			gijt = travelTime;
		    			
		    			if(gijt < min_xfiNt){min_xfiNt = gijt;} // check
		    		}*/
		    		for(int k = fromItoJ.size()-1; k >= 0 ; k--) // check
		    		{
		    			
		    			travelTime = travel_periods(tripSearch, fromItoJ, k);
		    			gijt = travelTime;
		    			
		    			if(gijt < min_xfiNt)
		    			{
		    				min_xfiNt = gijt;
		    			} // check
		    		}
	    		}


	    	}

	    	

	    	if(min_xfiNt < tlSize) // check
	    	{ 
	    		mod.add(di[i] <= (tlSize - min_xfiNt) ); // check
	    		
	    		min_xfiNt = tlSize;
	    	}else
	    	{			    			
	    		mod.add(di[i] <= tlSize);
	    	}
	    	

	    }

	    /**
		*	9th: d_{0} <= s
	    */

	  	mod.add(di[0] <= slackPeriods); // check




	  	/**
	  	*	10th:
	  	*		(1-L[j])x[i][j][t]P[j][1] <= d[i] + f[i][t][j];
	  	*/

	  	for(i = 0; i < dSize; i++) // check
	    {
	    	
	    	for(j = 0; j <= dSize; j++) // check
	    	{
	    		
	    		for(t = 0; t < tlSize; t++) // check
	    		{
	    			fromItoJ.clear();
	    			if(j == dSize) // check
	    			{	
	    				
	    				if(tl.get_period_by_index()[t+1] != NULL) // check
			    		{
			    			
			    			if(!tl.get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
			    			{
			    				
			    				cpy = destinations[i]->get_destination()->get_code();
			    				
			    				
			    				if(tl.get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check
				    			{
				    				
				    				s1 = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();

									
									for(f = 0; f < s1; f++) // check
									{
										fly = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];
										
										cpy2 = fly->get_to_where()->get_code();
										
										if( compare_string(cpy2, destinations[0]->get_destination()->get_code()) == 0) // check
										{
											fromItoJ.push_back(fly);
										}
									}
				    			}		
			    			}
		    			}

		    			//Sort -> get the smallest value
		    			fromItoJ = merge_sort(fromItoJ, fromItoJ.size());

		    			for(int k = 0; k < xijtTq[i][j][t]; k++)
			    		{
			    		    travelTime = travel_periods(tripSearch, fromItoJ, k);
			    			
			    			gijt = travelTime;
			    			
			    			mod.add( 0 <= di[i] + gijt);
			    		}
			    	
		    		}else if(i!=j) // check
		    		{
		    			if(tl.get_period_by_index()[t+1] != NULL) // check
		    			{
		    				if(!tl.get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
		    				{
		    					cpy = destinations[i]->get_destination()->get_code(); 
							
								if(tl.get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check
								{
									s1 = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();

									for(f = 0; f < s1; f++) // check
									{
										fly = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];

										cpy2 = fly->get_to_where()->get_code();

										if( compare_string(cpy2, destinations[j]->get_destination()->get_code()) == 0) // check
										{
											fromItoJ.push_back(fly);
										}
									}
								}   
		    				} 		
						}

						//Sort -> get the smallest value
						fromItoJ = merge_sort(fromItoJ, fromItoJ.size());

						//ATRIBUTE
						for(int k = 0; k < xijtTq[i][j][t]; k++)
						{
						    travelTime = travel_periods(tripSearch, fromItoJ, k);

							gijt = travelTime; // check
							
							mod.add( xijtk[i][j][t][k]*destinations[j]->get_min_arr() <= di[i] + gijt);
						}
		    		}

	    		}
	    	}
	    }


	  	/**
	  	*	11th:
	  	*		d[i] + f[i][t][j] <= x[i][j][t]P[j][2] + L[j]M;
	  	*/

	
	  	for(i = 0; i < dSize; i++) // check
	    {
	    	
	    	for(j = 1; j <= dSize; j++) // check
	    	{
	    		
	    		for(t = 0; t < tlSize; t++) // check
	    		{
	    			fromItoJ.clear();
	    			if(j == dSize) // check
	    			{	
	    			    if(tl.get_period_by_index()[t+1] != NULL) // check
			    		{
			    			
			    			if(!tl.get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
			    			{
			    				
			    				cpy = destinations[i]->get_destination()->get_code();
			    				
			    				
			    				if(tl.get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check
				    			{
				    				
				    				s1 = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();

									
									for(f = 0; f < s1; f++) // check
									{
										fly = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];
										
										cpy2 = fly->get_to_where()->get_code();
										
										if( compare_string(cpy2, destinations[0]->get_destination()->get_code()) == 0) // check
										{
											fromItoJ.push_back(fly);
										}
									}
				    			}		
			    			}
		    			}

		    			//Sort -> get the smallest value
		    			fromItoJ = merge_sort(fromItoJ, fromItoJ.size());

		    			for(int k = 0; k < xijtTq[i][j][t]; k++)
			    		{
			    		    mod.add( di[j] <= tlSize);
			    		}
			    	
		    		}else if(i!=j) // check
		    		{
		    		    if(tl.get_period_by_index()[t+1] != NULL) // check
		    			{
		    				if(!tl.get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
		    				{
		    					cpy = destinations[i]->get_destination()->get_code(); 
							
								if(tl.get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check
								{
									s1 = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();

									for(f = 0; f < s1; f++) // check
									{
										fly = tl.get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];

										cpy2 = fly->get_to_where()->get_code();

										if( compare_string(cpy2, destinations[j]->get_destination()->get_code()) == 0) // check
										{
											fromItoJ.push_back(fly);
										}
									}
								}   
		    				} 		
						}

						//Sort -> get the smallest value
						fromItoJ = merge_sort(fromItoJ, fromItoJ.size());

						//ATRIBUTE
						for(int k = 0; k < xijtTq[i][j][t]; k++)
						{
						    mod.add( di[j] <= xijtk[i][j][t][k]*destinations[j]->get_max_dep() + (1-xijtk[i][j][t][k])*tlSize);
						}
		    		}

	    		}
	    	}
	    }
	    // =====================================================================================================================
	    

	    clock_t start_it, end_it, start, end;
	    IloCplex cplex(mod);
	    

	    //Use only one Thread
	    cplex.setParam(IloCplex::Threads, 1);
	    cplex.setParam(IloCplex::EpGap, 0);

	    cl.stop();
	    readCons = cl.getTime();

	    cl.reset();
	    //start = clock();
	    


	 	for(i = 0; i < 1; i++)
	 	{
	 		//start_it = clock();	

	 		cl.start();

			cplex.setParam(IloCplex::TiLim, 3600.0); 
			j = cplex.solve();
			//end_it = clock();
			cl.stop();

			//print_iteractions(argv[1], j, 0, readCons+cl.getTime(), cplex, i);
			solvF += (readCons + cl.getTime());
			cl.reset();
	 	}

	 	//end = clock();
	 	
	 	//if(cplex.isPrimalFeasible())
	 	//{
	 		print_best_value(argv[1], destinations, cplex, xijtk, varVec, di, solvF, yi, cost);
	 	//}

	 	

	 	exportLP(argv[1], cplex);
			
		cplex.end();	

	}catch(IloException& ex) {
     		cerr << "\nError: " << ex << endl;
   	}catch (string er){
      		cerr << "\nError \n" << er << "\n" << endl;
   	}

	return 0;
}