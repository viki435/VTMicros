#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdint.h>
#include <math.h>
// #define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include "vtmicros.h"

unsigned int g_index = 0;

/*
 * vtmicros.c
 * Description: 
 * This file does the following:
 *  1.    Reads the arguments from file by default.
 *  2.    If the arguments are given in the command line, then ignores the arguments read from file and update the data from command line.
 *  3.    Creates number of threads based on the number of CPUs.
 *  4.    Create three micros (MICRO_1, MICRO_2 and MICRO_3) with corresponding functionality implemented as follows:
 *        a.    MICRO_1 for Memory Data – Filling the destination buffer with the source buffer
 *        b.    MICRO_2 for CPU Data – Calls random function based for number of iterations to keep the CPU busy
 *        c.    MICRO_3 for Frequent Sleep and Wake functionality – Sleeps for a certain duration, calls an add function for iteration count times and marks start and end time for each of the iteration

 * Created on: <14/12/22>
 *
 */
 
/* Function Implementations */

/*********************************************************************************************************************************
Name:        FillPerfData
Arguments:   
Returns:     NONE
Purpose:     Fills the data from cache line structure to performance data structure for each iteration
Call Tree:   Called from MICRO_2
*********************************************************************************************************************************/
void FillPerfData (time_t startT, time_t endT, long double itrns)
{
    static unsigned int index=0;
    perfData[index].startTime = startT;
    perfData[index].endTime = endT;
    perfData[index].iterations = itrns;
    // printf ("Difference in times after copying to perfData at index %d is: %ld\n", index, (perfData[index].endTime - perfData[index].startTime));
    // printf ("Number of iterations are: %Lf\n", perfData[index].iterations);
    index++;
}


/*********************************************************************************************************************************
Name:        MICRO_1
Arguments:   Thread Arguments Structure (stThreadArguments)
Returns:     NONE
Purpose:     Based on the memory size copies the data to the destination buffer using memcpy.
Call Tree:   Called from threadHandlerFunc
*********************************************************************************************************************************/
void MICRO_1 (struct stMemData memData)
{
    unsigned int i;
	struct timespec ts;
	// unsigned int b;
    memOnceFlag = memOnceFlag+1;
    printf ("***MEMORY OPERATION START***\n");
	// scanf ("%d", &b);
    struct stMemData *memDataPtr;
    memDataPtr = (struct stMemData *) malloc ((memData.size)*sizeof(struct stMemData));
	// memTime.startTime = time(NULL);
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	memTime.startTime = SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
    for (i=0; i<memData.size; i++)
    {
        (memDataPtr+i)->src=i+100;
    }
    // printf ("Source Data is: \n");
    for (i=0; i<memData.size; i++)
    {
        // printf ("%d\t", ((memDataPtr+i)->src));
    }
    printf ("\n");
    for (i=0; i<memData.size; i++)
    {
        // (memDataPtr+i)->dest = srcArray[i];// (memDataPtr+i)->src;
		(memDataPtr+i)->dest = (memDataPtr+i)->src;
    }
    // printf ("Destination Data is: \n");
    for (i=0; i<memData.size; i++)
    {
        // printf ("%d\t", ((memDataPtr+i)->dest));
    }
	// memTime.endTime = time(NULL);
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	memTime.endTime = SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
    printf ("\nStart time, End time & difference in timing are: %ld\t%ld\t%ld\n", memTime.startTime, memTime.endTime, (memTime.endTime - memTime.startTime));
    printf ("\n***MEMORY OPERATION END***\n");
}

/*********************************************************************************************************************************
Name:        MICRO_2
Arguments:   Thread Arguments Structure (stThreadArguments)
Returns:     NONE
Purpose:     CPU Computation function. Keeps calling the rand() function for number of iterations times to keep the CPU busy
Call Tree:   Called from threadHandlerFunc
*********************************************************************************************************************************/
void MICRO_2 (struct stCpuData cpuData)
{
    unsigned int l = 0;// , m = 0;
	time_t startTime, endTime;
	long double iterations;
    cpuOnceFlag = cpuOnceFlag+1;
    printf ("***CPU OPERATION START***\n");
    // Marking the start time
    startTime = time(NULL);
    for (l=0; l<cpuData.noOfIterations; l++)
    {
		iterations = l;
        int m = rand();
    }
    // Marking the end time
    endTime = time(NULL);
	printf ("**********************************************\nFilling Perf Data\n**********************************************\n");
    FillPerfData (startTime, endTime, iterations);
    printf ("***CPU OPERATION END***\n");
}

