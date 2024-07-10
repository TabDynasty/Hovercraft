#include "led_show.h"
#include "straight_road.h"
#include "obstacle.h"
#include "circle.h"
#include "cross.h"
void Led_show_all(void)
{

    if(straight_road_type == STRAIGHT_IN)
    {
        gpio_set_level(YELLOW, 1);
    }else{
        gpio_set_level(YELLOW, 0);
    }
    if(circle_type!=CIRCLE_NONE)
    {
        gpio_set_level(BlUE, 1);
    }else{
        gpio_set_level(BlUE, 0);
    }
    if(cross_type!=CROSS_NONE)
    {
        gpio_set_level(GREEN, 1);
    }else{
        gpio_set_level(GREEN, 0);
    }
    if(obstacle_type!=OBSTACLE_NONE)
    {
        gpio_set_level(RED, 1);
    }else{
        gpio_set_level(RED, 0);
    }
//    switch (key) {
//        case value:
//
//            break;
//        default:
//            break;
//    }

}
