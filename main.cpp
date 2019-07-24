#include <algorithm>
#include <execution>

#include <fmt/ostream.h>

#include <Usagi/Extension/Win32/Win32.hpp>
#include <Usagi/Extension/Win32/Win32MacroFix.hpp>

#include <Usagi/Utility/Utf8Main.hpp>
#include <Usagi/ECS/Game2.hpp>

using namespace usagi::ecs;

struct PositionComponent
{
    int x = 0, y = 0;
};

struct FloatValueComponent
{
    float value = 0;
};

struct RGBColorComponent
{
    int r = 0, g = 0, b = 0;
};

class CosineWaveValueUpdateSystem : public FrameUpdateSystem
{
public:
    void execute(GameContext &ctx) override
    {
        ctx.execution.parallel<
            PositionComponent,
            FloatValueComponent
        >([&, this](auto &c_pos, auto &c_float_val) {
            const auto t = static_cast<float>(ctx.master_clock.totalElapsed());
            c_float_val.value = std::abs(
                std::cos(c_pos.x + t) +
                std::cos(c_pos.y * 2 + t)
            );
        });
    }
};

class ColorMappingSystem : public FrameUpdateSystem
{
public:
    void execute(GameContext &ctx) override
    {
        ctx.execution.parallel<
            FloatValueComponent,
            RGBColorComponent
        >([this](auto &c_float_val, auto &c_color) {
            c_color.r = static_cast<int>(c_float_val.value * 255) % 255;
            c_color.g = static_cast<int>(c_float_val.value * 512) % 255;
            c_color.b = static_cast<int>(c_float_val.value * 666) % 255;
        });
    }
};

class ConsolePixelDrawSystem : public FrameUpdateSystem
{
public:
    // https://stackoverflow.com/questions/12378642/c-pixels-in-console-window
    void execute(GameContext &ctx) override
    {
        const auto console = GetConsoleWindow();
        const auto dc = GetDC(console);
        ctx.execution.sequential<
            PositionComponent,
            RGBColorComponent
        >([&, this](auto &c_pos, auto &c_color) {
            SetPixel(
                dc,
                c_pos.x, c_pos.y,
                RGB(c_color.r, c_color.g, c_color.b)
            );
        });
        ReleaseDC(console, dc);
    }
};

class PixelInitSystem : public GameInitSystem
{
public:
    void execute(GameContext &ctx) override
    {
        for(auto i = 0; i < 100; ++i)
        {
            for(int j = 0; j < 100; ++j)
            {
                ctx.entity_manager.addEntity()
                    .addComponent<PositionComponent>(i, j)
                    .addComponent<FloatValueComponent>()
                    .addComponent<RGBColorComponent>()
                    ;
            }
        }
    }
};
int usagi_main(const std::vector<std::string> &args)
{
    Game game;

    game.addGameInitSystem(std::make_unique<PixelInitSystem>())
        .addFrameUpdateSystem(std::make_unique<CosineWaveValueUpdateSystem>())
        .addFrameUpdateSystem(std::make_unique<ColorMappingSystem>())
        .addFrameUpdateSystem(std::make_unique<ConsolePixelDrawSystem>())
        ;

    game.mainLoop();

    return 0;
}