/*********************************************************************************************************************************
Name:        MICRO_3
Arguments:   Thread Arguments Structure (stThreadArguments)
Returns:     NONE
Purpose:     This function sleeps for the amount sleep duration parameter from 
             stThreadArguments structure and calls the add function.
             This action is repeated based on the NO_OF_SLEEPS macro.
Call Tree:   Called from threadHandlerFunc
*********************************************************************************************************************************/
void MICRO_3 (struct stFreqSleep freqSleepData, int coreID, pthread_t pid)
{
    unsigned int i=0;
	unsigned int ind=0;
    struct timespec ts;
    uint64_t start_us;
    uint64_t end_us;
    uint64_t elapsed;
	long double iterations;
    
    static unsigned int itrTemp = 10;

    freqSleepOnceFlag = freqSleepOnceFlag+1;
    printf ("\n***FREQUENT SLEEP AND WAKE OPERATION START***\t on CPU: %d\n", sched_getcpu());
	// printf ("CPU ID is: %d\n", sched_getcpu());
	iterations = freqSleepData.iterationCount;
    for (i=0; i<freqSleepData.noOfSleeps; i++)
    {
        // iterations = iterations * itrTemp;
		
		// printf ("iterations at start is              ----------------------------- %Lf\n", iterations);
		clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
		start_us = SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
		for (ind=0; ind<iterations; ind++)
		{
			rand ();
		}
		clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
		end_us = SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
		
		elapsed = end_us - start_us;

		// printf ("Elapsed time in uS is: %ld\n", elapsed);
		// itrnData[i].itrTime = elapsed; // TODO - Every thread is using the same itrnData global variable.
									   // Pass core_id as an argument to this MICRO. Use the cacheLine structure for filling the data.
		// itrnData[i].freqSleepIterations = iterations;
		
		cacheLine[coreID][i].elapsedTime = elapsed;
		cacheLine[coreID][i].iterations = iterations;
		// printf ("MICRO_3 - core_id, elapsed time and iterations are: %d\t%ld\t%Lf\n", coreID, cacheLine[coreID][i].elapsedTime, cacheLine[coreID][i].iterations);
		usleep(freqSleepData.sleepDuration);
    }
    printf ("\n***FREQUENT SLEEP AND WAKE OPERATION END***\n");
}


void PrintItrnData ()
{
    unsigned int index=0;
	FILE *fptr = fopen("output.txt", "w");
	if (fptr == NULL)
	{
		printf("Could not open file");
	}
	
	printf ("IN PRINTITRNDATA\n");
    for (index=0; index <threadArgs.noOfThreads; index++)
    {
		for (int j=0; j<freqSleepData.noOfSleeps; j++)
		{
			// printf ("Time difference for the Iteration count %Lf is %ld microseconds with CPU ID as %d\n", cacheLine[index][j].iterations, cacheLine[index][j].elapsedTime, index);
			printf ("PInD - core_id, elapsed time and iterations are: %d\t%ld\t%Lf\n", index, cacheLine[index][j].elapsedTime, cacheLine[index][j].iterations);
			fprintf (fptr, "%d\t%ld\t%Lf\n", index, cacheLine[index][j].elapsedTime, cacheLine[index][j].iterations);
		}
    }
	fclose(fptr);
}

