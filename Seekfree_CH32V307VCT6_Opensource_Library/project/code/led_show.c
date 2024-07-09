#include "led_show.h"
#include "straight_road.h"
void Led_show_all(void)
{

    if(straight_road_type == STRAIGHT_IN)
    {
        gpio_set_level(YELLOW, 1);
    }else{
        gpio_set_level(YELLOW, 0);
    }
//    switch (key) {
//        case value:
//
//            break;
//        default:
//            break;
//    }

}
