#include "Panel.h"

using namespace HUB75;

Panel::Panel()
{
}

Panel::Panel(const I2S::Config config)
{
    setConfig(config);
}

Panel::~Panel()
{
}

bool Panel::begin()
{
    if (initialized)
    {
        return true;
    }

    if (!config_initialized)
    {
        return false;
    }

    setShiftDriver(config.driver);

    if (!allocate())
    {
        return false;
    }

    resetBuffers();

    configureDMA();

    return initialized;
}