/*********************************************************************************************************************************
Name:        threadHandlerFunc
Arguments:   args
Returns:     NONE
Purpose:     This is the thread handler function created once the pthread_create is called.
             This threadHandlerFunc is one the argument in pthread_create.
             The args are taken from the file and passed to threadHandlerFunc based on which it calls certain functionality
             (for eg. Memory Functionality, CPU Funcitonality, Frequent Sleep etc)
Call Tree:   Called from pthread_create which is in the ThreadMgmtFunction
*********************************************************************************************************************************/
void *threadHandlerFunc (void* args)
{
    unsigned int i = 0, j = 0;
	struct thread_info *thread_info = args;
	const pthread_t pid = pthread_self();
	const int core_id = thread_info->core_id;
	// printf ("pid is: %ld\n", pid);
	
	// printf ("threadID is: %ld\n", g_index);
	// printf ("thread_info->core_id is: %d\n", thread_info->core_id);
	
	// cpu_set_t: This data set is a bitset where each bit represents a CPU.
	cpu_set_t cpuset;
	// CPU_ZERO: This macro initializes the CPU set set to be the empty set.
	CPU_ZERO(&cpuset);
	// CPU_SET: This macro adds cpu to the CPU set set.
	CPU_SET(core_id, &cpuset);
	
	const int set_result = pthread_setaffinity_np(pid, sizeof(cpu_set_t), &cpuset);
	if (set_result != 0)
	{
		printf ("SETTING AFFINITY ERROR\n");
	}
	/*else
	{
		printf ("SETTING AFFINITY SUCCESSFUL\n");
	}*/
		
	const int get_affinity = pthread_getaffinity_np(pid, sizeof(cpu_set_t), &cpuset);
	if (get_affinity != 0)
	{
		printf ("GETTING AFFINITY ERROR\n");
	}
	/*else
	{
		printf ("GETTING AFFINITY SUCCESSFUL\n");
	}*/

    if ((threadArgs.MemoryFlag == TRUE))// && (memOnceFlag == 0))
    {
        MICRO_1 (memData);
    }
    
    if ((threadArgs.CPUFlag == TRUE))// && (cpuOnceFlag == 0))
    {
        MICRO_2 (cpuData);
    }

    if ((threadArgs.freqSleepFlag == TRUE))// && (freqSleepOnceFlag == 0))
    {
        MICRO_3 (freqSleepData, core_id, pid);
    }
	
    pthread_exit(NULL);
}

