/*
 * circle.h
 *
 *  Created on: 2023年5月30日
 *      Author: 唐纪元
 */

#ifndef CIRCLE_H_
#define CIRCLE_H_

enum circle_type_e {
    CIRCLE_NONE = 0,
    CIRCLE_LEFT_BEGIN, CIRCLE_RIGHT_BEGIN,
    CIRCLE_LEFT_IN, CIRCLE_RIGHT_IN,
    CIRCLE_LEFT_RUNNING, CIRCLE_RIGHT_RUNNING,
    CIRCLE_LEFT_OUT, CIRCLE_RIGHT_OUT,
    CIRCLE_LEFT_END, CIRCLE_RIGHT_END,
};
void check_circle();
void run_Lcircle();
void run_Rcircle();
extern enum circle_type_e circle_type;
#endif /* CIRCLE_H_ */
