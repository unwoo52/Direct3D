#pragma once
#include<chrono>
using namespace std;
using namespace chrono;

class Time 
{
#ifdef _WIN32
	time_point<steady_clock> start;
	time_point<steady_clock> stop;
#else
	time_point<system_clock> start;
	time_point<system_clock> stop;
#endif
	bool isRunning = false;	
	static double deltaTime;
public:
	Time();
	double GetMilisecondsElapsed();
	bool Start();
	bool Stop();
	void Restart();
	void UdateDeltaTime();
	static double GetDeltaTime();
};

