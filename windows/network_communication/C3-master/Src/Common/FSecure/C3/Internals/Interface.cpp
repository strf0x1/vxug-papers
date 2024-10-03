#include "StdAfx.h"
#include "Interface.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FSecure::C3::AbstractPeripheral::OnReceive()
{
	if (auto bridge = GetBridge(); bridge)
		if (auto command = OnReceiveFromPeripheral(); !command.empty())
			bridge->PostCommandToConnector(command);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FSecure::C3::AbstractChannel::OnReceive()
{
	if (auto bridge = GetBridge(); bridge)
		for (auto&& packet : OnReceiveFromChannelInternal())
			bridge->PassNetworkPacket(packet);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FSecure::C3::Device::SetUpdateDelay(std::chrono::milliseconds minUpdateDelayInMs, std::chrono::milliseconds maxUpdateDelayInMs)
{
	// Sanity checks.
	if (minUpdateDelayInMs > maxUpdateDelayInMs)
		throw std::invalid_argument{ OBF("maxUpdateDelay must be greater or equal to minUpdateDelay.") };
	if (minUpdateDelayInMs < 30ms)
		throw std::invalid_argument{ OBF("minUpdateDelay must be greater or equal to 30ms.") };

	std::lock_guard<std::mutex> guard(m_UpdateDelayMutex);
	m_MinUpdateDelay = minUpdateDelayInMs;
	m_MaxUpdateDelay = maxUpdateDelayInMs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FSecure::C3::Device::SetUpdateDelay(std::chrono::milliseconds frequencyInMs)
{
	std::lock_guard<std::mutex> guard(m_UpdateDelayMutex);
	m_MinUpdateDelay = frequencyInMs;
	m_MaxUpdateDelay = m_MinUpdateDelay;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::chrono::milliseconds FSecure::C3::Device::GetUpdateDelay() const
{
	std::lock_guard<std::mutex> guard(m_UpdateDelayMutex);
	return m_MinUpdateDelay != m_MaxUpdateDelay ? FSecure::Utils::GenerateRandomValue(m_MinUpdateDelay, m_MaxUpdateDelay) : m_MinUpdateDelay;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FSecure::ByteVector FSecure::C3::Device::OnRunCommand(ByteView command)
{
	switch (command.Read<uint16_t>())
	{
		case static_cast<uint16_t>(FSecure::C3::Command::Close):
		return Close(), ByteVector{};
	case static_cast<uint16_t>(FSecure::C3::Command::UpdateJitter) :
	{
		auto [minVal, maxVal] = command.Read<float, float>();
		return SetUpdateDelay(FSecure::Utils::ToMilliseconds(minVal), FSecure::Utils::ToMilliseconds(maxVal)), ByteVector{};
	}
	default:
		throw std::runtime_error(OBF("Device received an unknown command"));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FSecure::ByteVector FSecure::C3::AbstractConnector::OnRunCommand(ByteView command)
{
	switch (command.Read<uint16_t>())
	{
		case static_cast<uint16_t>(-1) :
			return TurnOff(), ByteVector{};
		default:
			throw std::runtime_error(OBF("AbstractConnector received an unknown command"));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FSecure::C3::Device::Close()
{
	GetBridge()->Close();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FSecure::C3::AbstractConnector::TurnOff()
{
	GetBridge()->TurnOff();
}
