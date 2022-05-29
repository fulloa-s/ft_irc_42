/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_nick.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fulloa-s <fulloa-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 15:06:25 by fulloa-s          #+#    #+#             */
/*   Updated: 2022/03/07 12:41:06 by fulloa-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"
#include "server.hpp"

int	Server::existing_nick(Client &client, std::string new_nick)
{
	std::map<int, Client>::iterator end = clients.end();
	if (client.get_nick() != new_nick)
	{
		for (std::map<int, Client>::iterator start = clients.begin(); start != end; start++)
		{
			if (start->second.get_nick() == new_nick)
				return (send_error(client, 433, 1, new_nick.c_str()));
		}
	}
	else
		return (2);
	return (0);
}

