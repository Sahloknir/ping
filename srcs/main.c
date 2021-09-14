/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axbal <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/10 13:28:51 by axbal             #+#    #+#             */
/*   Updated: 2021/09/14 16:19:10 by axbal            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

struct s_stats		stats;

/* penser a modifier le checksum */
unsigned short		cal_chksum(unsigned short *addr, int len) {
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
/* penser a modifier le checksum */

void			print_request() {
	printf("PING %s (%s): %d data bytes\n", stats.host, stats.host_addr, PKT_SIZE);
}

void			print_reply(int recv, int seq, float rtt) {
	int			ttl;

	ttl = 57;
	printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", recv, stats.host_addr, seq, ttl, rtt);
}

void			print_stats(int sig) {
	float		percent;
	float		tmp;

	tmp = 0.0;
	stats.rtt = &tmp;
	percent = (float)(100.0 - (((float)stats.pkts_recv / (float)stats.pkts_sent) * 100.0));
	printf("\n--- %s ping statistics ---\n", stats.host);
	printf("%d packets transmitted, %d packets received, %.1f%% packet loss\n", stats.pkts_sent, stats.pkts_recv, percent);
	printf("round-trip min/avg/max = %.3f/%.3f/%.3f ms\n", 0.0, 0.0, 0.0);
	sig = 0;
	exit(0);
}

struct icmp		*create_pkt(int seq, char *pkt_buf) {
	struct icmp			*pkt;

	pkt = (struct icmp*)pkt_buf;
	pkt->icmp_type = ICMP_ECHO;
	pkt->icmp_code = 0;
	pkt->icmp_cksum = 0;
	pkt->icmp_seq = seq;
	pkt->icmp_id = getpid();
	pkt->icmp_cksum = cal_chksum((unsigned short*)pkt, PKT_SIZE);
	return (pkt);
}

int				send_pkt(int sockfd, struct addrinfo *rp, int seq) {
	int				sent;
	char			pkt_buf[4096];
	struct icmp		*pkt;

	pkt = create_pkt(seq, pkt_buf);
	if ((sent = sendto(sockfd, pkt, PKT_SIZE, 0, rp->ai_addr, sizeof(struct sockaddr_in))) < 0)
		ft_putstr_fd("error : couldn't send packet\n", 2);
	return (sent);
}

int				recv_pkt(int sockfd, int seq, struct timeval time_sent) {
	int					recv;
	struct timeval		time_recv;
	char				recv_buf[64];
	struct sockaddr		rep_addr;
	int					rep_len;
	float				rtt;

	rep_len = sizeof(rep_addr);
	if ((recv = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, &rep_addr, (unsigned int *)&rep_len)) < 0) {
		ft_putstr_fd("error : recvfrom error\n", 2);
	} else {
		gettimeofday(&time_recv, NULL);
		rtt = (float)((float)(time_recv.tv_usec - time_sent.tv_usec)) / 1000.0;
		print_reply(recv, seq, rtt);
	}
	return (recv);
}

int				ft_ping(int sockfd, struct addrinfo *rp) {
	int				seq;
	struct timeval	sent;

	seq = 0;
	print_request();
	while (1) {
		signal(SIGINT, print_stats);
		gettimeofday(&sent, NULL);
		if (send_pkt(sockfd, rp, seq) < 0)
			return (-1);
		stats.pkts_sent++;
		if (recv_pkt(sockfd, seq, sent) < 0)
			return (-1);
		stats.pkts_recv++;
		seq++;
		sleep(1);
	}
	return (0);
}

void			resolve_host(struct addrinfo *rp) {
	char				*ret;
	struct sockaddr_in	*host_addr;

	host_addr = (struct sockaddr_in *)rp->ai_addr;
	ret = malloc(sizeof(char) * 128);
	stats.host_addr = inet_ntop(AF_INET, &host_addr->sin_addr, ret, sizeof(char) * 128);
}

int				gethostinfo(char *host) {
	struct addrinfo		hints;
	struct addrinfo		*results, *rp;
	int					sockfd;
	int					sockopt;

	ft_memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 1;

	getaddrinfo(host, NULL, &hints, &results);
	// gerer les erreurs des retours de getaddrinfo()

	for (rp = results; rp != NULL; rp = rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd > 0) {
			break;
		}
		// gerer les erreurs
	}
	freeaddrinfo(results);
	sockopt = 50 * 1024;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &sockopt, sizeof(sockopt));
	// gerer les erreurs

	resolve_host(rp);
	ft_ping(sockfd, rp);
	return (0);
}

int				main(int argc, char **argv) {
	if (argc != 2) {
		return (-1);
	}
	else {
		stats.host = argv[1];
		stats.pkts_sent = 0;
		stats.pkts_recv = 0;
		gethostinfo(argv[1]);
	}
	return (0);
}
