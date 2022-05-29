/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: francesco <francesco@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 15:05:15 by vspinell          #+#    #+#             */
/*   Updated: 2022/03/09 16:21:16 by francesco        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_H
#define HEADER_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <map>
#include <vector>
#include <stdio.h>
#include <cctype>

#define space " "
#define CRLF "/r/n"


int			ft_perror(std::string message);
std::string	ft_toupper(std::string str);
void		handle_signal(int sig);
void	    print(std::vector<std::string> ve);

#endif