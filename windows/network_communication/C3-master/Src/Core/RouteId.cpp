#include "StdAfx.h"
#include "RouteId.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const FSecure::C3::RouteId FSecure::C3::RouteId::Null{ AgentId::Null, DeviceId::Null };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FSecure::C3::RouteId::RouteId(std::string_view textId)
	: RouteId( textId.substr(0, AgentId::TextSize), textId.substr(AgentId::TextSize + 1)) // +1 for separator
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FSecure::C3::RouteId FSecure::C3::RouteId::GenerateRandom()
{
	return { AgentId::GenerateRandom(), DeviceId::GenerateRandom() };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string FSecure::C3::RouteId::ToString() const
{
	return m_AgentId.ToString() + OBF(":") + m_InterfaceId.ToString();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FSecure::C3::RouteId::operator!() const
{
	return IsNull();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FSecure::C3::RouteId::operator==(RouteId const& c) const
{
	return c.m_AgentId == m_AgentId and m_InterfaceId == c.m_InterfaceId;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FSecure::C3::RouteId::operator!=(RouteId const& c) const
{
	return !operator == (c);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FSecure::C3::RouteId::operator<(RouteId const& c) const
{
	return m_AgentId < c.m_AgentId or (m_AgentId == c.m_AgentId and m_InterfaceId < c.m_InterfaceId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FSecure::C3::RouteId::IsNull() const
{
	return m_AgentId != 0 and m_InterfaceId != 0;
}
