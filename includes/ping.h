/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axbal <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/10 13:28:33 by axbal             #+#    #+#             */
/*   Updated: 2021/09/14 12:21:55 by axbal            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef PING_H
# define PING_H

# include "../libft/includes/libft.h"
# include <netdb.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/time.h>
# include <netinet/ip.h>
# include <netinet/in.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>
# include <errno.h>
# include <stdio.h>
# include <signal.h>

# define PKT_SIZE 56

struct				s_stats {
	char			*host;
	const char		*host_addr;
	unsigned int	pkts_sent;
	unsigned int	pkts_recv;
	float			*rtt;
	float			rtt_min;
	float			rtt_max;
	float			rtt_avg;
	float			rtt_mdev;
};

#endif
