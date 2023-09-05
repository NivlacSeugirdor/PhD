#ifndef _VARIABLEPRICE_H_
#define _VARIABLEPRICE_H_

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "VARIABLEINFO.h"

using namespace std;

class VARIABLEPRICE : public VARIABLEINFO
{
	public:
		VARIABLEPRICE(int i, int j, int t, int tl) : VARIABLEINFO(i, j, t, tl){}

		//Methods to provide informations
		double get_price(){return this.price;}

		//Methods to update informations
		void set_price(double price){this.price = price;}


	private:
		double price;
}

#endif