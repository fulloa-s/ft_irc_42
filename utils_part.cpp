/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_part.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fulloa-s <fulloa-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 15:06:25 by fulloa-s          #+#    #+#             */
/*   Updated: 2022/03/04 13:06:47 by fulloa-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"
#include "server.hpp"

void	Server::part_sendmsg(Client &client, std::vector<std::string>::iterator it, std::vector<std::string> &ve)
{
	std::string msg;
	Client *_client = &client;
		
	msg = client.client_id() + " PART #" + (*it) + "\r\n";
	send(client.get_fd(), msg.c_str(), msg.size(),0);
	msg = client.client_id() + " PART #" + (*it) + " ";
	msg += create_msg(ve, 2);
	clean_one_channel(_client, find_ch_ptr(_client, (*it)),msg);
}

Server::client_ch_iterator Server::find_ch_ptr(Client *client, std::string ch_name)
{
	client_ch_iterator ret;
    
	for(client_ch_iterator c_it = client->get_vec_begin(); c_it != client->get_vec_end(); c_it++)
	{
		if((*c_it)->get_name() == ch_name)
		{
			ret = c_it;
			break;
		}
	}
	return(ret);
}

std::string Server::create_msg(std::vector<std::string> &ve, int from)
{
	std::string msg;
	
	for(std::vector<std::string>::size_type it = from; it < ve.size(); it++)
	{
		msg += ve[it];
		if(it == ve.size() - 1)
			msg += "\r\n";
		else
			msg += " ";
	}
	if(!msg.empty() && msg[0] != ':')
		msg.insert(0,1,':');
	if(msg.empty())
		msg = ":No boundaries on the net!\r\n";
	return(msg);
}