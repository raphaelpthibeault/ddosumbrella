#ifndef DDOSUMBRELLA_UMBRELLA_H
#define DDOSUMBRELLA_UMBRELLA_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <assert.h>
#include <errno.h>

#include <inttypes.h>

#include <netlink/netlink.h>
#include <netlink/cache.h>
#include <netlink/route/link.h>

#define CTkbrx 1 // 00 01
#define CTkbtx 2 // 00 10
#define CTpktrx 4 // 01 00
#define CTpkttx 8 // 10 00
#define true 1
#define false 0
#define statsfree(x) nl_socket_free(x)

typedef unsigned char CType;
typedef unsigned char Bool;

struct s_counter {
    time_t ts;
    unsigned long int cnt;
    CType type:5;
    unsigned int samples;
};
typedef struct s_counter Counter;

struct s_counters {
    char interface[24];
    Counter kbrx;
    Counter kbtx;
    Counter pktrx;
    Counter pkttx;
    struct rtnl_link *link;
    struct nl_sock *sock;
};
typedef struct s_counters Counters;

Counters *statsinit(const char*);
Bool statsupdate(Counters*);

#endif //DDOSUMBRELLA_UMBRELLA_H
