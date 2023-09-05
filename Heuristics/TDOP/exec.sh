clear
clear
clear
clear
clear
echo "---> Compilando ---"
rm artMerg.out
rm *.ods
rm *outputs.txt
rm *.lp

#g++ Chrono.h DATA.h GNG.h GNG.cpp Local_Search.h Local_Search.cpp IGAS.h IGAS.cpp main.cpp -L/usr/local/lib -fopenmp -o artMerg


echo "---> CPLEX_Studio126"

cplex_path="/opt/ibm/ILOG/CPLEX_Studio126/"

# CPLEX

export PATH=$PATH:$cplex_path$"cplex/bin/x86-64_linux"

export LIBRARY_PATH=$LIBRARY_PATH:$cplex_path$"cplex/lib/x86-64_linux/static_pic"

export C_INCLUDE_PATH=$C_INCLUDE_PATH:$cplex_path$"cplex/include"
export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:$cplex_path$"cplex/include"

# CONCERT (CPP)

export LIBRARY_PATH=$LIBRARY_PATH:$cplex_path$"concert/lib/x86-64_linux/static_pic"

export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:$cplex_path$"concert/include"

# op-L

export PATH=$PATH:$cplex_path$"opl/bin/x86-64_linux"

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$cplex_path$"op-l/bin/x86-64_linux"


g++ -O2 -g -fmax-errors=10 -L/usr/local/lib -fopenmp -o artMerg.out GRASP.cpp GRASP.h main.cpp -DIL_STD -lilocplex -lconcert -lcplex -lm -lpthread -std=c++11 -fPIC

