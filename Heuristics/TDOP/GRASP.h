#ifndef _GRASP_H_
#define _GRASP_H_

#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdio>
#include <stdlib.h>
#include <cmath>
#include <iomanip> 
#include <ctime>
#include <string>
#include <list>
#include "Chrono.h"


#include "DATA.h"
#include "FBTBP.h"

#define MAX 9999999


using namespace std;


class GRASP
{
	public:
		GRASP(DATA *root){this->root = root;}


		void execute(int it_G, char arc[50])
		{
			int it = 0, ac_Prize;
			int* w, s;
			float b_Bar = MAX, f_Bar = 0, val;
			char sv[60];

			sprintf(sv, "%s-%d", arc, it_G);

			ChronoReal cl_In;

			cl_In.start();

			int i = 0;


			best_Prize = 0;
			best_Cost = MAX;

			vector<int> sol;
			


			//Initializes the set of starting points
			while(root->info->flights[i].dept_Index == 0)
			{
				start_Points.push_back(i+1);
				i++;
			}
			
			i = 0;


			//FBTBP *model;
			float alpha;
			int ver, change;

			srand( time(NULL)*5988*rand() );

			while(it < root->max_It && cl_In.getTime() <= 360)
			{
				time_T = root->info->t0;
				
				val = 0;

				alpha = ((rand()%4)/10)  + 0.1;


				//Construction
				sol = construction(alpha, &val, &ac_Prize, root->mS->max_Cost, root->mS->pmin, best_Cost, best_Prize);

				if(sol.size()>0)
				{
					change = 0;
				
					for(int i = 1; i <= root->info->n_Airp; i++)
					{
						if(sol[i]==0){change = 1;}
						if(change){sol[i] = -sol[i];}
					}

					//Local Search -- If you want to test the integration with the model, simply remove the comments from lines 67 and 96 to 99.
					//model = new FBTBP();		
					//ver = model->call_model(sv, (root->mS->destinations), (root->mS->flights), &(root->mS->data), (root->mS->tripSearch), (root->mS->tl), (root->mS->dep), sol, &up, val, root->alpha);
					//delete model;
					//if(ver){val = up;}

								
					//Max Prize 2
					if(root->mS->max_Cost >= val && ac_Prize > best_Prize)
					{
						best_Cost = val;
						best_Prize = ac_Prize;

						best_Sol = sol;	
						it = 0;				
					}else
					{
						it++;
					}
					
					sol.clear();
				}
								
			}

			cl_In.stop();
			start_Points.clear();
		}
	


		int get_best_Cost(){return best_Cost;}

		vector<int> get_best_sol(){return best_Sol;}

		//vector<int> get_best_sol(){return best_Sol_T;}

		double get_Time(){return best_T;}

		void save(int ite, char arc[50], int it, double time, int act)
		{	
			FILE *fp;

			char fn[100];//name of the archive to be read (fn = file's name)

			sprintf(fn, "%s_it_%d", arc, it); 

			fp = fopen("results.ods", "a+");

			fprintf(fp, "%s & %d & %f & %d & %lf & ", fn, act, best_Cost, best_Prize, time);


			int  c = 1;

			for(int i = 0; i < best_Sol.size(); i++)
			{
				if(best_Sol[i]>0)
					c++;

				if(best_Sol[i]>=0)
					fprintf(fp, "%d ", best_Sol[i]);
				else
					fprintf(fp, "%d ", -best_Sol[i]);
			}

			fprintf(fp, " & %d ", c);

			if(ite < 4){fprintf(fp, " & ");}
			else{fprintf(fp, "\n");}
				


			best_Sol.clear();

			fclose(fp);
		}

	private:
		DATA *root;

		int max_it; // Maximum iterations without improvement
		
		float best_Cost;
		int  best_Prize;

		double best_T;
		double time_T;


		//Solution
		vector<int> best_Sol; // Best sequence of airports
	

		

		//To helps the construction of solution
		vector<int> start_Points;

		//sol = construction(root->alpha, &val, &ac_Prize, root->mS->max_Cost, root->mS->pmin, best_Cost, best_Prize);
		vector<int> construction(float alpha, float *val, int *prize, float upper_Val, int lower_Val, float best_Val, int best_Prize);
		
		void gen_sol(int* sols, int s);

		vector<int> take_options(float alpha, int pos, int* sols, float prev_Val, float upper_Val, int prev_flight);
};
#endif