void ReadCmdLineArgs (int argc, char **argv)
{
	unsigned int i;
	unsigned int POS=0;
	// Cases: M, s, i, d, I, n
	// Eg: ./vtmicros -M4 -s5001 -i10000000 -d1000000 -I10000000 -n10
	for (i = 1; (i < argc && argv[i][0] == '-'); i++)
	{
		switch (argv[i][1])
		{
			// The following argument is mandatory to read from command line.
			case 'M':
				printf ("\n********************************************************\n");
				printf ("READING COMMAND LINE ARGUMENTS\n");
				printf ("********************************************************\n");
				threadArgs.microFlag = atoi(&argv[i][2]);
				printf ("MICRO flag is: %d\n", threadArgs.microFlag);
				threadArgs.MemoryFlag = 0;
				threadArgs.CPUFlag = 0;
				threadArgs.freqSleepFlag = 0;
				POS=0;
				if (1 & (threadArgs.microFlag >> POS))
				{
					threadArgs.MemoryFlag = 1;
				}
				POS++;
				if (1 & (threadArgs.microFlag >> POS))
				{
					threadArgs.CPUFlag = 1;
				}
				POS++;
				if (1 & (threadArgs.microFlag >> POS))
				{
					threadArgs.freqSleepFlag = 1;
				}
				printf ("Flag values are: %d\t%d\t%d\n", threadArgs.MemoryFlag, threadArgs.CPUFlag, threadArgs.freqSleepFlag);
			break;
			case 's':
				memData.size = atoi(&argv[i][2]);
				printf ("Size is: %d\n", memData.size);
			break;
			case 'i':
				cpuData.noOfIterations = atoi(&argv[i][2]);
				printf ("Number of iterations is: %d\n", cpuData.noOfIterations);
			break;
			case 'd':
				freqSleepData.sleepDuration = atoi(&argv[i][2]);
				printf ("Sleep Duration is: %d\n", freqSleepData.sleepDuration);
			break;
			case 'I':
				freqSleepData.iterationCount = atoi(&argv[i][2]);
				printf ("Iteration count is: %d\n", freqSleepData.iterationCount);
			break;
			case 'n':
				freqSleepData.noOfSleeps = atoi(&argv[i][2]);
				printf ("Number of sleeps is: %d\n", freqSleepData.noOfSleeps);
			break;
			case 't':
				printf ("\n********************************************************\n");
				printf ("READING ARGUMENTS FROM FILE\n");
				printf ("********************************************************\n");
				ReadArgsFromFile ();
			break;
			case '?':
					/*printf ("\n***PROVIDE ONLY REQUIRED ARGUMENTS FOR THE MICRO WHICH YOU WANT TO EXECUTE***\n");
					printf ("\n***********************************************************************\n");
					printf ("TO READ INPUT ARGUMENTS THROUGH COMMAND LINE\n");
					printf ("***********************************************************************\n");
					printf ("Usage: ./vtmicros.exe -M <F> <s> <i> <d> <I> <n>\nwhere,\n");
					printf ("F-indicates which micro or combination of micros to be executed\n");
					printf ("s-size of memory to be copied to destination buffer as part of memory micro\n");
					printf ("i-number of times the random function is called in CPU micro to keep CPU busy\n");
					printf ("d-the sleep duration in Frequent sleep & wake micro\n");
					printf ("I-Iteration count to keep CPU busy by calling random function in Frequent sleep & wake micro\n");
					printf ("n-number of sleeps times the CPU needs to be kept busy and idle alternately in Frequent sleep & wake micro\n");
					printf ("\n***********************************************************************\nTO READ INPUT ARGUMENTS FROM FILE\n***********************************************************************\n");
					printf ("Usage: ./vtmicros.exe -t or ./vtmicros.exe\n\n");*/
					
					printf("\tvtmicros [-Mn] [-sn] [-in] [-dn] [-In] [-nn]\n");
					printf("where\n");
					printf("\t-M<n> : Micro flag to determine which micro to be executed - defaults to 0\n");
					printf("\t-s<n> : Size for Memory Micro - defaults to 5001\n");
					printf("\t-i<n> : Number of iterations for CPU Micro- defaults to 1000000000\n");
					printf("\t-d<n> : Sleep duration for Frequent sleep & wake Micro - defaults to 100000\n");
					printf("\t-I<n> : Iteration count for Frequent sleep & wake Micro - defaults to 10000000\n");
					printf("\t-n<n> : Number of sleeps for Frequent sleep & wake Micro - defaults to 8\n");
					exit(EXIT_SUCCESS);
				break;
			default:
				printf ("INVALID COMMAND LINE ARGUMENT\n");
				exit (EXIT_FAILURE);
			break;
		}
	}
	threadArgs.noOfThreads = get_nprocs();
    printf ("Thread parameters filled are: \n\n");
    printf ("MemoryFlag is: %d\n", threadArgs.MemoryFlag);
    printf ("CPUFlag is: %d\n", threadArgs.CPUFlag);
    printf ("freqSleepFlag is: %d\n", threadArgs.freqSleepFlag);
    printf ("noOfThreads is: %d\n", threadArgs.noOfThreads);
    printf ("noOfIterations is: %Lf\n", cpuData.noOfIterations);
    printf ("sleepDuration is: %d\n", freqSleepData.sleepDuration);
    printf ("size is: %d\n", memData.size);
    printf ("Iteration Count is: %Lf\n", freqSleepData.iterationCount);
    printf ("Number of sleeps is: %d\n", freqSleepData.noOfSleeps);
}

