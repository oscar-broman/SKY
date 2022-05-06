#pragma once

#include "amx/amx.h"
#include "sdk.hpp"
#include "types.hpp"
#include "Server/Components/Pawn/pawn.hpp"
#include "Server/Components/TextDraws/textdraws.hpp"

#include "Shared/bitstream.hpp"
#include "Shared/packet.hpp"

#include "network.hpp"
#include "player.hpp"
#include "global.hpp"
#include "scripting.hpp"

#ifdef _WIN32
#define OS_NAME "Windows"
#else
#define OS_NAME "Linux"
#endif

#define PROJECT_NAME "SKY"
#define PROJECT_MAJOR 2
#define PROJECT_MINOR 3
#define PROJECT_PATCH 0

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
