#ifndef _VARIABLEINFO_H_
#define _VARIABLEINFO_H_

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// Check
class VARIABLEINFO
{
	public:
		VARIABLEINFO(int i, int j, int t, int tl, double value, double reducedCost)
		{
			this.i = i;
			this.j = j;
			this.t = t;
			this.tl = tl;
			this.reducedCost = reducedCost;
			this.value = value;
		}

		VARIABLEINFO(int i, int j, int t, int tl)
		{
			this.i = i;
			this.j = j;
			this.t = t;
			this.tl = tl;
		}

		//Methods to provide informations
		int get_i(){return this.i;}

		bool get_is_on_solution(){return this.isOnSolution;}

		int get_j(){return this.j;}

		int get_t(){return this.t;}

		int get_tl(){return this.tl;}

		double get_reduced_cost(){return this.reducedCost;}

		double get_value(){return this.value;}



		//Methods to update informations
		void set_i(int i){this.i = i;}

		void get_is_on_solution(bool isOnSolution){this.isOnSolution = isOnSolution;}

		void set_j(int j){this.j = j;}

		void set_t(int t){this.t = t;}

		void set_tl(int tl){this.tl = tl;}

		double set_reduced_cost(double reducedCost){this.reducedCost = reducedCost;}

		void set_value(double value){this.value = value;}


	private:
		int i, j, t, tl;
		double value;
		double reducedCost;
		bool isOnSolution;
}

#endif