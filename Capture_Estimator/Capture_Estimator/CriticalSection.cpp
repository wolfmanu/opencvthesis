#include "StdAfx.h"
#include "CriticalSection.h"

CriticalSection::CriticalSection(void)
{
	InitializeCriticalSection(&cs);
}

CriticalSection::~CriticalSection(void)
{
	DeleteCriticalSection(&cs);
}

void CriticalSection::lock() {
	EnterCriticalSection(&cs);
}
void CriticalSection::unlock() {
	LeaveCriticalSection(&cs);
}