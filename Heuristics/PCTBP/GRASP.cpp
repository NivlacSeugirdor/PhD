#include "GRASP.h"
#include <stdio.h>
#include <vector>
#include <algorithm>    // std::sort
#include <unistd.h> 
#include <math.h> 


vector<int> GRASP::construction(float alpha, float *val, int *prize, float upper_Val, int lower_Val, float best_Val, int best_Prize, float best_FO, float *actual_Penalty)
{
	int ck = 0;
	int i;
	vector<int> lrc; //restrict list of candidates 
	vector<int> *destination_List = new vector<int>(); // Airports to visit
	vector<int> sol;

	ChronoReal cl_gen;

	

	int size_Sol = root->info->n_Airp+1;
	int* op_Sols;


	op_Sols = (int*)malloc(sizeof(int)*(size_Sol)); 
	op_Sols[0] = 0;

	srand( time(NULL)*5987*rand() );

	

	int c;
	int reg;	
	int emp = 0;
	int interval;
	int prev = 0;
	float local_Val = 0;
	float local_Prize = 0;
	float local_Penalty = root->info->total_Penalty; 

	float comp_Sum;

	
	cl_gen.start();

	for(i = 0; cl_gen.getTime() <= 60 && !ck; i++)
	{
		
		gen_sol(op_Sols, size_Sol);

		for(int j = 0; j < size_Sol-1 && !emp; j++)
		{
	

			lrc = take_options(alpha, j, op_Sols, local_Val, upper_Val, prev);

			if( lrc.empty() ){
				emp = 1;
				local_Val = 0;
				local_Prize = 0;
				local_Penalty = root->info->total_Penalty;
				prev = 0;	
			}else
			{
				interval = lrc.size();

				c = rand()%interval;
				
				local_Val += root->info->flights[ lrc[c]-1 ].price;
				local_Prize += root->info->prizes[ op_Sols[j+1] ];

				local_Penalty -= root->info->penalties[ op_Sols[j+1] ];

				prev = lrc[c];

				comp_Sum = local_Penalty + local_Val;
				
				//Min Cost 1
				/**/
				if(op_Sols[j+1] == 0 && comp_Sum  <= best_FO && local_Prize >= lower_Val){ 
					j = size_Sol;
					ck = 1; 
				}else if(op_Sols[j+1] == 0 && (comp_Sum > best_FO  || local_Prize < lower_Val) )
				{
					j = size_Sol;
					local_Val = 0;
					local_Prize = 0;
					local_Penalty = root->info->total_Penalty;
					prev = 0;
				}/**/
				
				lrc.clear();
			}

		}
			
		emp = 0;
	}

 	

	if(ck)
	{
		sol.reserve(size_Sol);

		sol.push_back(0);


		for(i=1; i < size_Sol; i++)
			sol.push_back(op_Sols[i]);
		
		

		(*val) =  local_Val;
		(*prize) = local_Prize;
		(*actual_Penalty) = local_Penalty;
	}

	free(op_Sols);


	return sol;
}


void GRASP::gen_sol(int* sols, int s)
{
	vector<int> *destinations = new vector<int>();
	destinations->reserve(s);
	
	int nc = s-1;

	int i, c;

	for(i = 0; i < nc; i++)
		destinations->push_back(i);

	do{
		c = rand()%destinations->size();
	}while(c==0);
		

	sols[1] = (*destinations)[c];
	destinations->erase(destinations->begin()+c);

	for(i = 2; i < s; i++)
	{
		c = rand()%destinations->size();
		sols[i] = (*destinations)[c];
		destinations->erase(destinations->begin()+c);
	}

	delete destinations;
}

vector<int> GRASP::take_options(float alpha, int pos, int* sols, float prev_Val, float upper_Val, int prev_flight)
{
	int s;
	int connect;
	vector<int> lrc;

	if(sols[pos] == 0)
	{
		s = start_Points.size();
		lrc.reserve(s);

		
		for(int i = 0; i < s; i++)
		{
			if(root->info->flights[start_Points[i]-1].arr_Index == sols[pos+1] && root->info->flights[start_Points[i]-1].conn.size() > 0)
			{
				lrc.push_back(start_Points[i]);
			}
		}
	}else
	{	
		s = root->info->flights[prev_flight-1].conn.size();
		lrc.reserve(s);
		
		for(int i = 0; i < s; i++)
		{
			connect = root->info->flights[prev_flight-1].conn[i]-1;

			
			if( (root->info->flights[connect].arr_Index == sols[pos+1])
					&& (prev_Val+root->info->flights[connect].price <= upper_Val) )
			{
				if( (root->info->flights[connect].arr_Index == 0) || (root->info->flights[connect].conn.size() > 0) )
				{
					lrc.push_back(connect+1);
				}
			}
		}
	}


	s = lrc.size();


	if(s > 4)
	{
		s = (int)ceil(s*alpha);

		s = lrc.size()-s;
	}

	for(int i = 0; s!=lrc.size() && i < s; i++)
		lrc.pop_back();


	return lrc;
}