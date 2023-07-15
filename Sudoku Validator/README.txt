INSTRUCTIONS

First in order to execute the program make sure that the compiler is able to compile c programs.
The user must also make an input file which specifies the dimensional value of N x N sudoku  (suppose N>0) and the number of threads (suppose K>0).
They must be written as follows:
K N
From the second line the elements of the sudoku must be present in the matrix[N][N] form.

for example- if the user wants to check a 4 x 4 sudoku with 4 threads (K = 4 and N = 4), he must write the input file accordingly (here a random sudoku is taken for example)
4 4
1 3 4 2
2 4 1 3
4 2 3 1
3 1 2 4

Now for executing the program, the user must run the following commands on the terminal of code editor or the main terminal(in case of linux(make sure you are in the directory where the source code file is present)).
Case 1: In order to execute source code using Pthread :
gcc Assgn2Srcpthread<ES21BTECH11007>.c -lm
./a.out <name of the inputfile.txt>

Case 2: In order to execute source code using OpenMP :
gcc -fopenmp Assgn2SrcOpenMp<ES21BTECH11007>.c  -lm
export OMP_NUM_THREADS=<the number of threads you want to use>     // as by default OpenMP uses number of threads = number of cores present on your CPU but you want to change the number of threads to
								   //   be used
./a.out <name of the inputfile.txt>	

and the code will be excecuted with the creation of output.txt in which all the details of the execution of each thread will be present and at last it will also mention whether the soduko is valid or not and the time taken to execute.
							      
