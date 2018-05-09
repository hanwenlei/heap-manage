#ifndef _CL_STORAGESTRUCT_H_
#define _CL_STORAGESTRUCT_H_

#include<atomic>

using namespace std;

class CL_ThreadCache;

typedef struct Small
{
	CL_ThreadCache* OwnerThreadCache;
	struct Small* NextPtr;
	unsigned int size;
	//char data[0];
}Small;

typedef struct Large
{
	struct Large* NextPtr;
	struct Large* PrevPtr;
	unsigned int size;
	//char data[0];
}Large;

typedef struct Huge
{
	struct Huge* NextPtr;
	struct Huge* PrevPtr;
	unsigned int size;
	//char data[0];
}Huge;

typedef struct
{
	void* Element;
	atomic_bool IsEmpty;
}Node;

#endif