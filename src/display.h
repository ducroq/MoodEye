#pragma once

#include <LovyanGFX.hpp>
#include "config.h"
#include "shared.h"

class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_GC9A01 _panel_instance;
    lgfx::Bus_SPI _bus_instance;

public:
    LGFX(void);
};

void displayTask(void * parameter);