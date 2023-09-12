#ifndef _AIRPORTDTO_H_
#define _AIRPORTDTO_H_

#include <iostream>
#include <string>
using namespace std;

// Check
class AIRPORTDTO
{
	public:
		string name;
		string city;
		string code;
		string country;
		string countryCode;

		AIRPORTDTO(){}

		AIRPORTDTO(string name, string city, string code, string country, string countryCode){
			this.name = name;
			this.city = city;
			this.code = code;
			this.country = country;
			this.countryCode = countryCode;
		}

		string get_name(){return name;}

		void set_name(string name){this.name = name;}

		string get_city(){return city;} 

		void set_city(string city){this.city = city;}

		string get_code(){return code;}

		void set_code(string code){this.code = code;}

		string get_country(){return country;}

		void set_country(string country){this.country = country;}

		string get_country_code(){return countryCode;}

		void set_country_code(string countryCode){this.countryCode = countryCode;}
}

#endif