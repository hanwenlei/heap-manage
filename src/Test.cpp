#include"Test.h"
#include<process.h>

vector<HANDLE> g_threads;

int display_mallinfo()
{
/*    struct mallinfo mi;
	mi = mallinfo();
    printf("Total non-mmapped bytes (arena):       %d\n", mi.arena);
    printf("# of free chunks (ordblks):            %d\n", mi.ordblks);
    printf("# of free fastbin blocks (smblks):     %d\n", mi.smblks);
    printf("# of mapped regions (hblks):           %d\n", mi.hblks);
    printf("Bytes in mapped regions (hblkhd):      %d\n", mi.hblkhd);
    printf("Max. total allocated space (usmblks):  %d\n", mi.usmblks);
    printf("Free bytes held in fastbins (fsmblks): %d\n", mi.fsmblks);
    printf("Total allocated space (uordblks):      %d\n", mi.uordblks);
    printf("Total free space (fordblks):           %d\n", mi.fordblks);
    printf("Topmost releasable block (keepcost):   %d\n", mi.keepcost);
*/
    return 0;
}

int Test()
{
	const int MAX_ALLOCS = 2000000;
    char *alloc[MAX_ALLOCS];
    int numBlocks = 128, j;
	int freeBegin = 1, freeEnd = 2000, freeStep = 5;
    size_t blockSize = 1024;
    
    display_mallinfo();
	for(j = 0; j < numBlocks; j++)
	{
        if(numBlocks >= MAX_ALLOCS)
        {
        	printf("Too many allocations\n");
		}
		alloc[j] = (char*)my_malloc(blockSize);
        if (alloc[j] == NULL)
        {
        	printf("malloc\n");
		}
    }
    
    display_mallinfo();
	for(j = freeBegin; j < freeEnd; j += freeStep)
	{
        my_free(alloc[j]);
	}
    display_mallinfo();
	return 0;
}

int VectorMemoryCost()
{  
    printf("malloc test\n");
    int blocksize = 512;
    for(int j = 0; j < 20; j++)
	{
        const int sum=25000;
        char* p1[sum];
        for(int i = 0; i < sum; i++)
		{
            p1[i]=(char*)my_malloc(blocksize);
        }
        
        for(int i = 0; i < sum; i++)
		{
            my_free(p1[i]);
        }
    }
    return 0;
}

unsigned int ThreadMallocFunction(void*)
{
	clock_t start = clock();
	MSG msg;
	srand((unsigned)time(0));

	for(int i = 0; i < 250000; i++)
	{
		int size = rand() % SizeOfPage + 1;
//		void* temp = my_malloc(size);
		void* temp = malloc(size);

		if (temp != NULL)
		{
			int index = rand() % 4;
			if (g_threads[index] == GetCurrentThread())
			{
				index = (index + 1) % 4;
			}
			PostThreadMessage(GetThreadId(g_threads[index]), WM_MALLOC, 0, (LPARAM)temp);
		}

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
//			my_free((void*)msg.lParam);
			free((void*)msg.lParam);
		}
	}

	clock_t finish = clock();
	double DuringTime = (finish - start) / CLOCKS_PER_SEC;
	printf("DuringTime : %lf seconds\n", DuringTime);
	return 0;
}

unsigned int ThreadFreeFunction(void*)
{
	MSG msg;
	while (true)
	{
		GetMessage(&msg, 0, 0, 0);
		if (msg.message == WM_MALLOC)
		{
//			my_free((void*)msg.lParam);
			free((void*)msg.lParam);
		}
		else
		{
			break;
		}
	}
	return 0;
}

int MyAllocate()
{
	for (int i = 0; i < 4; i++)
	{
		HANDLE temp = (HANDLE)_beginthreadex(NULL, 0, (unsigned(_stdcall*)(void*))ThreadMallocFunction, 0, 0, 0);
		g_threads.push_back(temp);
	}
    return 0;
}
