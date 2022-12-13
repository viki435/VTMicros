#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdint.h>

// Macros
#define OUTER_LOOP_CNT 10
// #define MEM_SIZE 100
#define NO_OF_SLEEPS 100
#define MAX_LINES 100
#define MAX_CHAR 80

// Convert seconds to milliseconds
#define SEC_TO_MS(sec) ((sec)*1000)
// Convert seconds to microseconds
#define SEC_TO_US(sec) ((sec)*1000000)
// Convert seconds to nanoseconds
#define SEC_TO_NS(sec) ((sec)*1000000000)

// Convert nanoseconds to seconds
#define NS_TO_SEC(ns)   ((ns)/1000000000)
// Convert nanoseconds to milliseconds
#define NS_TO_MS(ns)    ((ns)/1000000)
// Convert nanoseconds to microseconds
#define NS_TO_US(ns)    ((ns)/1000)

// Enumeration to check the state based on which the execution of Memory and CPU Functionality depends on
enum state
{
	FALSE=0,
	TRUE=1
};

/* Structures */

// This is the structure that can be used to pass the required arguments to the threads. As of now, only the current taskID and sleep duration are added
struct stThreadArguments
{
	// unsigned int taskID[NO_OF_THREADS];
	// unsigned long int noOfIterations; - MOVING: to stCpuData structure
	enum state MemoryFlag;
	enum state CPUFlag;
	enum state freqSleepFlag;
	enum state NetworkFlag;
	enum state DiskFlag;
	unsigned int index;
	// unsigned int sleepDuration; - MOVING: to stFreqSleep structure
	unsigned int noOfThreads;
	
	/* For Memory Functionality */
	// unsigned int src[MEM_SIZE];
	// unsigned int dest[MEM_SIZE];
	// unsigned int size; - MOVING: to stMemData


	/* New Design */
	unsigned int functionFlag;
}threadArgs;

struct stFreqSleep
{
	unsigned int sleepDuration;
	long double iterationCount;
	unsigned int noOfSleeps;
}freqSleepData;

struct stMemData
{
	unsigned int *src;
	unsigned int *dest;
	unsigned int size;
	// unsigned int size;
}memData;

struct stCpuData
{
	unsigned long int noOfIterations;
}cpuData;

// Cache Line structure which contains start time, end time and number of iterations
// For now to allign with Intel cache line structure, adding two temporary variables
struct stCacheLine
{
	time_t startTime, endTime;
	long double iterations;
	long double temp1, temp2; // Place holder to make the structure size as 64 bytes to allign with Intel Cache Line structure
}__attribute__((packed));
struct stCacheLine cacheLine;

// Structure for storing the perormance data (time for execution) for each iteration
struct stPerfData
{
	time_t startTime, endTime;
	long double iterations;
}perfData[OUTER_LOOP_CNT];

struct stItrnData
{
	time_t itrTime;
	long double freqSleepIterations;
}itrnData[1000];

/* Prototypes */

void MICRO_1 (struct stMemData memData);
void MICRO_2 (struct stCpuData cpuData);
void MICRO_3 (struct stFreqSleep freqSleepData);
void FillSrcData ();
void ThreadMgmtFunction ();
void ReadArgsFromFile ();
void ReadCmdLineArgs (int argc, char **argv);
void FillItrnData (double TimeDiff, long double itrnCount);
void FillPerfData (struct stCacheLine *cacheLineSt);
void PrintItrnData ();

/* Globals */

unsigned int memOnceFlag=0;
unsigned int cpuOnceFlag=0;
unsigned int freqSleepOnceFlag=0;

/* Function Implementations */

