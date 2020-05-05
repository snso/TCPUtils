#include "YTimeUtils.h"



YTimeUtils::YTimeUtils()
{
}


YTimeUtils::~YTimeUtils()
{
}

void YTimeUtils::Update()
{
	m_begin = std::chrono::high_resolution_clock::now();
}

double YTimeUtils::GetElapsedSecond()
{
	return GetElapsedTimeInMicroSec() * 0.000001;
}

double YTimeUtils::GetElapsedTimeInMilliSec()
{
	return GetElapsedTimeInMicroSec() * 0.001;
}

long long YTimeUtils::GetElapsedTimeInMicroSec()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
}
