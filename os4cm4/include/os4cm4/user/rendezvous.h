#ifndef OS4CM4_USER_RENDEZVOUS_H
#define OS4CM4_USER_RENDEZVOUS_H

#define TID_KILLER (0x01000000)

extern int rendezvous1 (int tid, int v, int * r);
extern int rendezvous0 (int tid, int v);

#endif
