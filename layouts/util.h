#pragma once
#include <ctime>
#include <raylib.h>
#include <string>

// Scales the screen item to center of the screen taking up the available space while maitaining aspect ratio
Rectangle scaleDynamic(Rectangle rect);
std::string timeToDB(time_t t);
std::string timeToHuman(time_t t);

