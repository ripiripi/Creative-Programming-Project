#pragma once

class TimeController {
private:
	long long StartTime;
	long long get_time_millisec(void);
public:
	void Reset();
	long long Current();
};