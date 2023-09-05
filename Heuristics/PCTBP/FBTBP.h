#ifndef _FBTBP_H_
#define _FBTBP_H_

#include <sstream>
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


class FBTBP
{
	public:

		int call_model(char arqv[60], vector<DESTINATION*> *destinations, vector<FLIGHT*> *flights, map<string, int> *data, FLIGHTTS *tripSearch, TIMELINE *tl, AIRPORT *depot, vector<int> toSolve, float *oldOb1, float uV, float alpha)
		{ 
			//Class
			vector<VARIABLE_Xijt*> varVec;
			vector<FLIGHT*> fromItoJ;
			VARIABLE_Xijt* varXijt;
			FLIGHT* fly;

			//Timer
			ChronoReal cl;
		   	double solvF = 0, readCons;

			//commom variables
			int slackPeriods, stayDays;
			int dSize, tlSize, travelTime, varVSize;
			char buffer[15];
			float gamma; 
			int i, j, t, f;// i, j, t for x_ijt, f to flights
			int M = 100000, gijt;
			int s1, s2;
			string cpy, cpy2;
			int min_xfiNt;
			int ***xijtTq; // <<<< quantity of ticket in each ijt

			float upperBound = uV;
			int ret = 0;

			cl.start();
			//Getting data
			
			//Updating values for quantity of destinations, size of the time line and the measuring of how many time of slack period
			dSize = destinations->size()-1;
			tlSize = tl->get_period_by_index().size();
			slackPeriods = tripSearch->get_period_value()*tripSearch->get_slack_days();
			
			
			fromItoJ.reserve(dSize);


			xijtTq = (int***)malloc(sizeof(int**)*(dSize+1));

			for(i = 0; i < dSize; i++)
			{
				xijtTq[i] = (int**)malloc(sizeof(int*)*(dSize+1));

				for(j = 0; j < dSize; j++) // + k
				{
					xijtTq[i][j] = (int*)malloc(sizeof(int)*(tlSize));

					for(t = 0; t < tlSize; t++)
						 xijtTq[i][j][t] = 0;
				}
			}


			for(i = 0; i < dSize; i++) // check
			{

				for(j = 0; j < dSize; j++) // check
				{

		            for(t = 0; t < tlSize; t++) // check
					{    

						fromItoJ.clear(); // check

		            	if(i!=j)
						{

							if(tl->get_period_by_index()[t+1] != NULL) // check
							{

								if(!tl->get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
			 					{

			 						cpy = (*destinations)[i]->get_destination()->get_code(); 
			 						
			 						if(tl->get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check 
			 						{


			 							s1 = tl->get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();
			 							
			 							for(f = 0; f < s1; f++) // check
			 							{
			 								fly = tl->get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];

			 								cpy2 = fly->get_to_where()->get_code();

			 								if( compare_string(cpy2, (*destinations)[j]->get_destination()->get_code()) == 0) // check
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

			int flag;

			//Initializing some variables of cplex
			try
			{

				xijtk = (IloIntVar****)malloc(sizeof(IloIntVar***)*(dSize+1));
				yi = (IloIntVar*)malloc(sizeof(IloIntVar)*(dSize+1));

				for(i = 0; i < dSize; i++)
				{
					xijtk[i] = (IloIntVar***)malloc(sizeof(IloIntVar**)*(dSize+1));

					sprintf(buffer, "y_%d", i);
					yi[i] = IloIntVar(env, 0, 1, buffer);

					for(j = 0; j < dSize; j++) // + k
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

			
				for(i = 0; i < dSize; i++)
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
				
				flag = 0;
				float norm = 1;


			    for(i = 0; i < dSize; i++) // check
				{			
					
					for(j = 0; j < dSize; j++) 
					{
		                
		                for(t = 0; t < tlSize; t++) 
						{    
							fromItoJ.clear(); 

							if(i!=j)
							{
								if(tl->get_period_by_index()[t+1] != NULL) 
								{

									if(!tl->get_period_by_index().at(t+1)->get_flights_by_from().empty()) 
				 					{

				 						cpy = (*destinations)[i]->get_destination()->get_code(); 
				 						
				 						if(tl->get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) 
				 						{


				 							s1 = tl->get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();
				 							
				 							for(f = 0; f < s1; f++) 
				 							{
				 								fly = tl->get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];

				 								cpy2 = fly->get_to_where()->get_code();

				 								if( compare_string(cpy2, (*destinations)[j]->get_destination()->get_code()) == 0) 
					 							{
					 								fromItoJ.push_back(fly);
					 							}
				 							}
				 						}
									}
								}

								//Sort -> get the smallest value without the use of merge
								fromItoJ = merge_sort(fromItoJ, fromItoJ.size());
								
								// Introduce the available flights for i to j in period t
								for(int k = 0; k < xijtTq[i][j][t]; k++)
								{
									travelTime = travel_periods(tripSearch, fromItoJ, k);

									stayDays = (*destinations)[j]->get_daysstay();

									gijt = travelTime + (stayDays*tripSearch->get_period_value()); // check
									
									
									if(flag)
										ob = ob + (fromItoJ[k]->get_price())*xijtk[i][j][t][k];// + K[j]*lijtk[i][j][t][k];
									else
									{
										ob = (fromItoJ[k]->get_price())*xijtk[i][j][t][k];// + K[j]*lijtk[i][j][t][k];
										flag = 1;
									}


									////cout << " + "<< fromItoJ[k]->get_price() << "*x[" << i << "][" << j << "][" << t << "][" << k << "]";
									varXijt = new VARIABLE_Xijt(i, j, t, k); // check

									varXijt->set_price(fromItoJ[k]->get_price()); // check
									varXijt->set_vars(fromItoJ[k]->get_from_where()->get_name(), fromItoJ[k]->get_to_where()->get_name(), fromItoJ[k]->get_departure_time(), fromItoJ[k]->get_arrive_time());
									varVec.push_back(varXijt); // check
								}
							}
						}
					}
				}


		        varVSize = varVec.size();

				fo = IloObjective(env, IloMinimize(env, ob)); // Objective function

				mod.add(fo); // add it to the model

				/***  Restrictions ***/

				
				//1st change - its only for the purpose of reselve a already existing sequence
				for(i = 0; i <= dSize; i++) //ok
				{
					if(toSolve[i] >= 0)
					{
						if( !(toSolve[i] == 0 && i > 0) )	
							mod.add(yi[toSolve[i]] == 1);
					}
					else{mod.add(yi[-toSolve[i]] == 0);}
				}

				// redo the 2 first restrictions

				
				/**
				*	2nd: Sum_{t} Sum_{j} Sum_{k} x_ijt <= 1, for all i
				*
				*	These restrictions require a departure on each airport only once.
			    */ 
				//2nd change - mod construct
				for(i = 0; i < dSize && toSolve[i+1] >= 0; i++) //-> airports to depart
				{
					_xijt_i = IloExpr(env);	
					flag = 0;
					for(j = 0; j < varVSize; j++) // verified flights
					{	
						if( (varVec[j]->get_j() == toSolve[i+1]) && (varVec[j]->get_i() == toSolve[i]) )
						{
							if(flag)
							{
								_xijt_i = _xijt_i + xijtk[varVec[j]->get_i()][varVec[j]->get_j()][varVec[j]->get_t()][varVec[j]->get_k()];
							}else{
								_xijt_i = xijtk[varVec[j]->get_i()][varVec[j]->get_j()][varVec[j]->get_t()][varVec[j]->get_k()];
								flag = 1;
							}
						}
					}


			    	mod.add(_xijt_i == yi[toSolve[i]]);	
				}


				for(i = 0; i < dSize && toSolve[i+1] >= 0; i++) //-> airports to depart
				{
					_xijt_i = IloExpr(env);	
					flag = 0;
					for(j = 0; j < varVSize; j++) // verified flights
					{	
						if( (varVec[j]->get_j() != toSolve[i+1]) && (varVec[j]->get_i() == toSolve[i]) )
						{
							if(flag)
							{
								_xijt_i = _xijt_i + xijtk[varVec[j]->get_i()][varVec[j]->get_j()][varVec[j]->get_t()][varVec[j]->get_k()];
							}else{
								_xijt_i = xijtk[varVec[j]->get_i()][varVec[j]->get_j()][varVec[j]->get_t()][varVec[j]->get_k()];
								flag = 1;
							}
						}
					}


			    	mod.add(_xijt_i == 0);	
				}


				/*
					================================================================================================================================================
					================================================================================================================================================
				*/

				/**
			    *	3rd: Sum_{t} Sum_{i} Sum_{k} x_ijtk <= 1, for all j
				*	
				*	These restrictions require an arrival from each airport only once.
				*/
				//3rd change  - mod construct
				for(j = 1; j < dSize && toSolve[j] >= 0; j++) // -> airports to arrive
				{
					_xijt_j = IloExpr(env);
					flag = 0;

					for(i = 0; i < varVSize; i++) // -> verified flights
					{
						if(  (varVec[i]->get_j() == toSolve[j]) && varVec[i]->get_i() == toSolve[j-1])
						{
							if(flag)
							{
								_xijt_j = _xijt_j + xijtk[varVec[i]->get_i()][varVec[i]->get_j()][varVec[i]->get_t()][varVec[i]->get_k()];
							}else{
								_xijt_j = xijtk[varVec[i]->get_i()][varVec[i]->get_j()][varVec[i]->get_t()][varVec[i]->get_k()];
								flag = 1;
							}
						}
					}

					sprintf(buffer, "rest_2_j_%d", j);
					_xijt_j.setName(buffer);

			    	mod.add(_xijt_j == yi[toSolve[j]]);
				}


				for(j = 1; j < dSize && toSolve[j] >= 0; j++) // -> airports to arrive
				{
					_xijt_j = IloExpr(env);
					flag = 0;

					for(i = 0; i < varVSize; i++) // -> verified flights
					{
						if(  (varVec[i]->get_j() == toSolve[j]) && varVec[i]->get_i() != toSolve[j-1])
						{
							if(flag)
							{
								_xijt_j = _xijt_j + xijtk[varVec[i]->get_i()][varVec[i]->get_j()][varVec[i]->get_t()][varVec[i]->get_k()];
							}else{
								_xijt_j = xijtk[varVec[i]->get_i()][varVec[i]->get_j()][varVec[i]->get_t()][varVec[i]->get_k()];
								flag = 1;
							}
						}
					}

					sprintf(buffer, "rest_2_j_%d", j);
					_xijt_j.setName(buffer);

			    	mod.add(_xijt_j == 0);
				}



			    /*
					================================================================================================================================================
					================================================================================================================================================
				*/

			    
			    _xijt_i = IloExpr(env);

			    for(i = 0; i < dSize; i++) // check
				{			
					for(j = 0; j < dSize; j++) // check
					{
		                for(t = 0; t < tlSize; t++) // check
						{    
							fromItoJ.clear(); // check

							if(i!=j)
							{
								if(tl->get_period_by_index()[t+1] != NULL) // check
								{

									if(!tl->get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
				 					{

				 						cpy = (*destinations)[i]->get_destination()->get_code(); 
				 						
				 						if(tl->get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check 
				 						{


				 							s1 = tl->get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();
				 							
				 							for(f = 0; f < s1; f++) // check
				 							{
				 								fly = tl->get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];

				 								cpy2 = fly->get_to_where()->get_code();

				 								if( compare_string(cpy2, (*destinations)[j]->get_destination()->get_code()) == 0) // check
					 							{
					 								fromItoJ.push_back(fly);
					 							}
				 							}
				 						}
									}
								}

								
								//Sort -> get the smallest value without the use of merge
								fromItoJ = merge_sort(fromItoJ, fromItoJ.size());
								
								// Introduce the available flights for i to j in period t

								for(int k = 0; k < xijtTq[i][j][t]; k++)
								{
									_xijt_i = _xijt_i + (fromItoJ[k]->get_price())*xijtk[i][j][t][k];
								}
							}
						}
					}
				}

				mod.add(_xijt_i <= upperBound);


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
			    	
			    	for(j = 1; j < dSize; j++) // check
			    	{
			    		
			    		for(t = 0; t < tlSize; t++) // check
			    		{
			    			fromItoJ.clear();
			    			if(i!=j) // check
				    		{
				    			if(tl->get_period_by_index()[t+1] != NULL) // check
				    			{
				    				if(!tl->get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
				    				{
				    					cpy = (*destinations)[i]->get_destination()->get_code(); 
									
										if(tl->get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check
										{
											s1 = tl->get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();

											for(f = 0; f < s1; f++) // check
											{
												fly = tl->get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];

												cpy2 = fly->get_to_where()->get_code();

												if( compare_string(cpy2, (*destinations)[j]->get_destination()->get_code()) == 0) // check
												{
													fromItoJ.push_back(fly);
												}
											}
										}   
				    				} 		
								}

								//Sort -> get the smallest value without the use of merge
								fromItoJ = merge_sort(fromItoJ, fromItoJ.size());

								//ATRIBUTE
								for(int k = 0; k < xijtTq[i][j][t]; k++)
								{
									travelTime = travel_periods(tripSearch, fromItoJ, k);

									stayDays = (*destinations)[j]->get_daysstay();

									gijt = travelTime + (stayDays*tripSearch->get_period_value()); // check

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
			    	
			    	for(j = 0; j < dSize; j++) // check
			    	{

			    		for(t = 0; t < tlSize; t++) // check
			    		{
			    			fromItoJ.clear();

			    			if(i!=j)
					    	{
					    		if(tl->get_period_by_index()[t+1] != NULL) // check
			    				{
		    						if(!tl->get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
		    						{
										cpy = (*destinations)[i]->get_destination()->get_code();

					    				if(tl->get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check
					    				{
					    					s1 = tl->get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();

				 							for(f = 0; f < s1; f++) // check
				 							{
				 								fly = tl->get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];

					 							cpy2 = fly->get_to_where()->get_code();

					 							if( compare_string(cpy2, (*destinations)[j]->get_destination()->get_code()) == 0) // check
					 							{
					 								fromItoJ.push_back(fly);
					 							}
				 							}
					    				}
		    						}
		    					}

					    		//Sort -> get the smallest value without the use of merge
			    				fromItoJ = merge_sort(fromItoJ, fromItoJ.size());

			    				for(int k = 0; k < xijtTq[i][j][t]; k++) // check
					    		{
					    			travelTime = travel_periods(tripSearch, fromItoJ, k);

					    			stayDays = (*destinations)[j]->get_daysstay();

					    			gijt = travelTime + (stayDays*tripSearch->get_period_value());
					    			
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
			    		if(tl->get_period_by_index()[t+1] != NULL) // check
			    		{	
			    			if(!tl->get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
			    			{
					    	
				    			cpy = (*destinations)[i]->get_destination()->get_code();

				    			if(tl->get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check
								{
									s1 = tl->get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();

									for(f = 0; f < s1; f++) // check
									{
										fly = tl->get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];

										cpy2 = fly->get_to_where()->get_code();

										if( compare_string(cpy2, (*destinations)[0]->get_destination()->get_code()) == 0) // check
										{
											fromItoJ.push_back(fly);
										}
									}
								}
			    			}
				    		
				    		//Sort -> get the smallest value without the use of merge
				    		fromItoJ = merge_sort(fromItoJ, fromItoJ.size());
			    			//atribute
				    		
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
			    	
			    	for(j = 0; j < dSize; j++) // check
			    	{
			    		
			    		for(t = 0; t < tlSize; t++) // check
			    		{
			    			fromItoJ.clear();
			    			if(i!=j) // check
				    		{
				    			if(tl->get_period_by_index()[t+1] != NULL) // check
				    			{
				    				if(!tl->get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
				    				{
				    					cpy = (*destinations)[i]->get_destination()->get_code(); 
									
										if(tl->get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check
										{
											s1 = tl->get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();

											for(f = 0; f < s1; f++) // check
											{
												fly = tl->get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];

												cpy2 = fly->get_to_where()->get_code();

												if( compare_string(cpy2, (*destinations)[j]->get_destination()->get_code()) == 0) // check
												{
													fromItoJ.push_back(fly);
												}
											}
										}   
				    				} 		
								}

								//Sort -> get the smallest value without the use of merge
								fromItoJ = merge_sort(fromItoJ, fromItoJ.size());

								//ATRIBUTE
								for(int k = 0; k < xijtTq[i][j][t]; k++)
								{
								    travelTime = travel_periods(tripSearch, fromItoJ, k);

									gijt = travelTime; // check
									
									mod.add( xijtk[i][j][t][k]*(*destinations)[j]->get_min_arr() <= di[i] + gijt);
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
			    	
			    	for(j = 1; j < dSize; j++) // check
			    	{
			    		
			    		for(t = 0; t < tlSize; t++) // check
			    		{
			    			fromItoJ.clear();
			    			if(i!=j) // check
				    		{
				    		    ////// cout <<"b\n";
				    			if(tl->get_period_by_index()[t+1] != NULL) // check
				    			{
				    				if(!tl->get_period_by_index().at(t+1)->get_flights_by_from().empty()) // check
				    				{
				    					cpy = (*destinations)[i]->get_destination()->get_code(); 
									
										if(tl->get_period_by_index()[t+1]->get_flights_by_from().count(cpy) > 0) // check
										{
											s1 = tl->get_period_by_index()[t+1]->get_flights_by_from().at(cpy).size();

											for(f = 0; f < s1; f++) // check
											{
												fly = tl->get_period_by_index()[t+1]->get_flights_by_from().at(cpy)[f];

												cpy2 = fly->get_to_where()->get_code();

												if( compare_string(cpy2, (*destinations)[j]->get_destination()->get_code()) == 0) // check
												{
													fromItoJ.push_back(fly);
												}
											}
										}   
				    				} 		
								}

								
								fromItoJ = merge_sort(fromItoJ, fromItoJ.size());

								//ATRIBUTE
								for(int k = 0; k < xijtTq[i][j][t]; k++)
								{
								    mod.add( di[j] <= xijtk[i][j][t][k]*(*destinations)[j]->get_max_dep() + (1-xijtk[i][j][t][k])*tlSize);
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
			    cplex.setParam(IloCplex::EpGap, 0.3);
			    cplex.setOut(env.getNullStream()); 


			    cl.stop();
			    readCons = cl.getTime();

			    cl.reset();
			    

			 	

				cplex.setParam(IloCplex::TiLim, 300.0); //3600
				j = cplex.solve();
			

			 	
			 	if(cplex.isPrimalFeasible())
			 	{
			 		//print_best_value(arqv, destinations, cplex, xijtk, varVec, di);
			 		*oldOb1 = (float)cplex.getObjValue();
			 		ret = 1;
			 	}else
			 	{
			 		ret = 0;
			 	}	
			 	free(yi);

			 	//exportLP(arqv, cplex);
				
			 	for(i = 0; i < dSize; i++)
			 	{
			 		for(j = 0; j < dSize; j++)
			 		{
			 			free(xijtTq[i][j]);

			 			for(t = 0; t < tlSize; t++)
			 				free(xijtk[i][j][t]);

			 			free(xijtk[i][j]);
			 		}
			 		free(xijtTq[i]);
			 		free(xijtk[i]);
			 	}

	 			free(xijtTq);
		 		free(xijtk);

		 		for(i = 0; i < varVec.size(); i++)
		 			delete varVec[i];

			 	varVec.clear();
				fromItoJ.clear();	

				env.end();


				//cplex.end();	

			}catch(IloException& ex) {
		     		cerr << "\nError: " << ex << endl;
		   	}catch (string er){
		      		cerr << "\nError \n" << er << "\n" << endl;
		   	}

			return ret;
		}

	private:

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

		//Method to save the model 
		static void exportLP(char arq[], IloCplex cplex) 
		{
			
			char fn[100];
			strcpy(fn, arq);
			strcat(fn, "_model_.lp");

			cplex.exportModel(fn);
			
		} 

		int get_period(int val , int t0)
		{
			return (int)ceil( (val-t0)/14400.00 );
		}

		void print_iteractions(char arc[60], int solve, clock_t start, clock_t end, IloCplex cplex, int it)
		{
			
			FILE *fp;
			char fn[100];//name of the archive to be read (fn = file's name)
			strcpy(fn, "Tabela_it_outputs.ods");

			fp = fopen(fn, "a+");

			fprintf(fp, "%s & ", arc); //Archive`s name
			fprintf(fp, "%lf & ", (double)(end-start));

			//fprintf(fp, "\n======================================================================================================\n");
			//fprintf(fp, "=========================              =====  Iteração %d  =====            ==========================\n", it);
			//fprintf(fp, "======================================================================================================\n");
			//fprintf(fp, "========================= Intervalo de tempo (s) %lf\n", (double)(end-start));
			//fprintf(fp, "======================================================================================================\n");

			if(solve)
			{
				fprintf(fp, "Solved\n");
				fprintf(fp, "Best value & %lf & ", cplex.getBestObjValue());
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
				fprintf(fp, "Not Solved\n");
				fprintf(fp, "Best value & %lf & Objective value & - & GAP & - \n", cplex.getBestObjValue());
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

		
		static void print_best_value(char arc[60], vector<DESTINATION*>* destinations, IloCplex cplex, IloIntVar ****xijtk, vector<VARIABLE_Xijt*> varVec, IloIntVar di[]) 
		{
			int i;

			int size = varVec.size();
			
			FILE *fp;

			char fn[100];//name of the archive to be read (fn = file's name)
			strcpy(fn, arc);
			strcat(fn, "_outputs.txt");

			fp = fopen(fn, "a+");

		    
			for(i = 0; i < size; i++)
			{
				if(cplex.getValue( xijtk[ varVec[i]->get_i() ][ varVec[i]->get_j() ][ varVec[i]->get_t() ][varVec[i]->get_k()]) > 0)
				{
					fprintf(fp, "x[%d][%d][%d][%d] - %f - Preço: %f - Dep %lf - Arr %lf\n", varVec[i]->get_i(), varVec[i]->get_j(), varVec[i]->get_t(), varVec[i]->get_k(), cplex.getValue(xijtk[varVec[i]->get_i()][varVec[i]->get_j()][varVec[i]->get_t()][varVec[i]->get_k()]), varVec[i]->get_price(), varVec[i]->get_dep(), varVec[i]->get_arr()); 	
				}
			}
			
		 	
			for(i = 0; i < destinations->size()-1; i++)
			{
				fprintf(fp, "di[%d]  - %d\n", i, (int)cplex.getValue(di[i]));
			}


			fprintf(fp, "Best = %lf\n", (double)cplex.getObjValue());
		    

			fclose(fp);
		} 


		static int travel_periods(FLIGHTTS *tripSearch, vector<FLIGHT*> fromItoJ, int k)
		{
			int arriveTime = (int)ceil((fromItoJ[k]->get_arrive_time() - tripSearch->get_departure_date())/14400.00); // 1 day equals to 6 periods of 14400 sec each
			int departureTime = (int)ceil((fromItoJ[k]->get_departure_time()  - tripSearch->get_departure_date())/14400.00);
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

};


#endif
