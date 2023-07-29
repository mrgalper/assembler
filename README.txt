
if you want to see the output of the first pass use the following env
ASSEMBLY_FIRST_PASS_IR=1 ./bin ....


if you want to see the output of the second pass use the following env
ASSEMBLY_SECOND_PASS_IR=1 ./bin ....

to build the project use the following:
cd build
cmake ../
make 

The binary is name assembly .
so you can do the ollowing 
./assembly file1 file2 ...
