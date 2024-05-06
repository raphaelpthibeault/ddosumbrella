#include "umbrella.h"

Counters *statsinit(const char *interface) {
    Counters *c;
    int e;

    c = (Counters*)malloc(sizeof(struct s_counters));
    memset(c, 0, sizeof(struct s_counters));
    assert(c);

    c->sock = nl_socket_alloc();
    assert(c->sock);

    e = nl_connect(c->sock, NETLINK_ROUTE);
    if (e)
        assert_perror(errno);

    strncpy(c->interface, interface, 23);
    c->kbrx.type = CTkbrx;
    c->kbtx.type = CTkbtx;
    c->pktrx.type = CTpktrx;
    c->pkttx.type = CTpkttx;

    return c;
}

Bool statsupdate(Counters *c) {
    int e;
    unsigned long int pktrx, pkttx, kbrx, kbtx;

    e = rtnl_link_get_kernel(c->sock, 0, c->interface, &c->link);
    if (e) {
        fprintf(stderr, "rtnl_link_get_kernel()\n");
        return false;
    }

    pktrx = rtnl_link_get_stat(c->link, RTNL_LINK_RX_PACKETS);
    pkttx = rtnl_link_get_stat(c->link, RTNL_LINK_TX_PACKETS);
    kbrx = rtnl_link_get_stat(c->link, RTNL_LINK_RX_BYTES);
    kbtx = rtnl_link_get_stat(c->link, RTNL_LINK_TX_BYTES);

    if (!pktrx || !pkttx || !kbrx || !kbtx) {
        fprintf(stderr, "%lu, %lu, %lu, %lu\n", pktrx, pkttx, kbrx, pkttx);
        return false;
    }

    c->pktrx.cnt = pktrx;
    c->pkttx.cnt = pkttx;
    c->kbrx.cnt = kbrx;
    c->kbtx.cnt = kbtx;

    rtnl_link_put(c->link);

    return true;
}

/*
 * network byte order to host byte order
 * */
char *fetch_ip(struct sockaddr * s) {
    unsigned int ip, *ipp;
    unsigned char a, b, c, d;
    char *p, *pp;
    int x;

    p = malloc(8);
    for (pp = p, x = 0; x < 8; ++p, ++x) {
        *p = 0;
    }
    p = pp;

    ipp = (unsigned int *)s;
    ++ipp;
    ip = *ipp;

    /*
        get the values of a,b,c,d

        means 04.06.0c.0a &
              FF.00.00.00 =
              04.00.00.00
              >> 24
              = 04
        similarly,
              04.06.0c.0a &
              00.FF.00.00 =
              00.06.00.00
              >> 16
              = 06
        ...
     */

    a = (ip & 0xFF000000) >> 24;
    b = (ip & 0x00FF0000) >> 16;
    c = (ip & 0x0000FF00) >> 8;
    d = (ip & 0x000000FF);

    // and reverse
    snprintf(p, 15, "%d.%d.%d.%d",
             d, c, b, a);

    return p;
}

int main(int argc, char **argv) {
    Counters *c;
    char *ip, *interface, *_ip;
    struct ifaddrs *x, *xs;
    Bool b;
    int n;
    if (argc < 2) {
        fprintf(stderr, "Usage: %s IPADDRESS\n", *argv);
        return -1;
    }

    ip = argv[1];
    n = getifaddrs(&xs);
    for (x = xs, interface = 0; x != NULL; x = x->ifa_next) {
        _ip = fetch_ip(x->ifa_addr);
        if (_ip && !strcmp(ip, _ip)) {
            interface = x->ifa_name;
            free(_ip);
            break;
        }
    }

    if (x) {
        printf("%s\n", interface);
    } else {
        fprintf(stderr, "Unable to find IP address.\n");
        return -1;
    }

    c = statsinit(interface);
    b = statsupdate(c);

    if (b)
        printf("%lu\n", c->pktrx.cnt);
    else {
        printf("update failed\n");
        return -1;
    }

    statsfree(c->sock);
    free(c);
    return 0;
}
