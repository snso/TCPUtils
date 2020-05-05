#pragma once

#include<chrono>

class YTimeUtils
{
public:
	YTimeUtils();
	~YTimeUtils();

	///更新时间
	void Update();

	/**
	*   获取当前秒
	*/
	double GetElapsedSecond();

	/**
	*   获取毫秒
	*/
	double GetElapsedTimeInMilliSec();

	/**
	*   获取微妙
	*/
	long long GetElapsedTimeInMicroSec();

protected:

	std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;


};

