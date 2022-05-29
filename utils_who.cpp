/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_who.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vspinell <vspinell@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/08 19:48:19 by vspinell          #+#    #+#             */
/*   Updated: 2022/03/09 19:18:47 by vspinell         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"
#include "server.hpp"
#include "client.hpp"

Client*	Server::cl_byname(std::string name)
{
	for (client_iterator begin = clients.begin(); begin != clients.end(); begin++)
	{
		if (begin->second.get_nick() == name)
			return &(begin->second);
	}
	return NULL;
}

Channel*	Client::get_last_channel(Client *cli)
{
	if (!cli->channels.empty())
	{
		client_ch_iterator ret = cli->get_vec_end();
		ret--;
		// std::cout << (*ret)->get_name() << std::endl;
		return (*ret);
	}
	return (NULL);
}

int	Server::find_is_Opbyname(std::string &name)
{
	Channel *ch;
	Client* cli;
	std::string ch_name;
	channel_iterator ret;
	cli = cl_byname(name);
	if (cli != NULL)
	{
		ch = cli->get_last_channel(cli);
		ch_name = (ch == NULL) ? "*" : ch->get_name();
		if (ch_name != "*")
		{
			ch = find_channel(ch_name);
			ret = find_client_in_channel(*ch, name);
			if (ret->second.is_Op())
				return (0);
		}
	}
	return (1);
}