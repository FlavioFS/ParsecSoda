#pragma once
#include <iostream>

class MovingAverage
{
public:
	void reset();
	void setWeight(float weight);
	void push(float value);

	float m_value = 0;
	uint32_t m_size = 1;
	float m_weight = 0.6;
};

