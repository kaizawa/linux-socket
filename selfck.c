/* 
 *  Copyright (C) 1998-2010 Kazuyoshi Aizawa. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 */ 
/*
 * Scan local tcp ports
 * selfck.c 
 * cc selfck.c -lnsl -o selfck
 */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <sys/utsname.h>
#include <stdlib.h>

#define MAXPORT 1024
#define MAXSVCNAME 15

int main()
{
    int net, portnum;
    struct hostent *host;
    struct servent *svc_ent;
    struct sockaddr_in sa;
    struct utsname  h_name[1];
    char   svc_name[MAXSVCNAME];

    memset(&sa, 0, sizeof(struct sockaddr_in));    
    if ( (uname(h_name)) < 0 ){
        perror("uname");
        exit(1);
    }
    if ((host = gethostbyname(h_name->nodename)) == NULL){
        perror("gethostbyname()");
        exit(1);
    }
    memcpy(&sa.sin_addr, host->h_addr, sizeof(struct in_addr));
    sa.sin_family = AF_INET;    
        
    for (portnum = 1; portnum <= MAXPORT ; portnum++) {
        sa.sin_port = htons(portnum);
        if((net = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            perror("socket");
            exit(1);
        }
        if (connect(net, (struct sockaddr *) &sa, sizeof(struct sockaddr_in)) < 0){
            printf("%s  %-5d %s\r", h_name->nodename, portnum, strerror(errno));
            fflush(stdout);
        } else {
            if ((svc_ent = getservbyport( htons(portnum) ,"tcp")) == NULL) {
                sprintf(svc_name,"Unknown");
            } else {
                snprintf(svc_name, MAXSVCNAME, "%s", svc_ent->s_name);                
            }
            printf("%s  %-5d < %-15s>   accepted.  \n",h_name->nodename,portnum, svc_name);
            if (shutdown(net, 2) < 0) {
                perror("shutdown()");
                exit(1);
            }
        }
        close(net);
    }
    printf("                                                   \r");
    exit(0);
}
