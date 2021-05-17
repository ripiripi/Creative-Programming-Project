#include "myHeader.h"
#include "Class_TimeController.h"

using namespace std::chrono;

long long TimeController::get_time_millisec(void) {
	return static_cast<long long>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

void TimeController::Reset() {
	StartTime = get_time_millisec();
}

long long TimeController::Current() {
	long long Time = get_time_millisec();
	return Time - StartTime;
}