#pragma once

class CriticalSection
{
private:
	CRITICAL_SECTION cs;
public:
	void lock();
	void unlock();
	CriticalSection(void);
	~CriticalSection(void);
};
