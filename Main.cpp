//#include <stdio.h>
//#include <thread>
//#include <vector>
//#include <stdlib.h>
//#include <time.h>
//#include <chrono>
//#include <random>
//
//#include <set>
//#include <mutex>
//
//#include "StampedMarkableReference.h"
//
////#include "LockFreeLinkedList.h"
//#include "LockFreeHashSet.h"
//
//#define NUMTHREAD 32
//#define SLEEPTIME 1
//#define RANGE 10000
//
//std::random_device rd;
//std::mutex mutex;
//
//std::set<int> testList;
////LockFreeList<int> list;
////BucketList<int> list;
//LockFreeHashSet<int> list(100000);
//
//void addThread(int id)
//{
//	//srand(time(NULL));
//	for (int i = 0; i < RANGE; i++)
//	{
//		list.add(i);
//	}
//	for (int i = 0; i < RANGE; i++)
//	{
//		list.remove(i);
//	}
//
//	while (true)
//	{
//		int num = rd() % RANGE;
//		//bool flag;
//		//{
//		//	std::lock_guard<std::mutex> lock(mutex);
//		//	if (testList.insert(num).second != list.add(num))
//		//	{
//		//		printf("fuck me!");
//		//	}
//
//		//}
//		////printf("Thread%d %s add %d\n ", id, list.add(num) ? "successfully" : "unsuccessfully", num);
//		//std::this_thread::sleep_for(std::chrono::milliseconds(SLEEPTIME));
//		//for (int i = 0; i < 10; i++)
//		//{
//		//	list.remove(i);
//		//}
//
//
//		num = rand() % RANGE;
//		printf("Thread%d %s add %d\n ", id, list.add(num) ? "successfully" : "unsuccessfully", num);
//		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEPTIME));
//		num = rd() % RANGE;
//		printf("Thread%d %s remove %d\n ", id, list.remove(num) ? "successfully" : "unsuccessfully", num);
//		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEPTIME));
//	}
//
//}
//
//void removeThread(int id)
//{
//
//	while (true)
//	{
//		int num = rd() % RANGE;
//		//printf("Thread%d %s remove %d\n ", id, list.remove(num) ? "successfully" : "unsuccessfully", num);
//		{
//			std::lock_guard<std::mutex> lock(mutex);
//			/*int a = testList.erase(num);
//			int b = list.remove(num) ? 1 : 0;
//			if (a != b)
//			{
//			printf("%d\n", a);
//			printf("%d\n", b);
//			}*/
//
//			if ((testList.erase(num) != 0) != list.remove(num))
//			{
//				printf("fuck me!");
//			}
//
//		}
//		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEPTIME));
//	}
//
//}
//
//void containsThread(int id)
//{
//	//srand(time(NULL));
//	while (true)
//	{
//		int num = rd() % RANGE;
//		//printf("Thread%d %s contains %d\n ", id, list.contains(num) ? "successfully" : "unsuccessfully", num);
//
//		{
//			std::lock_guard<std::mutex> lock(mutex);
//			if ((testList.find(num) != testList.end()) != list.contains(num))
//			{
//				printf("fuck me!");
//			}
//		}
//
//		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEPTIME));
//	}
//
//}
//
//
//
//int main()
//{
//	//LockFreeList<int> list;
//	//list.add(5);
//	//if (list.add(5) == false)
//	//	printf("no add\n");
//	//if (list.contains(5))
//	//	printf("YOLO\n");
//	//list.remove(5);
//	//if (!list.contains(5))
//	//	printf("YOLO\n");
//
//	//printf("%x", -1 >> 1);
//
//	//BucketList<int> list;
//	//list.add(5);
//	//if (list.add(5) == false)
//	//	printf("no add\n");
//	//if (list.contains(5))
//	//	printf("YOLO\n");
//	//list.remove(5);
//	//if (!list.contains(5))
//	//	printf("YOLO\n");
//
//	//printf("%s\n", testList.is_lock_free() ? "yes" : "no");
//
//
//	std::vector<std::thread> threads;
//	for (int i = 0; i < NUMTHREAD; i++)
//	{
//		threads.push_back(std::thread(addThread, i));
//		//threads.push_back(std::thread(removeThread, i+1));
//		//threads.push_back(std::thread(removeThread, i+3));
//		//threads.push_back(std::thread(containsThread, i+2));
//	}
//
//	for (auto& t : threads)
//	{
//		t.join();
//	}
//
//	//BucketList<int> list;
//	//LockFreeHashSet<int> hashSet(100);
//	//srand(time(NULL));
//
//	//printf("added\n");
//	//for (int i = 0; i < 10; i++)
//	//{
//	//	list.remove(i);
//	//}
//
//	//for (int i = 0; i < 100000; i++)
//	//{
//	//	int num = rand() % RANGE;
//	//	printf("%s add %d\n", list.add(num) ? "successfully" : "unsuccessfully", num);
//	//	std::this_thread::sleep_for(std::chrono::milliseconds(SLEEPTIME));
//	//	num = rand() % RANGE;
//	//	printf("%s remove %d\n", list.remove(num) ? "successfully" : "unsuccessfully", num);
//	//	
//	//}
//
//
//
//	//size_t a = 5;
//	//printf("%p\n", &a);
//	//printf("%llx\n", UINTPTR_MAX);
//
//	//LockFreeHashSet<int> hashSet(100);
//	//hashSet.add(11);
//
//	//printf("%d\n", true);
//
//	printf("done\n");
//	getchar();
//	return 0;
//}





