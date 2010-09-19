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
 * Probe service name by port number  
 * cc getserv.c -lnsl -o getserv
 */ 
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
    struct servent *serv ;
    int portnum;

    if(argc != 2){
        printf("Usage: %s port\n", argv[0]);
        exit(1);
    }

    portnum = atoi(argv[1]);
    if ( ( serv = getservbyport(htons(portnum),"tcp")) == NULL ){
        perror("getservbyport:");
        exit(1);
    }
    printf( "port %d is %s\n", portnum , serv->s_name );
    exit(0);
}
