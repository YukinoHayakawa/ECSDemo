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
                std::cos(c_pos.x + t / 50) +
                std::cos(c_pos.y * 2 + t / 50)
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
        const auto output = GetStdHandle(STD_OUTPUT_HANDLE);
        ctx.execution.sequential<
            PositionComponent,
            RGBColorComponent
        >([&, this](auto &c_pos, auto &c_color) {
            SetConsoleCursorPosition(output, {
                static_cast<SHORT>(c_pos.x),
                static_cast<SHORT>(c_pos.y)
            });
            SetConsoleTextAttribute(output, c_color.r);
            printf("x");
        });
    }
};

class PixelInitSystem : public GameInitSystem
{
public:
    void execute(GameContext &ctx) override
    {
        for(auto i = 0; i < 30; ++i)
        {
            for(int j = 0; j < 100; ++j)
            {
                ctx.entity_manager.addEntity()
                    .addComponent<PositionComponent>(j, i)
                    .addComponent<FloatValueComponent>()
                    .addComponent<RGBColorComponent>()
                    ;
            }
        }
    }
};

class EraseUnwantedPixelsInitSystem : public GameInitSystem
{
public:
    void execute(GameContext &ctx) override
    {
        for(auto i = 5; i < 25; ++i)
        {
            for(int j = 20; j < 80; ++j)
            {
                ctx.entity_manager
                    .removeComponent<PositionComponent>(i * 100 + j);
            }
        }
    }
};

class AddSomePixelsBackInitSystem : public GameInitSystem
{
public:
    void execute(GameContext &ctx) override
    {
        for(auto i = 10; i < 20; ++i)
        {
            for(int j = 30; j < 70; ++j)
            {
                ctx.entity_manager
                    .addComponent<PositionComponent>(i * 100 + j, j, i);
            }
        }
    }
};

int usagi_main(const std::vector<std::string> &args)
{
    Game game;

    game.addGameInitSystem(std::make_unique<PixelInitSystem>())
        .addGameInitSystem(std::make_unique<EraseUnwantedPixelsInitSystem>())
        .addGameInitSystem(std::make_unique<AddSomePixelsBackInitSystem>())
        .addFrameUpdateSystem(std::make_unique<CosineWaveValueUpdateSystem>())
        .addFrameUpdateSystem(std::make_unique<ColorMappingSystem>())
        .addFrameUpdateSystem(std::make_unique<ConsolePixelDrawSystem>())
        ;

    game.mainLoop();

    return 0;
}
