/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fulloa-s <fulloa-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 14:43:17 by vspinell          #+#    #+#             */
/*   Updated: 2022/03/08 16:07:54 by fulloa-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"
#include "server.hpp"
#include <signal.h>

Server *server; // per pulire gli fd se ctrl C 

void	handle_signal(int sig)
{
	(void)sig;
	int i = server->getfdmax();
	while(i-- > server->getfd())
	{
		if (FD_ISSET(i, &server->getfdmaster()))
		{
			FD_CLR(i, &server->getfdmaster());
			close(i);
		}
	}
	FD_CLR(server->getfd(), &server->getfdmaster());
}

int main(int ac, char **av)
{
	// PORT RANGE  1024 a 65535
	if (ac == 3)
	{
		try
		{
			server = new Server(av[1], av[2]);
			server->setup();
			server->start();
		}
		catch(int e)
		{
			delete server;
			return e;
		}
		delete server;
	}
	else
		return ft_perror("Error: wrong number of arguments");
}