#ifndef CANFD_H
#define CANFD_H

#ifdef __cplusplus
extern "C" {
#endif

void canfd_init(void);
void canfd_sendframe(void);
void canfd_receiveframe(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif