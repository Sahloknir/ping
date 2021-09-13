/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axbal <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/10 13:28:51 by axbal             #+#    #+#             */
/*   Updated: 2021/09/12 18:00:47 by axbal            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/cdefs.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

unsigned short cal_chksum(unsigned short *addr, int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short *w = addr;
    unsigned short answer = 0;
    while (nleft > 1)
    {
        sum +=  *w++;
        nleft -= 2;
    }
    if (nleft == 1)
    {
        *(unsigned char*)(&answer) = *(unsigned char*)w;
        sum += answer;
    }
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}

int		gethostinfo(char *host) {
	struct addrinfo		hints;
	struct addrinfo		*results, *rp;
	int					pid;
	int					r;
	int					sockfd;
	int					opt;
	int					sockopt;
	int					sent;
	int					recv;
	char				pkt_buf[4096];
	char				recv_buf[64];
	struct sockaddr		sender;
	int					sender_len;
	struct icmp			*pkt;

	pid = getpid();
	ft_memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 1;

	r = getaddrinfo(host, NULL, &hints, &results);
	ft_putnbr(r);
	ft_putstr("\n^ addrinfo ^\n");
	// gerer les erreurs des retours de getaddrinfo()

	pkt = (struct icmp*)pkt_buf;
	pkt->icmp_type = ICMP_ECHO;
	pkt->icmp_code = 0;
	pkt->icmp_cksum = 0;
	pkt->icmp_seq = 0;
	pkt->icmp_id = pid;
	pkt->icmp_cksum = cal_chksum((unsigned short*)pkt, 56);
	sender_len = sizeof(sender);
	for (rp = results; rp != NULL; rp = rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		ft_putnbr(sockfd);
		ft_putstr("\n^ sockfd ^\n");
		if (sockfd > 0) {
			break;
		}
		// gerer les erreurs
	}
	sockopt = 50 * 1024;
	opt = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &sockopt, sizeof(sockopt));
	ft_putnbr(opt);
	ft_putstr("\n ^ socket option ^\n");
	while (1) {
		sent = sendto(sockfd, pkt, 56, 0, rp->ai_addr, sizeof(rp->ai_addr));
		printf("sent : %d\n", sent);
		recv = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, &sender, (unsigned int *)&sender_len);
		printf("recieved : %d\n", recv);
		sleep(1);
	}
	freeaddrinfo(results);
	return (0);
}

int		main(int argc, char **argv) {
	if (argc != 2) {
		return (-1);
	}
	else {
		//setuid(getuid());
		gethostinfo(argv[1]);
	}
	return (0);
}
