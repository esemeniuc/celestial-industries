#pragma once

#include "world.hpp"
#include "logger.hpp"

// Global
extern Logger logger; //use extern so we share the logger in global.cpp with all other cpp files
extern World world;