#if _MSC_VER
#define  _CRT_SECURE_NO_WARNINGS
#endif

#include <cstdio>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include "LockFreeHashSet.h"
#include "Header.h"
#include "TestBuilder.h"
#include "tbb/concurrent_unordered_set.h"
#define MAX_NUM_OP 5000
#define CAPACITY 0x200000
#define MAX_NUM_THREAD 16
#define TOTAL_NUM_RUNS 5
#define ADD_OP_PERCENT 88
#define REMOVE_OP_PERCENT 90
#define TOTAL_PERCENTAGE 100

using namespace std;

bool ready = false;
tbb::concurrent_unordered_set<unsigned int>* tbbset;
LockFreeHashSet<unsigned int>* lockfreeHashSet;
testSet<unsigned int>* testset;
void test_tbb()
{
	TestBuilder tb(MAX_NUM_OP, TOTAL_PERCENTAGE);
	//tbb::concurrent_unordered_set<unsigned int> set;
	while (!ready)
		std::this_thread::yield();
	for (int i = 0; i<MAX_NUM_OP; i++)
	{
		unsigned int num = tb.getNextNum();
		unsigned int op = tb.getNextOp();
		if (op <= ADD_OP_PERCENT)
		{
			tbbset->insert(num);
		}
		else if (op <= REMOVE_OP_PERCENT)
			tbbset->unsafe_erase(num);
		else
			tbbset->find(num);
		tb.nextOp();
	}
}

void test_lockFreeHashSet()
{
	TestBuilder tb(MAX_NUM_OP, TOTAL_PERCENTAGE);
	//LockFreeHashSet<unsigned int> set(CAPACITY);
	while (!ready)
		std::this_thread::yield();
	for (int i = 0; i<MAX_NUM_OP; i++)
	{
		unsigned int num = tb.getNextNum();
		unsigned int op = tb.getNextOp();
		if (op <= ADD_OP_PERCENT)
		{
			lockfreeHashSet->add(num);
		}
		else if (op <= REMOVE_OP_PERCENT)
			lockfreeHashSet->remove(num);
		else
			lockfreeHashSet->contains(num);
		tb.nextOp();
	}
}

