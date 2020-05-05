#pragma once

#include<chrono>

class YTimeUtils
{
public:
	YTimeUtils();
	~YTimeUtils();

	///����ʱ��
	void Update();

	/**
	*   ��ȡ��ǰ��
	*/
	double GetElapsedSecond();

	/**
	*   ��ȡ����
	*/
	double GetElapsedTimeInMilliSec();

	/**
	*   ��ȡ΢��
	*/
	long long GetElapsedTimeInMicroSec();

protected:

	std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;


};

