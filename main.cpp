#include <algorithm>
#include <execution>

#include <Usagi/Extension/Win32/Win32.hpp>
#include <Usagi/Extension/Win32/Win32MacroFix.hpp>

#include <Usagi/Utility/Utf8Main.hpp>
#include <Usagi/ECS/Game.hpp>
#include <Usagi/ECS/Entity.hpp>
#include <Usagi/ECS/GameContext.hpp>
#include <fmt/ostream.h>

using namespace usagi::ecs;

struct PositionComponent
{
    int x = 0, y = 0;
};

struct RGBColorComponent
{
    int r = 0, g = 0, b = 0;
};

class CosineWaveValueUpdateSystem : public FrameUpdateSystem
{
    Entity::ComponentMask mComponentMask;

public:
    void init(GameContext &ctx)
    {
        mComponentMask = ctx.entity_manager.buildComponentMask<RGBColorComponent>();
    }

    void execute() override
    {
        // std::for_each(
        //     std::execution::par,
        //     ctx.entities.begin(),
        //     ctx.entities.end(),
        //     [](auto &&e) {
        //     if(e.matchMask())
        // })
    }
};

int usagi_main(const std::vector<std::string> &args)
{
    Game game;

    GameContext ctx;
    fmt::print("{}\n", ctx.entity_manager.buildComponentMask<RGBColorComponent>());
    fmt::print("{}\n", ctx.entity_manager.buildComponentMask<RGBColorComponent,PositionComponent>());

    return 0;
}