void test_unordered_set()
{
	TestBuilder tb(MAX_NUM_OP, TOTAL_PERCENTAGE);
	//testSet<unsigned int> set;
	while (!ready)
		std::this_thread::yield();
	for (int i = 0; i<MAX_NUM_OP; i++)
	{
		unsigned int num = tb.getNextNum();
		unsigned int op = tb.getNextOp();
		if (op <= ADD_OP_PERCENT)
		{
			testset->add(num);
		}
		else if (op <= REMOVE_OP_PERCENT)
			testset->remove(num);
		else
			testset->contains(num);
		tb.nextOp();
	}
}


class Timer
{
public:
	Timer() : beg_(clock_::now()) {}
	void reset() { beg_ = clock_::now(); }
	double elapsed() const {
		return std::chrono::duration_cast<std::chrono::milliseconds>
			(clock_::now() - beg_).count();
	}

private:
	typedef std::chrono::high_resolution_clock clock_;
	typedef std::chrono::duration<double, std::ratio<1> > second_;
	std::chrono::time_point<clock_> beg_;
};


int test_thread(int numOfThreads, std::function<void()> func)
{
	Timer tmr;
	ready = false;
	vector<thread> threads;
	for (int j = 0; j < numOfThreads; j++)
	{
		threads.push_back(thread(func));
	}
	// wait for all threads spawn and allocate test cases
	//this_thread::sleep_for(std::chrono::seconds(1));
	tmr.reset();
	ready = true;
	for (auto &th : threads)
	{
		th.join();
	}
	int t = tmr.elapsed();
	//printf("%d\n", t);
	return t;
}

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress(double percentage)
{
	int val = (int)(percentage * 100);
	int lpad = (int)(percentage * PBWIDTH);
	int rpad = PBWIDTH - lpad;
	printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
	fflush(stdout);
}


int main()
{	
	int result[3][MAX_NUM_THREAD] = { 0 };
	
	for (int n = 0; n < TOTAL_NUM_RUNS; n++)
	{
		printProgress(0.2*n);
		lockfreeHashSet = new LockFreeHashSet<unsigned int>(CAPACITY);
		for (int i = 1; i <= MAX_NUM_THREAD; i++)
		{
			result[0][i - 1] += test_thread(i, test_lockFreeHashSet);
		}
		delete lockfreeHashSet;
		testset = new testSet<unsigned int>();
		for (int i = 1; i <= MAX_NUM_THREAD; i++)
		{
			result[1][i - 1] += test_thread(i, test_unordered_set);
		}
		delete testset;
		tbbset = new tbb::concurrent_unordered_set<unsigned int>();
		for (int i = 1; i <= MAX_NUM_THREAD; i++)
		{
			result[2][i - 1] += test_thread(i, test_tbb);
		}
		delete tbbset;
		
	}
	printProgress(1);
	FILE* pFile;
	char filename[20];
	std::sprintf(filename, "%dAddOp.txt", ADD_OP_PERCENT);
	pFile = std::fopen(filename, "w");
	//fprintf(pFile, "Add Operation Percentage: %d%%", ADD_OP_PERCENT);
	//fprintf(stdout, "Add Operation Percentage: %d%%", ADD_OP_PERCENT);
	fprintf(pFile, "Threads,  lockFreeHashSet,   unorderedset, tbb\n");
	printf("\nThreads,  lockFreeHashSet,   unorderedset, tbb\n");
	for (int i = 0; i < MAX_NUM_THREAD; i++)
	{
		printf("%02d, %8d, %10d, %12d\n",i+1,
			result[0][i]/TOTAL_NUM_RUNS, result[1][i]/TOTAL_NUM_RUNS, result[2][i]/TOTAL_NUM_RUNS);
		fprintf(pFile,"%02d, %8d, %10d, %12d\n", i + 1,
			result[0][i] / TOTAL_NUM_RUNS, result[1][i] / TOTAL_NUM_RUNS , result[2][i] / TOTAL_NUM_RUNS);
	}
	fclose(pFile);
	printf("done\n");
	getchar();
	return 0;
}