/*void ReadCmdLineArgs (int argc, char **argv)
{
    unsigned int POS=0;
    unsigned int b;
    int opt;
	unsigned int i;
	// ./exe -M 7 501 1000000000 100000 10 7
	printf ("Value of argc is: %d\n", argc);
	for (i=1; i<argc; i++)
    {
		while ((opt = getopt(argc, argv, "Mt")) != -1)
		{
			switch (opt)
			{
				case 'M':
					printf ("\n\n***READING COMMAND LINE ARGUMENTS***\n\n");
					printf ("Option-M\n");
					threadArgs.microFlag = atoi(argv[2]);
					printf ("Function flag is: %d\n", threadArgs.microFlag);

					switch (threadArgs.microFlag)
					{
						case 0:
							printf ("INVALID FUNCTION FLAG - NO MICRO TO EXECUTE\n");
							exit (EXIT_FAILURE);
						break;
						case 1:
							if (argc != 4)
							{
								printf ("INVALID SET OF ARGUMENTS\n");
								exit (EXIT_FAILURE);
							}
							printf ("MEMORY MICRO TO BE EXECUTED\n");
							threadArgs.MemoryFlag = 1;
							memData.size = atoi(argv[3]);
						break;
						case 2:
							if (argc != 4)
							{
								printf ("INVALID SET OF ARGUMENTS\n");
								exit (EXIT_FAILURE);
							}
							printf ("CPU MICRO TO BE EXECUTED\n");
							threadArgs.CPUFlag = 1;
							cpuData.noOfIterations = atoi(argv[3]);
						break;
						case 3:
							if (argc != 5)
							{
								printf ("INVALID SET OF ARGUMENTS\n");
								exit (EXIT_FAILURE);
							}
							printf ("MEMORY & CPU MICROS TO BE EXECUTED\n");
							threadArgs.MemoryFlag = 1;
							threadArgs.CPUFlag = 1;
							memData.size = atoi(argv[3]);
							cpuData.noOfIterations = atoi(argv[4]);
						break;
						case 4:
							if (argc != 6)
							{
								printf ("INVALID SET OF ARGUMENTS\n");
								exit (EXIT_FAILURE);
							}
							printf ("FREQUENT SLEEP & WAKE MICRO TO BE EXECUTED\n");
							threadArgs.freqSleepFlag = 1;
							freqSleepData.sleepDuration = atoi(argv[3]);
							freqSleepData.iterationCount = atoi(argv[4]);
							freqSleepData.noOfSleeps = atoi(argv[5]);
						break;
						case 5:
							if (argc != 7)
							{
								printf ("INVALID SET OF ARGUMENTS\n");
								exit (EXIT_FAILURE);
							}
							printf ("MEMORY, FREQUENT SLEEP & WAKE MICROS TO BE EXECUTED\n");
							threadArgs.MemoryFlag = 1;
							threadArgs.freqSleepFlag = 1;
							memData.size = atoi(argv[3]);
							freqSleepData.sleepDuration = atoi(argv[4]);
							freqSleepData.iterationCount = atoi(argv[5]);
							freqSleepData.noOfSleeps = atoi(argv[6]);
						break;
						case 6:
							if (argc != 7)
							{
								printf ("INVALID SET OF ARGUMENTS\n");
								exit (EXIT_FAILURE);
							}
							printf ("CPU, FREQUENT SLEEP & WAKE MICROS TO BE EXECUTED\n");
							threadArgs.CPUFlag = 1;
							threadArgs.freqSleepFlag = 1;
							cpuData.noOfIterations = atoi(argv[3]);
							freqSleepData.sleepDuration = atoi(argv[4]);
							freqSleepData.iterationCount = atoi(argv[5]);
							freqSleepData.noOfSleeps = atoi(argv[6]);
						break;
						case 7:
							if (argc != 8)
							{
								printf ("INVALID SET OF ARGUMENTS\n");
								exit (EXIT_FAILURE);
							}
							printf ("MEMORY, CPU, FREQUENT SLEEP & WAKE MICROS TO BE EXECUTED\n");
							threadArgs.MemoryFlag = 1;
							threadArgs.CPUFlag = 1;
							threadArgs.freqSleepFlag = 1;
							memData.size = atoi(argv[3]);
							cpuData.noOfIterations = atoi(argv[4]);
							freqSleepData.sleepDuration = atoi(argv[5]);
							freqSleepData.iterationCount = atoi(argv[6]);
							freqSleepData.noOfSleeps = atoi(argv[7]);
						break;
						default:
							printf ("INVALID MICRO ARGUMENT\n");
							exit (EXIT_FAILURE);
						break;
					}
					break;
				case 't':
					printf ("Option-t: Read from file\n");
					ReadArgsFromFile ();
				break;
				case '?':
					printf ("\n***PROVIDE ONLY REQUIRED ARGUMENTS FOR THE MICRO WHICH YOU WANT TO EXECUTE***\n");
					printf ("\n***********************************************************************\n");
					printf ("TO READ INPUT ARGUMENTS THROUGH COMMAND LINE\n");
					printf ("***********************************************************************\n");
					printf ("Usage: ./vtmicros.exe -M <F> <s> <i> <d> <I> <n>\nwhere,\n");
					printf ("F-indicates which micro or combination of micros to be executed\n");
					printf ("s-size of memory to be copied to destination buffer as part of memory micro\n");
					printf ("i-number of times the random function is called in CPU micro to keep CPU busy\n");
					printf ("d-the sleep duration in Frequent sleep & wake micro\n");
					printf ("I-Iteration count to keep CPU busy by calling random function in Frequent sleep & wake micro\n");
					printf ("n-number of sleeps times the CPU needs to be kept busy and idle alternately in Frequent sleep & wake micro\n");
					printf ("\n***********************************************************************\nTO READ INPUT ARGUMENTS FROM FILE\n***********************************************************************\n");
					printf ("Usage: ./vtmicros.exe -t or ./vtmicros.exe\n\n");
					// fprintf(stderr, "Usage: %s -f <F> <s> <i> <d> <I> <n>\t or %s -t\n", argv[0], argv[0]);
					// printf("where\nF indicates which functionality to execute\n");
					// printf("s is size of memory to be copied to destination buffer as part of memory functionality\n");
					// printf("i is the times the rand () is called in CPU functionality\n");
					// printf("d is the sleep time in Frequent sleep and wake functionality\n");
					// printf("I is the times the rand () is called in Frequent sleep and wake functionality\n");
					// printf("n is for calling the rand () based on iteration count and sleep based on sleep duration in Frequent sleep and wake functionality\n");
					
					// printf ("\n\n\n%s -M is for reading the arguments from command line", argv[0]);
					// printf ("\n\n\n%s -t    or     %s are for reading the arguments from file\n", argv[0], argv[0]);
					exit(EXIT_SUCCESS);
				break;
				default:
					exit(EXIT_FAILURE);
			}
		}
    }

    threadArgs.noOfThreads = get_nprocs();
    printf ("Thread parameters filled are: \n\n");
    printf ("MemoryFlag is: %d\n", threadArgs.MemoryFlag);
    printf ("CPUFlag is: %d\n", threadArgs.CPUFlag);
    printf ("freqSleepFlag is: %d\n", threadArgs.freqSleepFlag);
    printf ("noOfThreads is: %d\n", threadArgs.noOfThreads);
    printf ("noOfIterations is: %Lf\n", cpuData.noOfIterations);
    printf ("sleepDuration is: %d\n", freqSleepData.sleepDuration);
    printf ("size is: %d\n", memData.size);
    printf ("Iteration Count is: %Lf\n", freqSleepData.iterationCount);
    printf ("Number of sleeps is: %d\n", freqSleepData.noOfSleeps);
    // scanf ("%d", &b);
}*/

