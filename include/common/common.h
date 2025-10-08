#ifndef COMMON_H
#define COMMON_H

#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif // TRUE

#ifndef round
#define round(X) (((X) >= 0) ? (int)((X)+0.5) : (int)((X)-0.5))
#endif // round

#ifndef MAXPATHLENGTH
#define MAXPATHLENGTH 256
#endif // MAXPATHLENGTH

#endif // COMMON_H