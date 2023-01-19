#include "Guest.h"

Guest::Guest()
	: name(""), userID(0), id(0), status(Status::INVALID)
{}

Guest::Guest(string name, uint32_t userID, uint32_t id, Status status)
	: name(name), userID(userID), id(id), status(status)
{}

Guest::Guest(ParsecGuest guest)
	: name(guest.name), userID(guest.userID), id(guest.id), status(Status::OK)
{}

const bool Guest::isValid()
{
	return status == Status::OK;
}

Guest Guest::copy(const Guest& guest)
{
	name = guest.name;
	userID = guest.userID;
	id = guest.id;
	status = guest.status;
	return *this;
}

void GuestMetrics::updateMetrics(ParsecGuest guest)
{
	updateCongestion(guest);
	m_parsecMetrics = guest.metrics[0];
	m_averageLatency.push(m_parsecMetrics.networkLatency);
}

void GuestMetrics::updateCongestion(ParsecGuest guest)
{
	if (guest.metrics[0].slowRTs > m_parsecMetrics.slowRTs) m_congestion = CONGESTION::HIGH;
	else if (guest.metrics[0].fastRTs > m_parsecMetrics.fastRTs) m_congestion = CONGESTION::LOW;
	else m_congestion = CONGESTION::NONE;
}

const float GuestMetrics::getLatency() const
{
	return m_averageLatency.m_value;
}

const GuestMetrics::CONGESTION GuestMetrics::getCongestion() const
{
	return m_congestion;
}

const ParsecMetrics GuestMetrics::getMetrics() const
{
	return m_parsecMetrics;
}

const char* GuestMetrics::toString() const
{
	char metricsAsString[100] = "";
	snprintf(
		metricsAsString,
		sizeof(metricsAsString),
		"NED:%3.1f/%3.1f/3.1f B:%6.1f RT:%d/%d Q:%d P:%d",
		m_parsecMetrics.networkLatency,
		m_parsecMetrics.encodeLatency,
		m_parsecMetrics.decodeLatency,
		m_parsecMetrics.bitrate,
		m_parsecMetrics.fastRTs,
		m_parsecMetrics.slowRTs,
		m_parsecMetrics.queuedFrames,
		m_parsecMetrics.packetsSent
	);

	return metricsAsString;
}

