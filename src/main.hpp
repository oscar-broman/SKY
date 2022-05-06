#pragma once

#include "amx/amx.h"
#include "sdk.hpp"
#include "types.hpp"
#include "Server/Components/Pawn/pawn.hpp"
#include "Server/Components/TextDraws/textdraws.hpp"

#include "Shared/bitstream.hpp"
#include "Shared/packet.hpp"

#include "player.hpp"
#include "global.hpp"
#include "scripting.hpp"
#include "network.hpp"

class SkyComponent final : public IComponent,
                           public PawnEventHandler,
                           public PlayerEventHandler
{
    PROVIDE_UID(0xecaf15d2bc782d14);

    StringView componentName() const override
    {
        return "SKY";
    }

    SemanticVersion componentVersion() const override;

    void onLoad(ICore *c) override;

    void onInit(IComponentList *components) override;

    void onAmxLoad(void *amx) override;

    void onAmxUnload(void *amx) override;

    void onFree(IComponent *component) override;

    void reset() override;

    void free() override;

public:
    static ICore *&getCore();
    static ITextDrawsComponent *&getTextDraws();
    static void Log(LogLevel level, const char *fmt, ...);

private:
    ICore *core_{};
    IPlayerPool *players_{};
    IPawnComponent *pawn_component_{};
    ITextDrawsComponent *textdraw_component_{};
};
