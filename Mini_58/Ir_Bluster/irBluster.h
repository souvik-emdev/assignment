#ifndef IRBLUSTER_H_INCLUDED
#define IRBLUSTER_H_INCLUDED
/* ^^ these are the include guards */

/* Prototypes for the functions */

void setAsGpio(void);
void setAsPwm(void);
void tx1();
void tx0();
void startbit();
void stopbit();
void transmit();

#endif