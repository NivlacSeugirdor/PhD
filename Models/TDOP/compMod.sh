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
#rm *outputs.txt
#rm *.ods
#rm *.lp
#rm salvaop-*

g++ -O2 -g -fmax-errors=10 -o OUTPUT.out FBTBP.cpp -DIL_STD -lilocplex -lconcert -lcplex -lm -lpthread -std=c++11 -fPIC

#clear

echo " --> Running the model!"
echo " --> It's working!"
echo " --> Thanks God!"
echo " --> Finally!"

sh subCompModN.sh &
sh subCompModN2.sh &

wait

#mv Table_Outputs.txt Table_Outputs_1_10.txt
#mv Table_Outputs.txt Table_Outputs_2_10.txt
#mv Table_Outputs.txt Table_Outputs_3_10.txt
#mv Table_Outputs.txt Table_Outputs_4_10.txt
mv Table_Outputs.txt Table_Outputs_5_10.txt

sh subCompModN3.sh &
sh subCompModN4.sh &

wait

#mv Table_Outputs.txt Table_Outputs_1_20.txt
#mv Table_Outputs.txt Table_Outputs_2_20.txt
#mv Table_Outputs.txt Table_Outputs_3_20.txt
#mv Table_Outputs.txt Table_Outputs_4_20.txt
mv Table_Outputs.txt Table_Outputs_5_20.txt


rm *_6-*.txt
rm *_8-*.txt

#mv *_model_.lp ./Models
#mv *_outputs.txt ./OutputsExev
#mv salvaop*.txt ./OutputsSave