/*********************************************************************************************************************************
Name:        FillPerfData
Arguments:   Cache Line Structure (stCacheLine)
Returns:     NONE
Purpose:     Fills the data from cache line structure to performance data structure for each iteration
Call Tree:   Called from MICRO_2
*********************************************************************************************************************************/
void FillPerfData (struct stCacheLine *cacheLineSt)
{
	static unsigned int index=0;
	perfData[index].startTime = cacheLineSt->startTime;
	perfData[index].endTime = cacheLineSt->endTime;
	perfData[index].iterations = cacheLineSt->iterations;
	printf ("Difference in times after copying to perfData at index %d is: %ld\n", index, (perfData[index].endTime - perfData[index].startTime));
	printf ("Number of iterations are: %Lf\n", perfData[index].iterations);
	index++;
}


/*********************************************************************************************************************************
Name:        MICRO_1
Arguments:   Thread Arguments Structure (stThreadArguments)
Returns:     NONE
Purpose:     Calls FillSrcData function to fill the data in the source buffer. As of now it's hard coded
             data, but will be taken from the configuration file later.
             Based on the memory size copies the data to the destination buffer using memcpy.
Call Tree:   Called from threadHandlerFunc
*********************************************************************************************************************************/
void MICRO_1 (struct stMemData memData)
{
	unsigned int i;
	memOnceFlag = memOnceFlag+1;
	printf ("***MEMORY OPERATION START***\n");

	struct stMemData *memDataPtr;
	memDataPtr = (struct stMemData *) malloc ((memData.size)*sizeof(struct stMemData));
	for (i=0; i<memData.size; i++)
	{
		(memDataPtr+i)->src=i+100;
	}
	printf ("Source Data is: \n");
	for (i=0; i<memData.size; i++)
	{
		printf ("%d\t", ((memDataPtr+i)->src));
	}
	printf ("\n");
	for (i=0; i<memData.size; i++)
	{
		(memDataPtr+i)->dest = (memDataPtr+i)->src;
	}
	printf ("Destination Data is: \n");
	for (i=0; i<memData.size; i++)
	{
		printf ("%d\t", ((memDataPtr+i)->dest));
	}
	printf ("\n");
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
	cpuOnceFlag = cpuOnceFlag+1;
	printf ("***CPU OPERATION START***\n");
	// Marking the start time
	cacheLine.startTime = time(NULL);
	// cacheLine.iterations = i;
	for (l=0; l<cpuData.noOfIterations; l++)
	{
		int m = rand();
	}
	// Marking the end time
	cacheLine.endTime = time(NULL);
	// Incrementing the iterations by 1
	cacheLine.iterations = cacheLine.iterations + 1;
	// printf ("Difference in timing for thread ID %d is: %ld\n", args->index, cacheLine.endTime - cacheLine.startTime);
	// Filling the performace data structure for each thread with cacheLine data
	FillPerfData (&cacheLine);
	printf ("***CPU OPERATION END***\n");
}


void FillItrnData (double TimeDiff, long double itrnCount)
{
	static unsigned int index=0;
	itrnData[index].itrTime = TimeDiff;
	itrnData[index].freqSleepIterations = itrnCount;
	printf ("DATA FILLED\n");
	index++;
}