/*********************************************************************************************************************************
Name:        ReadArgsFromFile
Arguments:   NONE
Returns:     NONE
Purpose:     Opens the inputFile.config file and reads the data. Once the data is read it is copied to the
             stThreadArguments structure for using across the file.
Call Tree:   Called  from main
*********************************************************************************************************************************/
void ReadArgsFromFile ()
{
    FILE *fptr;
    char line[MAX_CHAR] = {0};
    char str[MAX_LINES][MAX_CHAR];
    unsigned int line_count = 0;
    unsigned int POS=0;
    int curr_line = 0, i = 0;
    
	printf ("\n********************************************************\n");
	printf ("\nREADING ARGUMENTS FROM FILE***\n\n");
	printf ("********************************************************\n");
    

    fptr = fopen ("inputFile.config", "r");

    if (fptr == NULL)
    {
        printf ("FILE OPENING ERROR\n");
		exit(EXIT_FAILURE);
    }
    else
    {
        printf ("FILE OPENING SUCCESSFUL\n");
    }

    while((curr_line < MAX_LINES) && ((fgets(line, MAX_CHAR, fptr)) != NULL))
    {
        //str[i] = buffer;
        memcpy(str[i], line, strlen(line));
        curr_line++;
        ++i;
    }

    threadArgs.microFlag = atoi(str[0]);
    threadArgs.noOfThreads = get_nprocs();
    cpuData.noOfIterations = atoi(str[1]);
    freqSleepData.sleepDuration = atoi(str[2]);
    memData.size = atoi(str[3]);
    freqSleepData.iterationCount = atoi(str[4]);
    freqSleepData.noOfSleeps = atoi(str[5]);

    printf ("Function flag is: %d\n", threadArgs.microFlag);

    POS=0;
    if (1 & (threadArgs.microFlag >> POS))
    {
        threadArgs.MemoryFlag = 1;
    }
    POS++;
    if (1 & (threadArgs.microFlag >> POS))
    {
        threadArgs.CPUFlag = 1;
    }
    POS++;
    if (1 & (threadArgs.microFlag >> POS))
    {
        threadArgs.freqSleepFlag = 1;
    }


    printf ("Thread parameters filled are: \n");
    printf ("MemoryFlag is: %d\n", threadArgs.MemoryFlag);
    printf ("CPUFlag is: %d\n", threadArgs.CPUFlag);
    printf ("freqSleepFlag is: %d\n", threadArgs.freqSleepFlag);
    printf ("noOfThreads is: %d\n", threadArgs.noOfThreads);
    printf ("noOfIterations is: %Lf\n", cpuData.noOfIterations);
    printf ("sleepDuration is: %d\n", freqSleepData.sleepDuration);
    printf ("size is: %d\n", memData.size);
    printf ("Iteration Count is: %Lf\n", freqSleepData.iterationCount);
    printf ("Number of sleeps is: %d\n", freqSleepData.noOfSleeps);
}

