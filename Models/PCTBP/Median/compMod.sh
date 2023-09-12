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

echo " --> Compiling"
rm *.out
rm *outputs.txt
rm *.ods
rm *.lp
rm table_results.txt
#rm salvaop-*

g++ -O2 -g -fmax-errors=10 -o OUTPUT.out FBTBP.cpp -DIL_STD -lilocplex -lconcert -lcplex -lm -lpthread -std=c++11 -fPIC

#clear

echo " --> Running the model!"
echo " --> It's working!"
echo " --> Thanks God!"
echo " --> Finally!"



sh subCompModU.sh &
sh subCompModN.sh &

wait

#rm *_6-*.txt
#
sh subCompModU2.sh &
sh subCompModN2.sh &

wait

#rm *_8-*.txt