/*********************************************************************************************************************************
Name:        addParameters
Arguments:   a, b
Returns:     sum of the arguments
Purpose:     Adds the parameters passed as arguments and returns the sum
Call Tree:   Called from MICRO_3
*********************************************************************************************************************************/
void addParameters (long double iterations)
{
	unsigned int ind=0;
	double sum = 0;
    	double add = 1;
	struct timespec ts;
	uint64_t start_us;
	uint64_t end_us;
	uint64_t elapsed;
	
	static unsigned int itrTemp = 10;
	iterations = iterations * itrTemp;
	itrTemp = itrTemp * 10;
	
	printf ("iterations at start is              ----------------------------- %Lf\n", iterations);
	// return sum;
	// clock_t start = clock();
	// printf ("iterations is: %ld\n", iterations);
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	start_us = SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
	// printf ("Start Time is: %ld microseconds\n", start_us);
	for (ind=0; ind<iterations; ind++)
	{
		sum += add;
        add /= 2.0;
	}
	// clock_t end = clock();
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	end_us = SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
	// printf ("End Time is: %ld microseconds\n", end_us);
	
	elapsed = end_us - start_us;

	printf ("Elapsed time in uS is: %ld\n", elapsed);

	// struct timespec ts;
	// clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	// end_us = SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
	// printf ("Time in uS is: %ld\n", end_us);
	// printf ("ITERATIONS CALCULATED IS: %Lf\n", iterations);
	FillItrnData (elapsed, iterations);
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
void MICRO_3 (struct stFreqSleep freqSleepData)
{
	unsigned int i=0;
	// static unsigned int sum=0;	
	freqSleepOnceFlag = freqSleepOnceFlag+1;
	printf ("\n***FREQUENT SLEEP AND WAKE OPERATION START***\n");
	for (i=0; i<freqSleepData.noOfSleeps; i++)
	{
		usleep(freqSleepData.sleepDuration);
		addParameters (freqSleepData.iterationCount);
	}
	PrintItrnData ();
	printf ("\n***FREQUENT SLEEP AND WAKE OPERATION END***\n");
}


void PrintItrnData ()
{
	unsigned int index=0;
	for (index=0; index <freqSleepData.noOfSleeps; index++)
	{
		printf ("Time difference for the Iteration count %Lf is %ld microseconds\n", itrnData[index].freqSleepIterations, itrnData[index].itrTime);
	}
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
	struct stThreadArguments *threadArguments = args;
	// struct stMemData *threadMemData;

	if ((threadArguments->MemoryFlag == TRUE) && (memOnceFlag == 0))
	{
		MICRO_1 (memData);
	}
	
	if ((threadArguments->CPUFlag == TRUE) && (cpuOnceFlag == 0))
	{
		MICRO_2 (cpuData);
	}

	if ((threadArguments->freqSleepFlag == TRUE) && (freqSleepOnceFlag == 0))
	{
		MICRO_3 (freqSleepData);
	}
	pthread_exit(NULL);
}

void ReadCmdLineArgs (int argc, char **argv)
{
	unsigned int POS=0;

	threadArgs.functionFlag = atoi(argv[1]);
	// threadArgs.noOfThreads = atoi(argv[2]);
	threadArgs.noOfThreads = get_nprocs();
	cpuData.noOfIterations = atoi(argv[2]);
	freqSleepData.sleepDuration = atoi(argv[3]);
	memData.size = atoi(argv[4]);
	freqSleepData.iterationCount = atoi(argv[5]);
	freqSleepData.noOfSleeps = atoi(argv[6]);

	printf ("Function flag is: %d\n", threadArgs.functionFlag);
	printf ("***READING ARGUMENTS FROM COMMAND LINE***\n\nValue of argc and argv's are: %d\t%s\t%s\t%s\t%s\t%s\t%s\n", argc, argv[0],argv[1], argv[2], argv[3], argv[4], argv[5]);

	POS=0;
	if (1 & (threadArgs.functionFlag >> POS))
	{
		threadArgs.MemoryFlag = 1;
	}
	POS++;
	if (1 & (threadArgs.functionFlag >> POS))
	{
		threadArgs.CPUFlag = 1;
	}
	POS++;
	if (1 & (threadArgs.functionFlag >> POS))
	{
		threadArgs.freqSleepFlag = 1;
	}

	printf ("Thread parameters filled are: \n");
	printf ("MemoryFlag is: %d\n", threadArgs.MemoryFlag);
	printf ("CPUFlag is: %d\n", threadArgs.CPUFlag);
	printf ("freqSleepFlag is: %d\n", threadArgs.freqSleepFlag);
	printf ("noOfThreads is: %d\n", threadArgs.noOfThreads);
	printf ("noOfIterations is: %ld\n", cpuData.noOfIterations);
	printf ("sleepDuration is: %d\n", freqSleepData.sleepDuration);
	printf ("size is: %d\n", memData.size);
	printf ("Iteration Count is: %Lf\n", freqSleepData.iterationCount);
	printf ("Number of sleeps is: %d\n", freqSleepData.noOfSleeps);
}

/*********************************************************************************************************************************
Name:        ReadArgsFromFile
Arguments:   NONE
Returns:     NONE
Purpose:     Opens the inputFile_1.config file and reads the data. Once the data is read it is copied to the
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
	
	printf ("***READING ARGUMENTS FROM FILE***\n");

	fptr = fopen ("inputFile_1.config", "r");

	if (fptr == NULL)
	{
		printf ("FILE OPENING ERROR\n");
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

	threadArgs.functionFlag = atoi(str[0]);
	// threadArgs.noOfThreads = atoi(str[1]);
	threadArgs.noOfThreads = get_nprocs();
	cpuData.noOfIterations = atoi(str[1]);
	freqSleepData.sleepDuration = atoi(str[2]);
	memData.size = atoi(str[3]);
	freqSleepData.iterationCount = atoi(str[4]);
	freqSleepData.noOfSleeps = atoi(str[5]);

	printf ("Function flag is: %d\n", threadArgs.functionFlag);

	POS=0;
	if (1 & (threadArgs.functionFlag >> POS))
	{
		threadArgs.MemoryFlag = 1;
	}
	POS++;
	if (1 & (threadArgs.functionFlag >> POS))
	{
		threadArgs.CPUFlag = 1;
	}
	POS++;
	if (1 & (threadArgs.functionFlag >> POS))
	{
		threadArgs.freqSleepFlag = 1;
	}


	printf ("Thread parameters filled are: \n");
	printf ("MemoryFlag is: %d\n", threadArgs.MemoryFlag);
	printf ("CPUFlag is: %d\n", threadArgs.CPUFlag);
	printf ("freqSleepFlag is: %d\n", threadArgs.freqSleepFlag);
	printf ("noOfThreads is: %d\n", threadArgs.noOfThreads);
	printf ("noOfIterations is: %ld\n", cpuData.noOfIterations);
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
	pthread_t threadID[threadArgs.noOfThreads];

	// printf ("Size of cacheLine structure is: %ld\n", sizeof(cacheLine));

	for (i=0; i<threadArgs.noOfThreads; i++)
	{
		threadArgs.index = i;
		/* Fill Arguments Data */
		retThreadCreate = pthread_create (&threadID[i], NULL, threadHandlerFunc, (void *) &threadArgs);
		// pthread_join (threadID[i], NULL);
		if (retThreadCreate)
		{
			printf ("Thread Creation Error\n");
			exit (-1);
		}
		else
		{
			printf ("Thread %d creation successful\n", i);
		}
	}
	pthread_exit(NULL);
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
	if (argc > 1)
	{
		ReadCmdLineArgs (argc, argv);
	}
	else
	{
		ReadArgsFromFile ();
	}
	// long number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);
	// printf ("Number of processors are: %ld\n", number_of_processors);
	// ValidateArguments ();
	ThreadMgmtFunction ();
	return 0;
}

// Rough Work

/*
ThreadMgmtFunction (unsigned int mixed, unsigned int noOfIterations, memorySize, sleepDuration, percentageMemory, percentageCPU, numberOfFreqSleeps);
1. Mixed parameter one needed if we want to execute both CPU and Memory or only CPU or only Memory or only frequent sleep.
   IDEA: Take one 3 bit binary number (as of now) and based on the bit set execute the specific functionality.
         Also have one basic value which says all functions need to be executed.

2. Work on getting number of CPUs, VMs, VCPUs etc., and based on these threads to be exeucted based on cores.
3. Cache related programming.
4. The mmap() function is used for mapping between a process address space and either files or devices. When a file is mapped to a process address space, the file can be accessed like an array in the program.
*/
