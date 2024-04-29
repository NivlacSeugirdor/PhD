# The priority-based traveling backpacker problem: formulations and heuristics (Expert Systems with Applications)


One must cite: 

da Costa, C.R.; Nascimento, M.C.V. The priority-based traveling backpacker problem: formulations and heuristics. Expert Systems with Applications, v. 238, p. 121818, 2024.

Data and algorithms related to the manuscript entitled "The priority-based traveling backpacker problem: formulations and heuristics (Expert Systems with Applications)".


Organization:

     - [1] Folder Prizes with Gamma, Normal and Uniform distributions:
     
          . Has 5 spreadsheets with the values referring to the prizes used in the construction of the instances gene-
    rated for the TDOP model. They are arranged as follows:
     
                * First.ods: Prizes for the instances with suffix "a";
                
                * Second.ods: Prizes for the instances with suffix "b";
                
                * Third.ods: Prizes for the instances with suffix "c";
                
                * Fourth.ods: Prizes for the instances with suffix "d";
                
                * Fifth.ods: Prizes for the instances with suffix "e";
                
          . Each spreadsheets has 4 columns. The first one refers to the airports in the instances and the other three 
     are respectively the prizes obtained by the Uniform distribution (prefix "U"), Normal distribution (prefix "N") 
     and the Gamma distribution (prefix "G");
     
          . All the obtained prizes are in the range of 10 to 100.

     - [2] Folder Model Results:
          . For the folders [2.1] and [2.2] the following instances are used:
               * 1_6-1_G_a; 1_6-1_G_b; 1_6-1_G_c; 1_6-1_G_d; 1_6-1_G_e; 1_6-2_G_a; 1_6-2_G_b; 1_6-2_G_c; 1_6-2_G_d; 
      1_6-2_G_e; 1_6-3_G_a; 1_6-3_G_b; 1_6-3_G_c; 1_6-3_G_d; 1_6-3_G_e; 1_6-4_G_a; 1_6-4_G_b; 1_6-4_G_c; 1_6-4_G_d; 
      1_6-4_G_e; 1_6-5_G_a; 1_6-5_G_b; 1_6-5_G_c; 1_6-5_G_d; 1_6-5_G_e; 1_8-1_G_a; 1_8-1_G_b; 1_8-1_G_c; 1_8-1_G_d; 
      1_8-1_G_e; 1_8-2_G_a; 1_8-2_G_b; 1_8-2_G_c; 1_8-2_G_d; 1_8-2_G_e; 1_8-3_G_a; 1_8-3_G_b; 1_8-3_G_c; 1_8-3_G_d; 
      1_8-3_G_e; 1_8-4_G_a; 1_8-4_G_b; 1_8-4_G_c; 1_8-4_G_d; 1_8-4_G_e; 1_8-5_G_a; 1_8-5_G_b; 1_8-5_G_c; 1_8-5_G_d; 
      1_8-5_G_e; 2_6-1_G_a; 2_6-1_G_b; 2_6-1_G_c; 2_6-1_G_d; 2_6-1_G_e; 2_6-2_G_a; 2_6-2_G_b; 2_6-2_G_c; 2_6-2_G_d; 
      2_6-2_G_e; 2_6-3_G_a; 2_6-3_G_b; 2_6-3_G_c; 2_6-3_G_d; 2_6-3_G_e; 2_6-4_G_a; 2_6-4_G_b; 2_6-4_G_c; 2_6-4_G_d; 
      2_6-4_G_e; 2_6-5_G_a; 2_6-5_G_b; 2_6-5_G_c; 2_6-5_G_d; 2_6-5_G_e; 2_8-1_G_a; 2_8-1_G_b; 2_8-1_G_c; 2_8-1_G_d; 
      2_8-1_G_e; 2_8-2_G_a; 2_8-2_G_b; 2_8-2_G_c; 2_8-2_G_d; 2_8-2_G_e; 2_8-3_G_a; 2_8-3_G_b; 2_8-3_G_c; 2_8-3_G_d; 
      2_8-3_G_e; 2_8-4_G_a; 2_8-4_G_b; 2_8-4_G_c; 2_8-4_G_d; 2_8-4_G_e; 2_8-5_G_a; 2_8-5_G_b; 2_8-5_G_c; 2_8-5_G_d; 
      2_8-5_G_e;
               * 1_6-1_N_a; 1_6-1_N_b; 1_6-1_N_c; 1_6-1_N_d; 1_6-1_N_e; 1_6-2_N_a; 1_6-2_N_b; 1_6-2_N_c; 1_6-2_N_d; 
      1_6-2_N_e; 1_6-3_N_a; 1_6-3_N_b; 1_6-3_N_c; 1_6-3_N_d; 1_6-3_N_e; 1_6-4_N_a; 1_6-4_N_b; 1_6-4_N_c; 1_6-4_N_d; 
      1_6-4_N_e; 1_6-5_N_a; 1_6-5_N_b; 1_6-5_N_c; 1_6-5_N_d; 1_6-5_N_e; 1_8-1_N_a; 1_8-1_N_b; 1_8-1_N_c; 1_8-1_N_d; 
      1_8-1_N_e; 1_8-2_N_a; 1_8-2_N_b; 1_8-2_N_c; 1_8-2_N_d; 1_8-2_N_e; 1_8-3_N_a; 1_8-3_N_b; 1_8-3_N_c; 1_8-3_N_d; 
      1_8-3_N_e; 1_8-4_N_a; 1_8-4_N_b; 1_8-4_N_c; 1_8-4_N_d; 1_8-4_N_e; 1_8-5_N_a; 1_8-5_N_b; 1_8-5_N_c; 1_8-5_N_d; 
      1_8-5_N_e; 2_6-1_N_a; 2_6-1_N_b; 2_6-1_N_c; 2_6-1_N_d; 2_6-1_N_e; 2_6-2_N_a; 2_6-2_N_b; 2_6-2_N_c; 2_6-2_N_d; 
      2_6-2_N_e; 2_6-3_N_a; 2_6-3_N_b; 2_6-3_N_c; 2_6-3_N_d; 2_6-3_N_e; 2_6-4_N_a; 2_6-4_N_b; 2_6-4_N_c; 2_6-4_N_d; 
      2_6-4_N_e; 2_6-5_N_a; 2_6-5_N_b; 2_6-5_N_c; 2_6-5_N_d; 2_6-5_N_e; 2_8-1_N_a; 2_8-1_N_b; 2_8-1_N_c; 2_8-1_N_d; 
      2_8-1_N_e; 2_8-2_N_a; 2_8-2_N_b; 2_8-2_N_c; 2_8-2_N_d; 2_8-2_N_e; 2_8-3_N_a; 2_8-3_N_b; 2_8-3_N_c; 2_8-3_N_d; 
      2_8-3_N_e; 2_8-4_N_a; 2_8-4_N_b; 2_8-4_N_c; 2_8-4_N_d; 2_8-4_N_e; 2_8-5_N_a; 2_8-5_N_b; 2_8-5_N_c; 2_8-5_N_d; 
      2_8-5_N_e;
               * 1_6-1_U_a; 1_6-1_U_b; 1_6-1_U_c; 1_6-1_U_d; 1_6-1_U_e; 1_6-2_U_a; 1_6-2_U_b; 1_6-2_U_c; 1_6-2_U_d; 
      1_6-2_U_e; 1_6-3_U_a; 1_6-3_U_b; 1_6-3_U_c; 1_6-3_U_d; 1_6-3_U_e; 1_6-4_U_a; 1_6-4_U_b; 1_6-4_U_c; 1_6-4_U_d; 
      1_6-4_U_e; 1_6-5_U_a; 1_6-5_U_b; 1_6-5_U_c; 1_6-5_U_d; 1_6-5_U_e; 1_8-1_U_a; 1_8-1_U_b; 1_8-1_U_c; 1_8-1_U_d; 
      1_8-1_U_e; 1_8-2_U_a; 1_8-2_U_b; 1_8-2_U_c; 1_8-2_U_d; 1_8-2_U_e; 1_8-3_U_a; 1_8-3_U_b; 1_8-3_U_c; 1_8-3_U_d; 
      1_8-3_U_e; 1_8-4_U_a; 1_8-4_U_b; 1_8-4_U_c; 1_8-4_U_d; 1_8-4_U_e; 1_8-5_U_a; 1_8-5_U_b; 1_8-5_U_c; 1_8-5_U_d; 
      1_8-5_U_e; 2_6-1_U_a; 2_6-1_U_b; 2_6-1_U_c; 2_6-1_U_d; 2_6-1_U_e; 2_6-2_U_a; 2_6-2_U_b; 2_6-2_U_c; 2_6-2_U_d; 
      2_6-2_U_e; 2_6-3_U_a; 2_6-3_U_b; 2_6-3_U_c; 2_6-3_U_d; 2_6-3_U_e; 2_6-4_U_a; 2_6-4_U_b; 2_6-4_U_c; 2_6-4_U_d; 
      2_6-4_U_e; 2_6-5_U_a; 2_6-5_U_b; 2_6-5_U_c; 2_6-5_U_d; 2_6-5_U_e; 2_8-1_U_a; 2_8-1_U_b; 2_8-1_U_c; 2_8-1_U_d; 
      2_8-1_U_e; 2_8-2_U_a; 2_8-2_U_b; 2_8-2_U_c; 2_8-2_U_d; 2_8-2_U_e; 2_8-3_U_a; 2_8-3_U_b; 2_8-3_U_c; 2_8-3_U_d; 
      2_8-3_U_e; 2_8-4_U_a; 2_8-4_U_b; 2_8-4_U_c; 2_8-4_U_d; 2_8-4_U_e; 2_8-5_U_a; 2_8-5_U_b; 2_8-5_U_c; 2_8-5_U_d; 
      2_8-5_U_e.
          . [2.1] Folder PCTBP: The obtained results with the PCTBP using:
               *  an objective function minimizing the travel cost and penalizing not adding the available cities to the 
     route;
               *  "i" as a reference index to a city in the instance;
               * (first quartile[i]) and (median[i]) are respectively the first quartile and the median of the ticket va-
     lues to the city "i". Applying the following penalties:
                    :- (first quartile[i])*(1+ prize[i]/prize_max); 
                    :- (median[i])*(1+ prize[i]/prize_max). 
               * Prize min as 10, 50 and 100.
          . [2.2] Folder TDOP: The obtained results with the TDOP using:
               *  an objective function maximizing the prizes collected for passing the available cities;
               *  a restriction to limit the cost of travel based on data already available in the literature;  
               *  being "B" the budge for the user, it has a value for each instance in each instance according to the re-
     sults provided by Nakamura (2018). The value of "B" varies between 90% and 60% in steps of 10% of the value given by 
     Nakamura (2018).

     - [3] Folder Instances:
          . All instances that were used in the tests with the heuristics MSR_PCTBP and MSR_TDOP. The prizes in these instan-
     ces were generated through a normal distribution;
          . If are going to use them for the PCTB model, it will be necessary to change the value present in the third line 
     of the instances to the wanted value as a lower bound the collected prizes. Remembering that the value taken from the 
     instance must be passed as a parameter to the model.
          
     - [4] Folder Heuristics:
          .  Includes the implementation of heuristics for PCTBP (folder MSR_PCTBP) and for TDOP (folder MSR_TDOP).

     - [5] Folder Results - Heurists
          .  All the obtained results with the implemented heuristics. 
          
     - [6] Folder Models: 
          .  Includes the implementation of models for MSR_PCTBP (folder PCTBP -- [Median and Quartil] ) and for MSR_TDOP (folder TDOP).   
