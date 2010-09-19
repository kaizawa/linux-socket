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
 *  packet monitor program 
 *  pckmon2.c 
 *  % gcc pckmon2.c -lnsl -o pckmon2
 */

#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netpacket/packet.h>

void recv_pkt();
int rsock ;

/* 
 * Receiving  packet 
 */
void recv_pkt()
{
  int rsin_size, count;
  struct sockaddr_in rsin;
  struct in_addr insaddr,indaddr;
  fd_set fds;

  struct buf 
  {
    struct iphdr ip;
    struct tcphdr tcp;
    unsigned char blah[65535];
  } buf;

  rsin_size = sizeof(rsin);

  FD_ZERO(&fds);
  FD_SET(rsock, &fds);

  for ( count = 0 ;; count++){

    if( select(rsock + 1, &fds , NULL, NULL, NULL) < 0 ){
      perror("select");
      exit(0);
    }

    if ( FD_ISSET(rsock, &fds)){
      if(recvfrom(rsock, &buf, sizeof(buf), 0, (struct sockaddr *)&rsin, &rsin_size) < 0 ) {
	perror("recvfrom");
      }

      if ( buf.ip.protocol != IPPROTO_TCP)
	continue; 

      insaddr.s_addr = buf.ip.saddr;	
      indaddr.s_addr = buf.ip.daddr;	


      printf("Packet number : %d\n", count);
      printf("----IP Header--------------------\n");
      printf("version     : %u\n",buf.ip.version);
      printf("ihl         : %u\n",buf.ip.ihl);
      printf("tos         : %u\n",buf.ip.tos);
      printf("tot length  : %u\n",ntohs(buf.ip.tot_len));
      printf("id          : %u\n",ntohs(buf.ip.id));
      printf("frag_off    : %u\n",ntohs(buf.ip.frag_off) & 8191);
      printf("ttl         : %u\n",buf.ip.ttl);
      printf("protocol    : %u\n",buf.ip.protocol);
      printf("check       : 0x%x\n",ntohs(buf.ip.check));
      printf("saddr       : %s\n",inet_ntoa(insaddr));
      printf("daddr       : %s\n",inet_ntoa(indaddr));

      printf("----TCP Header-------------------\n");
      printf("source port : %u\n",ntohs(buf.tcp.source));
      printf("dest port   : %u\n",ntohs(buf.tcp.dest));
      printf("sequence    : %u\n",ntohl(buf.tcp.seq));
      printf("ack seq     : %u\n",ntohl(buf.tcp.ack_seq));
      printf("frags       :");
      buf.tcp.fin ? printf(" FIN") : 0 ;
      buf.tcp.syn ? printf(" SYN") : 0 ;
      buf.tcp.rst ? printf(" RST") : 0 ;
      buf.tcp.psh ? printf(" PSH") : 0 ;
      buf.tcp.ack ? printf(" ACK") : 0 ;
      buf.tcp.urg ? printf(" URG") : 0 ;
      printf("\n");
      printf("window      : %u\n",ntohs(buf.tcp.window));
      printf("check       : 0x%x\n",ntohs(buf.tcp.check));
      printf("urt_ptr     : %u\n\n\n",buf.tcp.urg_ptr);
    }
  }
}

main(int argc, char *argv[]) {

  struct ifreq ifr;
  struct packet_mreq mreq;        

  if(argc != 2) {
    printf("Usage: %s interface\n", argv[0]);
    exit(1);
  }

  if ((rsock = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_IP))) < 0 ){
    perror("socket");
    exit(0);
  }

  strcpy(ifr.ifr_name, argv[1]);

  if(ioctl(rsock, SIOCGIFINDEX, &ifr) < 0 ){            
    perror("ioctl SIOCGIFINDEX");
    exit(0);
  }

  mreq.mr_type = PACKET_MR_PROMISC;
  mreq.mr_ifindex = ifr.ifr_ifindex;
  mreq.mr_alen = 0;
  mreq.mr_address[0] ='\0';

  if( (setsockopt(rsock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, (void *)&mreq, sizeof(mreq))) < 0){
    perror("setsockopt");
    exit(0);
  }
        
  recv_pkt();
}