# For testing purposes, the alpha parameter is being set to 0 since it is not being parameterized. The alpha value used during the runs is being randomly generated per iteration, with values ranging from 0.1 to 0.5.
#               Instance  - Alpha  - Iterations without improvement
./artMerg.out "1_6-1_N_a_conv2" "0.0" "100"
./artMerg.out "1_6-1_N_b_conv2" "0.0" "100"
./artMerg.out "1_6-1_N_c_conv2" "0.0" "100"
./artMerg.out "1_6-1_N_d_conv2" "0.0" "100"
./artMerg.out "1_6-1_N_e_conv2" "0.0" "100"
./artMerg.out "1_6-2_N_a_conv2" "0.0" "100"
./artMerg.out "1_6-2_N_b_conv2" "0.0" "100"
./artMerg.out "1_6-2_N_c_conv2" "0.0" "100"
./artMerg.out "1_6-2_N_d_conv2" "0.0" "100"
./artMerg.out "1_6-2_N_e_conv2" "0.0" "100"
./artMerg.out "1_6-3_N_a_conv2" "0.0" "100"
./artMerg.out "1_6-3_N_b_conv2" "0.0" "100"
./artMerg.out "1_6-3_N_c_conv2" "0.0" "100"
./artMerg.out "1_6-3_N_d_conv2" "0.0" "100"
./artMerg.out "1_6-3_N_e_conv2" "0.0" "100"
./artMerg.out "1_6-4_N_a_conv2" "0.0" "100"
./artMerg.out "1_6-4_N_b_conv2" "0.0" "100"
./artMerg.out "1_6-4_N_c_conv2" "0.0" "100"
./artMerg.out "1_6-4_N_d_conv2" "0.0" "100"
./artMerg.out "1_6-4_N_e_conv2" "0.0" "100"
./artMerg.out "1_6-5_N_a_conv2" "0.0" "100"
./artMerg.out "1_6-5_N_b_conv2" "0.0" "100"
./artMerg.out "1_6-5_N_c_conv2" "0.0" "100"
./artMerg.out "1_6-5_N_d_conv2" "0.0" "100"
./artMerg.out "1_6-5_N_e_conv2" "0.0" "100"
./artMerg.out "1_8-1_N_a_conv2" "0.0" "100"
./artMerg.out "1_8-1_N_b_conv2" "0.0" "100"
./artMerg.out "1_8-1_N_c_conv2" "0.0" "100"
./artMerg.out "1_8-1_N_d_conv2" "0.0" "100"
./artMerg.out "1_8-1_N_e_conv2" "0.0" "100"
./artMerg.out "1_8-2_N_a_conv2" "0.0" "100"
./artMerg.out "1_8-2_N_b_conv2" "0.0" "100"
./artMerg.out "1_8-2_N_c_conv2" "0.0" "100"
./artMerg.out "1_8-2_N_d_conv2" "0.0" "100"
./artMerg.out "1_8-2_N_e_conv2" "0.0" "100"
./artMerg.out "1_8-3_N_a_conv2" "0.0" "100"
./artMerg.out "1_8-3_N_b_conv2" "0.0" "100"
./artMerg.out "1_8-3_N_c_conv2" "0.0" "100"
./artMerg.out "1_8-3_N_d_conv2" "0.0" "100"
./artMerg.out "1_8-3_N_e_conv2" "0.0" "100"
./artMerg.out "1_8-4_N_a_conv2" "0.0" "100"
./artMerg.out "1_8-4_N_b_conv2" "0.0" "100"
./artMerg.out "1_8-4_N_c_conv2" "0.0" "100"
./artMerg.out "1_8-4_N_d_conv2" "0.0" "100"
./artMerg.out "1_8-4_N_e_conv2" "0.0" "100"
./artMerg.out "1_8-5_N_a_conv2" "0.0" "100"
./artMerg.out "1_8-5_N_b_conv2" "0.0" "100"
./artMerg.out "1_8-5_N_c_conv2" "0.0" "100"
./artMerg.out "1_8-5_N_d_conv2" "0.0" "100"
./artMerg.out "1_8-5_N_e_conv2" "0.0" "100"
./artMerg.out "2_6-1_N_a_conv2" "0.0" "100"
./artMerg.out "2_6-1_N_b_conv2" "0.0" "100"
./artMerg.out "2_6-1_N_c_conv2" "0.0" "100"
./artMerg.out "2_6-1_N_d_conv2" "0.0" "100"
./artMerg.out "2_6-1_N_e_conv2" "0.0" "100"
./artMerg.out "2_6-2_N_a_conv2" "0.0" "100"
./artMerg.out "2_6-2_N_b_conv2" "0.0" "100"
./artMerg.out "2_6-2_N_c_conv2" "0.0" "100"
./artMerg.out "2_6-2_N_d_conv2" "0.0" "100"
./artMerg.out "2_6-2_N_e_conv2" "0.0" "100"
./artMerg.out "2_6-3_N_a_conv2" "0.0" "100"
./artMerg.out "2_6-3_N_b_conv2" "0.0" "100"
./artMerg.out "2_6-3_N_c_conv2" "0.0" "100"
./artMerg.out "2_6-3_N_d_conv2" "0.0" "100"
./artMerg.out "2_6-3_N_e_conv2" "0.0" "100"
./artMerg.out "2_6-4_N_a_conv2" "0.0" "100"
./artMerg.out "2_6-4_N_b_conv2" "0.0" "100"
./artMerg.out "2_6-4_N_c_conv2" "0.0" "100"
./artMerg.out "2_6-4_N_d_conv2" "0.0" "100"
./artMerg.out "2_6-4_N_e_conv2" "0.0" "100"
./artMerg.out "2_6-5_N_a_conv2" "0.0" "100"
./artMerg.out "2_6-5_N_b_conv2" "0.0" "100"
./artMerg.out "2_6-5_N_c_conv2" "0.0" "100"
./artMerg.out "2_6-5_N_d_conv2" "0.0" "100"
./artMerg.out "2_6-5_N_e_conv2" "0.0" "100"
./artMerg.out "2_8-1_N_a_conv2" "0.0" "100"
./artMerg.out "2_8-1_N_b_conv2" "0.0" "100"
./artMerg.out "2_8-1_N_c_conv2" "0.0" "100"
./artMerg.out "2_8-1_N_d_conv2" "0.0" "100"
./artMerg.out "2_8-1_N_e_conv2" "0.0" "100"
./artMerg.out "2_8-2_N_a_conv2" "0.0" "100"
./artMerg.out "2_8-2_N_b_conv2" "0.0" "100"
./artMerg.out "2_8-2_N_c_conv2" "0.0" "100"
./artMerg.out "2_8-2_N_d_conv2" "0.0" "100"
./artMerg.out "2_8-2_N_e_conv2" "0.0" "100"
./artMerg.out "2_8-3_N_a_conv2" "0.0" "100"
./artMerg.out "2_8-3_N_b_conv2" "0.0" "100"
./artMerg.out "2_8-3_N_c_conv2" "0.0" "100"
./artMerg.out "2_8-3_N_d_conv2" "0.0" "100"
./artMerg.out "2_8-3_N_e_conv2" "0.0" "100"
./artMerg.out "2_8-4_N_a_conv2" "0.0" "100"
./artMerg.out "2_8-4_N_b_conv2" "0.0" "100"
./artMerg.out "2_8-4_N_c_conv2" "0.0" "100"
./artMerg.out "2_8-4_N_d_conv2" "0.0" "100"
./artMerg.out "2_8-4_N_e_conv2" "0.0" "100"
./artMerg.out "2_8-5_N_a_conv2" "0.0" "100"
./artMerg.out "2_8-5_N_b_conv2" "0.0" "100"
./artMerg.out "2_8-5_N_c_conv2" "0.0" "100"
./artMerg.out "2_8-5_N_d_conv2" "0.0" "100"
./artMerg.out "2_8-5_N_e_conv2" "0.0" "100"
