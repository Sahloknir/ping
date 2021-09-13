/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axbal <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/10 13:28:33 by axbal             #+#    #+#             */
/*   Updated: 2021/09/13 16:16:27 by axbal            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef PING_H
# define PING_H

# include "../libft/includes/libft.h"
# include <netdb.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/cdefs.h>
# include <netinet/ip.h>
# include <netinet/in.h>
# include <netinet/ip_icmp.h>
# include <string.h>
# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <ctype.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <errno.h>
# include <signal.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/in_systm.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>

# define PKT_SIZE 56

#endif
