#ifndef KEY_H_
#define KEY_H_

#define KEY_UP_PORT     E4
#define KEY_DOWN_PORT   E2
#define KEY_LEFT_PORT   E5
#define KEY_RIGHT_PORT  E3
#define KEY_MID_PORT    E6
//#define KEY_GND_PORT

#define KEY_UP          1
#define KEY_DOWN        2
#define KEY_LEFT        3
#define KEY_RIGHT       4
#define KEY_MID         5



void Key_Init(void);
int Key_Scan(void);
int Key_Scan4(void);
void Key_test(void);
_Bool Key_Scan_Simple(void);

#endif /* KEY_H_ */
