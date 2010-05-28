#pragma once

class Guard {

	CriticalSection& cs;
public:
	Guard(CriticalSection& cs): cs(cs) {
		cs.lock();
	}

	~Guard() {
		cs.unlock();
	}
};
