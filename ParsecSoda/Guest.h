#pragma once

#include <iostream>
#include "parsec.h"
#include "MovingAverage.h"

using namespace std;

class GuestMetrics {
public:
	enum CONGESTION { NONE, LOW, HIGH };

	void updateMetrics(ParsecGuest guest);
	const float getLatency() const;
	const CONGESTION getCongestion() const;
	const ParsecMetrics getMetrics() const;

	std::string toString() const;

private:
	ParsecMetrics m_parsecMetrics { 0, 0, 0, 0, 0, 0, 0, 0 };
	CONGESTION m_congestion;
	MovingAverage m_averageLatency;

	void updateCongestion(ParsecGuest guest);
};

class Guest
{
public:
	enum class Status
	{
		OK = 0,
		INVALID = 1,
		EXPIRED = 2
	};

	/**
	 *  Empty constructor.
	 */
	Guest();

	/**
	 * Full constructor.
	 * @param name Guest username.
	 * @param userId Unique user ID.
	 * @param id Internal id for specific methods.
	 */
	Guest(string name, uint32_t userID, uint32_t id, Status status = Status::OK);
	
	/**
	 * Compatibility constructor.
	 * @param guest ParsecGuest object.
	 */
	Guest(ParsecGuest guest);

	const bool isValid();

	Guest copy(const Guest& guest);


	string name;
	uint32_t userID;
	uint32_t id;
	Status status;
	GuestMetrics metrics;
};