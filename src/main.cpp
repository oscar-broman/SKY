//----------------------------------------------------------
//
//   SA:MP Multiplayer Modification For GTA:SA
//   Copyright 2004-2007 SA:MP Team
//
//----------------------------------------------------------

#include "main.hpp"

#include "scripting.hpp"
#include "utils.hpp"

extern void *pAMXFunctions;

SemanticVersion SkyComponent::componentVersion() const
{
	return SemanticVersion(PROJECT_MAJOR, PROJECT_MINOR, PROJECT_PATCH, 0);
}

void SkyComponent::onLoad(ICore *c)
{
	core_ = c;
	players_ = &c->getPlayers();

	getCore() = c;

	// make sure bitstream versions match core
	if (NetworkBitStream::Version != c->getNetworkBitStreamVersion())
	{
		core_->logLn(LogLevel::Error, "Bitstream mismatch from SDK/Core");
	}

	// packet handlers
	PlayerSync ps;
	AimSync as;
	VehicleSync vs;
	PassengerSync pas;
	SpectatorSync ss;
	TrailerSync ts;

	c->addPerPacketInEventHandler<NetCode::Packet::PlayerFootSync::PacketID>(&ps);
	c->addPerPacketInEventHandler<NetCode::Packet::PlayerAimSync::PacketID>(&as);
	c->addPerPacketInEventHandler<NetCode::Packet::PlayerVehicleSync::PacketID>(&vs);
	c->addPerPacketInEventHandler<NetCode::Packet::PlayerPassengerSync::PacketID>(&pas);
	c->addPerPacketInEventHandler<NetCode::Packet::PlayerSpectatorSync::PacketID>(&ss);
	c->addPerPacketInEventHandler<NetCode::Packet::PlayerTrailerSync::PacketID>(&ts);

	// show version
	ShowPluginInfo();
}

void SkyComponent::onInit(IComponentList *components)
{
	StringView name = componentName();

	pawn_component_ = components->queryComponent<IPawnComponent>();
	if (!pawn_component_)
	{
		core_->logLn(LogLevel::Error,
					 "Error loading component %.*s: Pawn component not loaded",
					 name.length(), name.data());

		return;
	}

	textdraw_component_ = components->queryComponent<ITextDrawsComponent>();
	if (!textdraw_component_)
	{
		core_->logLn(LogLevel::Error,
					 "Error loading component %.*s: Textdraw component not loaded",
					 name.length(), name.data());

		return;
	}

	pawn_component_->getEventDispatcher().addEventHandler(this);
	players_->getEventDispatcher().addEventHandler(this);

	pAMXFunctions = (void *)&pawn_component_->getAmxFunctions();
}

void SkyComponent::onAmxLoad(void *amx)
{
	InitScripting((AMX *)amx);
};

void SkyComponent::onAmxUnload(void *amx){};

void SkyComponent::onFree(IComponent *component)
{
	if (component == pawn_component_ || component == this)
	{
		pawn_component_ = nullptr;
		pAMXFunctions = nullptr;
	}
}

void SkyComponent::reset() {}

void SkyComponent::free()
{
	if (pawn_component_)
	{
		pawn_component_->getEventDispatcher().removeEventHandler(this);
		players_->getEventDispatcher().removeEventHandler(this);
	}

	delete this;
}

void SkyComponent::Log(LogLevel level, const char *fmt, ...)
{
	auto core = getCore();
	if (!core)
	{
		return;
	}

	va_list args{};
	va_start(args, fmt);
	core->vlogLn(level, fmt, args);
	va_end(args);
}

ITextDrawsComponent *&SkyComponent::getTextDraws()
{
	static ITextDrawsComponent *textdraws{};
	return textdraws;
}

ICore *&SkyComponent::getCore()
{
	static ICore *core{};
	return core;
}

COMPONENT_ENTRY_POINT()
{
	return new SkyComponent();
}