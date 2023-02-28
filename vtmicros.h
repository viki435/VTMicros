#ifndef _VTMICROS_H
#define _VTMICROS_H

// Macros
#define OUTER_LOOP_CNT 10
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

struct thread_info
{
	pthread_t thread_id; // ID returned by pthread_create()
	int core_id; // Core ID we want this pthread to set its affinity to
};
// This is the structure that can be used to pass the required arguments to the threads. As of now, only the current taskID and sleep duration are added
struct stThreadArguments
{
    enum state MemoryFlag;
    enum state CPUFlag;
    enum state freqSleepFlag;
    enum state NetworkFlag;
    enum state DiskFlag;
    unsigned int noOfThreads;
    /* New Design */
    unsigned int microFlag;
	int core_id;
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
}memData;

struct stCpuData
{
    long double noOfIterations;
}cpuData;

// Cache Line structure which contains start time, end time and number of iterations
// For now to allign with Intel cache line structure, adding two temporary variables
struct stCacheLine
{
    // time_t startTime, endTime;
	time_t elapsedTime;
    long double iterations;
    long double temp1, temp2; // Place holder to make the structure size as 64 bytes to allign with Intel Cache Line structure
	unsigned int temp3, temp4;
}__attribute__((packed));
struct stCacheLine cacheLine[100][100];

// Structure for storing the perormance data (time for execution) for each iteration
struct stPerfData
{
    time_t startTime, endTime;
    long double iterations;
}perfData[OUTER_LOOP_CNT];

struct stMemTime
{
    time_t startTime, endTime;
    long double iterations;
}memTime;

struct stItrnData
{
    time_t itrTime;
    long double freqSleepIterations;
}itrnData[1000];

/* Prototypes */

void MICRO_1 (struct stMemData memData);
void MICRO_2 (struct stCpuData cpuData);
void MICRO_3 (struct stFreqSleep freqSleepData, int coreID, pthread_t pid);
void FillSrcData ();
void ThreadMgmtFunction ();
void ReadArgsFromFile ();
void ReadCmdLineArgs (int argc, char **argv);
void FillPerfData (time_t startT, time_t endT, long double itrns);
void PrintItrnData ();
void ValidateArguments (int argCount);
void InitializeDefaults ();

/* Globals */

unsigned int memOnceFlag=0;
unsigned int cpuOnceFlag=0;
unsigned int freqSleepOnceFlag=0;
unsigned int srcArray[1000];

#endif