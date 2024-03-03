/*
 * dual.h
 *
 *  Created on: 2023年7月17日
 *      Author: 唐纪元
 */

#ifndef DUAL_H_
#define DUAL_H_

typedef enum//  此枚举定义不允许用户修改
{
    REC_NO_MSG,
    REC_NO_POWER,
    REC_CHARGE_FINISH,
}DUAL_CAR_EVENT;


void dual_com_irq_handler(void);


int dual_com_init(void);
int dual_com_deinit(void);
int dual_com_read(void);
int dual_com_write(DUAL_CAR_EVENT event);

#endif /* DUAL_H_ */
