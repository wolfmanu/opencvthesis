#pragma once

class CriticalSection
{
//private:
public:
	CRITICAL_SECTION cs;
public:
	void lock();
	void unlock();
	CriticalSection(void);
	~CriticalSection(void);
};
