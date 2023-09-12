#ifndef _VARIABLE_Xijt_H_
#define _VARIABLE_Xijt_H_

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class VARIABLE_Xijt
{
	public:
		VARIABLE_Xijt(int i, int j, int t, int k)
		{
			this->i = i;
			this->j = j;
			this->t = t;
			this->k = k; 
		}

		//Methods to provide informations
		int get_i(){return this->i;}

		int get_j(){return this->j;}

		double get_price(){return this->price;}

		int get_t(){return this->t;}

		int get_k(){return this->k;}

		string get_ori(){return ori;}

		string get_dest(){return dest;}

		double get_dep(){return dep;}

		double get_arr(){return arr;}

		//Methods to update informations
		void set_i(int i){this->i = i;}

		void set_j(int j){this->j = j;}

		void set_price(double price){this->price = price;}

		void set_t(int t){this->t = t;}

		void set_vars(string ori, string dest, double dep, double arr)
		{
			this->dep = dep;
			this->arr = arr;
			this->ori = ori;
			this->dest = dest;
		}

	private:
		int i, j, t, k;
		double price;
		double dep, arr;
		string ori, dest;

};

#endif