/*********************************************************************************************************************************
Name:        ThreadMgmtFunction
Arguments:   NONE
Returns:     NONE
Purpose:     Creates threads based on the number of arguments parameter of stThreadArguments structure.
Call Tree:   Called  from main
*********************************************************************************************************************************/
void ThreadMgmtFunction ()
{
    unsigned int i=0, j=0;
    int retThreadCreate;
    // pthread_t threadID[threadArgs.noOfThreads];
	int core_id;
	struct thread_info *thread_info = calloc(threadArgs.noOfThreads, sizeof(struct thread_info));
	

    for (i=0; i<threadArgs.noOfThreads; i++)
    {
        /////threadArgs.core_id = i;
        /////retThreadCreate = pthread_create (&threadID[i], NULL, threadHandlerFunc, (void *) &threadArgs);

		thread_info[i].core_id = i;
        retThreadCreate = pthread_create (&thread_info[i].thread_id, NULL, threadHandlerFunc, (void *) &thread_info[i]);
		
		// g_index = i;
  
		if (retThreadCreate)
        {
			printf ("Thread %d creation error\n", i);
            exit (-1);
        }
        else
        {
			printf ("Thread ID is: %ld and Thread %d Creation Successful\n", thread_info[i].thread_id, i);
        }
        // pthread_join (thread_info[i].thread_id, NULL);
    }
	for (j=0; j<threadArgs.noOfThreads; j++)
	{
		pthread_join (thread_info[j].thread_id, NULL);
	}
	printf ("END OF THREADS EXECUTION\n");
}

/*********************************************************************************************************************************
Name:        ValidateArguments
Arguments:   Argument count provided by user
Returns:     NONE
Purpose:     Validate the input arguments taken from either file or command line
Call Tree:   Called from main function
*********************************************************************************************************************************/

void ValidateArguments (int argCount)
{
	printf ("VALIDATING ARGUMENTS, argument count is: %d\n", argCount);
	
	// Parameters from stThreadArgs structure
	if ((threadArgs.microFlag <= 0) || (threadArgs.microFlag > 7))
	{
		printf ("INVALID COMMAND LINE ARGUMENT: FUNCTION FLAG\n");
		exit(EXIT_FAILURE);
	}
	if ((threadArgs.MemoryFlag != 0) && (threadArgs.MemoryFlag != 1))
	{
		printf ("INVALID MEMORY FLAG\n");
		exit(EXIT_FAILURE);
	}
	if ((threadArgs.CPUFlag != 0) && (threadArgs.CPUFlag != 1))
	{
		printf ("INVALID CPU FLAG\n");
		exit(EXIT_FAILURE);
	}
	if ((threadArgs.freqSleepFlag != 0) && (threadArgs.freqSleepFlag != 1))
	{
		printf ("INVALID FREQUENT SLEEP AND WAKE FLAG\n");
		exit(EXIT_FAILURE);
	}
	if ((threadArgs.noOfThreads < 0) || (threadArgs.noOfThreads > (pow(2, (8*sizeof(unsigned int))))))
	{
		printf ("INVALID NUMBER OF THREADS\n");
		exit(EXIT_FAILURE);
	}
	
	// Parameters from stMemData structure
	if ((memData.size < 0) || (memData.size > (pow(2, (8*sizeof(unsigned int))))))
	{
		printf ("INVALID MEMORY SIZE\n");
		exit(EXIT_FAILURE);
	}
	
	// Parameters from stCpuData structure
	if ((cpuData.noOfIterations < 0) || (cpuData.noOfIterations > (pow(2, (8*sizeof(long double))))))
	{
		printf ("INVALID NUMBER OF ITERATIONS\n");
		exit(EXIT_FAILURE);
	}
	
	
	// Parameters from stFreqSleep structure
	if ((freqSleepData.sleepDuration < 0) || (freqSleepData.sleepDuration > (pow(2, (8*sizeof(unsigned int))))))
	{
		printf ("INVALID SLEEP DURATION\n");
		exit(EXIT_FAILURE);
	}
	
	if ((freqSleepData.noOfSleeps < 0) || (freqSleepData.noOfSleeps > (pow(2, (8*sizeof(unsigned int))))))
	{
		printf ("INVALID NUMBER OF SLEEPS\n");
		exit(EXIT_FAILURE);
	}
	
	if ((freqSleepData.iterationCount < 0) || (freqSleepData.iterationCount > (pow(2, (8*sizeof(long double))))))
	{
		printf ("INVALID ITERATION COUNT\n");
		exit(EXIT_FAILURE);
	}
}


