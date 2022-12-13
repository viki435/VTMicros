# VTMicros
Implementation of VTMicros


1.	Add the padding by yourself not the compiler generated padding - DONE
2.	Need one function for each functionality – DONE – Memory, CPU…
3.	Remove NO_OF_THREADS inside the thread handler function - DONE
4.	Each functionality needs to take multiple arguments – How long does each function need to execute (based on percentage) – Not Correct. So did not implement.
5.	Check underlying system details like VMs and how many threads can be created.
6.	CPU affinity needs to be one of the arguments. This implementation needs to be done little later.
7.	BIOS settings may also need to go as parameter – This implementation needs to be done little later.
8.	SNC, VM pinning 
9.	No. of iterations is the measure. Don't take percentage as argument
10.	Consider IO
11.	sleepDuration - change to number of iterations for CPU functionality
12.	Make Number of Threads as argument – Not done. Will be created based on number of CPUs.
13.	Function for implementing frequent sleep and a small add (eg) DONE
14.	Create a config file for taking all the arguments DONE

Comments on 6th December 2022:
 DONE:
1.	Check number of CPUs and create Number of threads based on that - DONE
2.	Pass the arguments through command line as well as through file - DONE
3.	Have multiple structures based on functionalities instead of single stThreadArguments structure - DONE
4.	Instead of memoryFunc, cpuFunc, frequentSleep functions have them as MICRO_1, MICRO_2, MICRO_3 and so on - DONE
5.	Frequent Sleep function - Iteration Count like 100, 1000, 10000 needs to be as an argument. Instead of add the numbers, the number of increments needs to be implemented for computation functionality. How many increments to be done before calling sleep function. Capture time difference for each iteration execution and store the time data in a buffer. Number of sleeps also needs to be an argument - DONE
6.	Memory implementation is fine as of now just name the function as a MICRO - DONE
 
IN PROGRESS: 
Need to take the arguments as ./EXE -f 7 -i 1000000000 -d 100000
where -f stands for function flag, -i stands for number of iterations, -d for Sleep Duration and so on for other arguments
