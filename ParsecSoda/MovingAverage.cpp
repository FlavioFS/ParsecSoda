#include "MovingAverage.h"

void MovingAverage::reset()
{
	m_value = 0;
	m_size = 1;
}

void MovingAverage::setWeight(float weight)
{
	if (weight < 0) return;
	if (weight < 0.5f) weight = 1 - weight;
	m_weight = weight;
}

void MovingAverage::push(float value)
{
	if (m_size <= 0) m_size = 1;
	m_value = m_weight * value + (1 - m_weight) * m_value;
}