void InitializeDefaults ()
{
	threadArgs.noOfThreads = 8;
	threadArgs.microFlag = 0;
	memData.size = 5001;
	cpuData.noOfIterations = 1000000000;
	freqSleepData.sleepDuration = 100000;
	freqSleepData.iterationCount = 10000000;
	freqSleepData.noOfSleeps = 8;
}

/*********************************************************************************************************************************
Name:        main
Arguments:   argc, argv
Returns:     0 on completion of program execution
Purpose:     main function, where the execution starts from. Calls a function to read input arguments from file.
             Calls thread management function to manage the creation of threads and execution of corresponding functionality.
Call Tree:   Starting point of execution
*********************************************************************************************************************************/
int main (int argc, char** argv)
{
	printf("Size of struct: %ld", sizeof(struct stCacheLine));
	InitializeDefaults ();
    if (argc > 1)
    {
        ReadCmdLineArgs (argc, argv);
    }
	
	if (argc == 1)
    {
		ReadArgsFromFile ();
    }
	
	/*printf ("Sizes are: \n");
	printf ("unsigned int: %d\n", sizeof(unsigned int));
	printf ("unsigned long int: %ld\n", sizeof(unsigned long int));
	printf ("long double: %ld\n", sizeof(long double));
	printf ("time_t: %ld\n", sizeof(time_t));

	printf ("unsigned int - %f\n", pow(2, (8*sizeof(unsigned int))));
	printf ("long double - %f\n", pow(2, (8*sizeof(long double))));
	printf ("time_t - %f\n", pow(2, (8*sizeof(time_t))));
	printf ("unsigend long int - %f\n", pow(2, (8*sizeof(unsigned long int))));*/
    
    ValidateArguments (argc);
    ThreadMgmtFunction ();
	PrintItrnData ();
    return 0;
}



/*


MEMORY:

1. Copy certain amount of memory from source memory to the destination memory
2. Memory bandwidth - rate in mbps at which the data can be read from or written to a memory both for physical memory and on cache lines
3. Cache to cache memory data transfer
4. Use the swap memory for storing the data and then see the performance. Note: Swap memory is the one which will be used after completely utilizing the normal RAM.

CPU:

1. Keep the CPU busy for the specified number of iterations and determine the time taken for each iteration
2. Setting the affinity i.e., assigning the particular thread to run on a specific core and checking the performance
3. Calculating the throughput (operations per sec) by keeping the CPU busy
4. 

FREQUENT SLEEP & WAKE:

1. Keep the CPU busy for a certain number of iterations and then idle for certain duration. Repeat this for certain number of times and marking the time for each iteration.

IO:

1. NVM over PCIe - accessing over a single VM


20 core machine - cache, memory bandwidth.
VM - Emulating the mapping.
Multi-VM case - Starting the workload across VMs - Not told by Yashmeet

14-02-2023:

Task wise list - Detailed description of the tasks. List in a very detailed way by this week Friday.
MLC - as of now


23-02-2023:

Inputs from Sajjid:

1. PrintItrnData should be called outside of MICRO_3 thread execution. As it is no need to print any data inside the thread which increases the thread usage.
2. Instead of filling itrnData structure in the MICRO_3, use cacheLine structure for containing this data. Every thread is using the same itrnData global variable. And it should be indexed with core_id as we need to know which core is having a particular data.

*/

// threadID or coreID should be the index for filling the iteration data in the cache line structure