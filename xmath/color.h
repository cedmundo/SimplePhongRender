#pragma once
#include "vec4.h"

typedef Vec4 Color;

// Static color
static const Color ColorBlack = {0.0f, 0.0f, 0.0f, 1.0f};
static const Color ColorRed = {1.0f, 0.0f, 0.0f, 1.0f};
static const Color ColorGreen = {0.0f, 1.0f, 0.0f, 1.0f};
static const Color ColorBlue = {0.0f, 0.0f, 1.0f, 1.0f};
static const Color ColorWhite = {1.0f, 1.0f, 1.0f, 1.0f};
static const Color ColorDarkGray = {0.1f, 0.1f, 0.1f, 1.0f};

// Return only RGB components of the given color
Vec3 ColorToRGB(Color color);
