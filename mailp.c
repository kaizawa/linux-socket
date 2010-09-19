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
 * POP3 client program
 * mailp.c
 * cc mailp.c -lnsl -o mailp
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <ctype.h>

#define  BUFMAX  256
#define  PORT_NO 110

void read_msg();
void print_err(char *);

extern int h_errno;
int net;
int nbyte;
char r_msg[BUFMAX];
char s_msg[BUFMAX];
char input[6];

int
main(int argc, char *argv[])
{
    struct hostent *host;
    struct servent *serv;
    struct sockaddr_in sa;
    char p_list[] = "LIST\n";
    char p_user[] = "USER pop_user\n";
    char p_pass[] = "PASS pop_password\n";
    char p_quit[] = "QUIT\n";

    if (argc != 2) {
        printf("Usage: %s hostname\n", argv[0]);
        exit(1);
    }

    /*
     * Check Host name
     */
    if ((host = gethostbyname(argv[1])) == NULL){
        herror("gethostbyname()");
        close(net);
        exit(1);        
    }

    memset(&sa,0, sizeof(struct sockaddr_in));
    memcpy(&sa.sin_addr, host->h_addr, sizeof(struct in_addr));

    sa.sin_family = AF_INET;
    sa.sin_port = htons(PORT_NO);

    /*
     * Make socket
     */
    if((net = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        print_err("socket()");

    /*
     * Connect to the mail server
     */
    if (connect(net, (struct sockaddr *)&sa, sizeof(struct sockaddr)) < 0)
        print_err("connect()");

    /*
     * Receive connection messages from server
     */
    memset(r_msg,0,BUFMAX);
    if( recv(net,r_msg,BUFMAX,0) < 0)
        print_err("recv()");
    printf("%s",r_msg);

    /*
     * Send user name
     */
    nbyte = strlen( p_user );
    if( send(net,p_user,nbyte,0) < 0)
        print_err("send()");

    memset(r_msg,0,BUFMAX);
    if( recv(net,r_msg,BUFMAX,0) < 0)
        print_err("recv");
    printf("%s",r_msg);

    /*
     * Send password
     */
    nbyte = strlen(p_pass);
    if( send(net,p_pass,nbyte,0) < 0 )
        print_err("send()");

    memset(r_msg,0,BUFMAX);
    if( recv(net,r_msg,BUFMAX,0) < 0)
        print_err("recv()");
    printf("%s",r_msg);

    /*
     * Request mail list & Select letter number
     */
    while(1){  /* loop start */ 
        nbyte = strlen( p_list );
        if( send(net,p_list,nbyte,0) < 0 )
            print_err("send()");
        while(1){
            memset(r_msg,0,BUFMAX);
            if( recv(net,r_msg,BUFMAX,0) < 0)
                print_err("recv()");
            printf("%s",r_msg);
            if ( strstr( r_msg, "\n.") !=  NULL) {
                printf("\n");
                break;
            } 
        }

        /*
         * Enter letter number
         */
        printf("Enter the number you want to read( q for quit)\n");
        if ( scanf("%5s", input) < 0)
            print_err("scanf()");
        
        if ( input[0] == 'q' ) {
            nbyte = strlen( p_quit );
            if( send(net,p_quit,nbyte,0) < 0)
                print_err("send()");
            memset(r_msg,0,BUFMAX);                    
            if( recv(net,r_msg,BUFMAX,0) < 0)
                print_err("recv()");
            printf("%s\n",r_msg);
            /* Normal closure process */
            if (shutdown(net, 2) < 0)
                print_err("shutdown()");
            close(net);
            exit(0);
        } 
        if( (isdigit(input[0])) == 0){
            printf("Input error\n");
            continue;
        }
        read_msg();
    } /* loop end */
}

/*********************************************************
 *  read_msg()
 *
 *  Retrieve and print email message from the mail server
 *********************************************************/
void 
read_msg()
{
    sprintf(s_msg,"RETR %s\n",input);
    nbyte = strlen(s_msg);
    if( send(net,s_msg,nbyte,0) < 0 )
        print_err("send()");
    while(1){
        memset(r_msg,0,BUFMAX);        
        if( recv(net,r_msg,BUFMAX,0) < 0)
            print_err("recv()");
        printf("%s",r_msg);
        if( strstr(r_msg,"\n.") != NULL ){
            printf("\n");
            break;
        }
    }
    return;
}

void
print_err(char *func)
{
    perror(func);
    close(net);
    exit(1);
}
