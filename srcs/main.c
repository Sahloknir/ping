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

void				handle_error(const char *error) {
	ft_putstr_fd(ft_strjoin("ERROR : ", error), 2);
	printf("errno : %d\n", errno);
	exit(-1);
}

void				print_request() {
	printf("PING %s (%s): %d data bytes\n", stats.host, stats.host_addr, PKT_SIZE);
}

void				print_reply(int recv, int seq, float rtt, int ttl) {
	printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", recv, stats.host_addr, seq, ttl, rtt);
}

void				set_rtt_stats() {
	unsigned int	i;
	float			rtt_sum;
	float			rtt_sum2;

	i = 0;
	stats.rtt_min = -1;
	stats.rtt_max = -1;
	rtt_sum = 0.0;
	while (i < stats.pkts_recv) {
		if (stats.rtt_min < 0 || stats.rtt[i] < stats.rtt_min)
			stats.rtt_min = stats.rtt[i];
		if (stats.rtt_max < 0 || stats.rtt[i] > stats.rtt_max)
			stats.rtt_max = stats.rtt[i];
		rtt_sum += stats.rtt[i];
		rtt_sum2 += stats.rtt[i] * stats.rtt[i];
		i++;
	}
	stats.rtt_avg = rtt_sum / i;
	i = 0;
	rtt_sum2 = 0.0;
	while (i < stats.pkts_recv) {
		rtt_sum2 += ft_fabs(stats.rtt_avg - stats.rtt[i]);
		i++;
	}
	stats.rtt_mdev = (float)(rtt_sum2 / i);
}

void				print_stats(int sig) {
	float		percent;

	percent = (float)(100.0 - (((float)stats.pkts_recv / (float)stats.pkts_sent) * 100.0));
	set_rtt_stats();
	printf("\n--- %s ping statistics ---\n", stats.host);
	printf("%d packets transmitted, %d packets received, %.1f%% packet loss\n", stats.pkts_sent, stats.pkts_recv, percent);
	printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", stats.rtt_min, stats.rtt_avg, stats.rtt_max, stats.rtt_mdev);
	sig = 0;
	exit(sig);
}

void				add_to_stats(float rtt) {
	unsigned int	i;
	float			*tmp;

	i = 0;
	tmp = ft_memalloc(sizeof(float) * stats.pkts_recv);
	while (stats.rtt && i < stats.pkts_recv - 1) {
		tmp[i] = stats.rtt[i];
		i++;
	}
	tmp[i] = rtt;
	if (stats.rtt)
		free(stats.rtt);
	stats.rtt = tmp;
}

struct icmp			*create_pkt(int seq, char *pkt_buf) {
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

int					send_pkt(int sockfd, struct addrinfo *rp, int seq) {
	int				sent;
	char			pkt_buf[4096];
	struct icmp		*pkt;

	pkt = create_pkt(seq, pkt_buf);
//	printf("sokfd : %d\n", cal_chksum((unsigned short*)rp->ai_addr, sizeof(struct sockaddr_in)));
	if ((sent = sendto(sockfd, pkt, PKT_SIZE, 0, rp->ai_addr, sizeof(struct sockaddr_in))) < 0)
		handle_error(ERROR_SEND);
	stats.pkts_sent++;
	return (sent);
}

int					recv_pkt(int sockfd, int seq, struct timeval time_sent) {
	int					recv;
	struct timeval		time_recv;
	char				recv_buf[64];
	struct sockaddr		rep_addr;
	int					rep_len;
	float				rtt;

	rep_len = sizeof(rep_addr);
	if ((recv = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, &rep_addr, (unsigned int *)&rep_len)) < 0) {
		handle_error(ERROR_RECV);
	} else {
		if (gettimeofday(&time_recv, NULL) < 0)
			handle_error(ERROR_TIME);
		rtt = (float)((float)(time_recv.tv_usec - time_sent.tv_usec)) / 1000.0;
		stats.pkts_recv++;
		add_to_stats(rtt);
		print_reply(recv, seq, rtt, 57);
	}
	return (recv);
}

void				resolve_host(struct addrinfo *rp) {
	char				*ret;
	struct sockaddr_in	*host_addr;

	host_addr = (struct sockaddr_in *)rp->ai_addr;
	if (!(ret = malloc(sizeof(char) * 128)))
		handle_error(ERROR_MALLOC);
	if (!(stats.host_addr = inet_ntop(AF_INET, &host_addr->sin_addr, ret, sizeof(char) * 128)))
		handle_error("inet_ntop\n");
}

struct addrinfo		*gethostinfo(char *host, int *sockfd) {
	struct addrinfo		hints;
	struct addrinfo		*results;
	struct addrinfo		*rp;
	int					sockopt;
	int					err;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_flags = 0;
	hints.ai_protocol = 1;
	if ((err = getaddrinfo(host, NULL, &hints, &results)) != 0) {
		handle_error(ERROR_ADDR_INFO);
	}
	for (rp = results; rp != NULL; rp = rp->ai_next) {
		*sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (*sockfd > 0) {
			break;
		}
		handle_error(ERROR_SOCKET);
	}
	freeaddrinfo(results);
	sockopt = 50 * 1024;
	if (setsockopt(*sockfd, SOL_SOCKET, SO_RCVBUF, &sockopt, sizeof(sockopt)) != 0)
		handle_error(ERROR_SOCK_OPT);
	return (rp);
}

int					ft_ping(char *host) {
	int					seq;
	int					sockfd;
	struct timeval		sent;
	struct addrinfo		*rp;

	rp = gethostinfo(host, &sockfd);
	resolve_host(rp);
	seq = 0;
	print_request();
	while (1) {
		signal(SIGINT, print_stats);
		if (gettimeofday(&sent, NULL) < 0)
			handle_error(ERROR_TIME);
		if (send_pkt(sockfd, rp, seq) < 0)
			return (-1);
		if (recv_pkt(sockfd, seq, sent) < 0)
			return (-1);
		seq++;
		sleep(1);
	}
	return (0);
}

int					main(int argc, char **argv) {
	if (argc != 2) {
		return (-1);
	}
	else {
		stats.host = argv[1];
		stats.pkts_sent = 0;
		stats.pkts_recv = 0;
		stats.rtt = NULL;
		ft_ping(argv[1]);
	}
	return (0);
}
