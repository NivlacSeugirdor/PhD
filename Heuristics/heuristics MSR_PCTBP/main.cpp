#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector
#include <cstdio>
#include <ctime>

#include "DATA.h"
#include "GRASP.h"
#include "Chrono.h"
#define max_Times 50
#define INFI 99999



int main(int argc, char *argv[]){
    GRASP *gr;
    DATA *root;

    ChronoReal cl;
    double dur_R, dur_S;

    int max_It;
    float max_Size_GNG, max_Age_Edge, ep_N1, ep_Neig, err_Tax, lamb;
    float alpha = atof(argv[2]);;
  
    max_It = atoi(argv[3]);
    
    cl.start();

    

    root = new DATA(argv[1], alpha, max_It);
    root->read_arquive();


    cl.stop();

    dur_R = cl.getTime();
    cl.reset();

    for(int i = 0; i < 5; i++)
    {   
        cl.start();

        gr = new GRASP(root);
        gr->execute(i, argv[1]);

        cl.stop();

        dur_S = cl.getTime();

        gr->save(i, argv[1], max_It, dur_S+dur_R, i); // reading + execution

        cl.reset();
        
        delete gr;
    }
}
