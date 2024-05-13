#include <iostream>
#include <iomanip>
#include <util.h>

Rectangle scaleDynamic(Rectangle rect){
    float factor = 
        (float)GetScreenWidth()/SIZE_X <= (float)GetScreenHeight()/SIZE_Y ? 
        (float)GetScreenWidth()/SIZE_X :
        (float)GetScreenHeight()/SIZE_Y;

    rect.width = rect.width * factor;
    rect.height = rect.height * factor;
    rect.x =  GetScreenWidth()/2.0 - SIZE_X/2.0 * factor + rect.x * factor;
    rect.y =  GetScreenHeight()/2.0 - SIZE_Y/2.0 * factor + rect.y * factor;

    return rect;
}

std::string timeToDB(time_t t){
    char fmt[] ="%a %b %d %T %Y"; 
    std::tm  timeM{};
    std::stringstream is(ctime(&t));
    is >> std::get_time(&timeM, fmt);
    std::stringstream os;
    os << std::put_time(&timeM, "%F");
    return  os.str();

}
std::string timeToHuman(time_t t){
    char fmt[] ="%a %b %d %T %Y"; 
    std::tm  timeM{};
    std::stringstream is(ctime(&t));
    is >> std::get_time(&timeM, fmt);
    std::stringstream os;
    os << std::put_time(&timeM, "%a %b %d %Y");
    return  os.str();

}
