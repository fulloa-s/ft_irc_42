/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vspinell <vspinell@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 13:06:05 by vspinell          #+#    #+#             */
/*   Updated: 2022/05/06 13:25:10 by vspinell         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

int	Server::commands_handler(Client &client, std::vector<std::string> &ve)
{
	std::string	levels[13] = {	
								"NICK",
								"PASS",
								"USER",
								"JOIN",
								"PRIVMSG",
								"QUIT",
								"PING",
								"PART",
								"MODE",
								"KICK",
								"NOTICE",
								"LIST",
								"WHO"
							};
	for (short i = 0; i < 13; i++)
	{
		if (ft_toupper(ve[0]) == levels[i])
		{
			if (!client.get_auth() && (levels[i] != "NICK" && levels[i] != "PASS" && levels[i] != "USER"))
				return (send_error(client, 451, 0));
			return (this->*(checker[i]))(client, ve, 0);
		}
		
	}
	if (client.get_auth())
		return	(send_error(client, 421, 1, ve[0].c_str()));
	return (0);
}

int	Server::list(Client &client, std::vector<std::string> &ve, int index)
{
	(void)index;
	std::string reply;
	s_channel_iterator s_it;
	char 		users[256];
	std::vector<std::string> vect;

	
	reply = ":myserv 321 " + client.get_nick() + " Channel :Users  Name\r\n";
	send(client.get_fd(), reply.c_str(), reply.size(), 0);
	if(ve.size() == 1)
	{
		for(s_channel_iterator it = channels.begin(); it != channels.end(); it++)
		{
			sprintf(users, "%lu", (it->second.map_size()));
			reply = ":myserv 322 " + client.get_nick() + " #" + it->first + " " + users + " :\r\n";
			send(client.get_fd(), reply.c_str(), reply.size(), 0);
		}
	}
	else
	{
		vect.push_back(ve[1]);
		vector_split(vect, ',');
		for(std::vector<std::string>::iterator it = vect.begin(); it != vect.end(); it++)
		{
			if((*it).find("#") != std::string::npos)
				(*it).erase(0,1);
			else
				continue;
			if(( s_it = channels.find(*it)) != channels.end())
			{
				sprintf(users, "%lu", (s_it->second.map_size()));
				reply = ":myserv 322 " + client.get_nick() + " #" + s_it->first + " " + users + " :\r\n";
				send(client.get_fd(), reply.c_str(), reply.size(), 0);
			}
		}
	}
	reply = ":myserv 323 " + client.get_nick() + " :End of /LIST\r\n";
	send(client.get_fd(), reply.c_str(), reply.size(), 0);
	return(0);
	
}

int	Server::kick(Client &client, std::vector<std::string> &ve, int index)
{
	(void)index;
	std::vector<std::string> ch_list;
	std::vector<std::string> client_list;
	std::string str = "#";
	Channel	*ch_ptr;

	if(ve.size() < 3)
		return(send_error(client, 461, 1, "KICK"));
	ch_list.push_back(ve[1]);
	vector_split(ch_list, ',');
	client_list.push_back(ve[2]);
	vector_split(client_list, ',');
	for(std::vector<std::string>::iterator it = ch_list.begin(); it != ch_list.end(); it++)
	{
		if((*it).at(0) == '#' || (*it).at(0) == '&')
		{
			(*it).erase(0,1);
			if(channel_exist(*it))
			{
				ch_ptr = find_channel(*it);
				if(is_channel_client(client, *ch_ptr))
				{
					if((*(find_client_in_channel(*ch_ptr, client.get_nick()))).second.is_Op() == 1)
						kick_clients(client, client_list, ch_ptr, ve[ve.size() - 1]);
					else
						send_error(client, 482, 1, (str + *it).c_str());
				}
				else
					send_error(client, 442, 1, (str + *it).c_str());
			}
			else
				send_error(client, 403, 1, (str + *it).c_str());	
		}
		else
			send_error(client, 403, 1, (*it).c_str());
		}
		return(0);
}

int	Server::part(Client &client, std::vector<std::string> &ve, int index)
{
	std::vector<std::string> ch_list;
	std::string str = "#";

	if(ve.size() == 1)
		return(send_error(client, 461, 1, "PART"));
	ch_list.push_back(ve[1]);
	vector_split(ch_list, ',');
	for(std::vector<std::string>::iterator it = ch_list.begin(); it != ch_list.end(); it++)
	{
		if((*it).at(0) == '#' || (*it).at(0) == '&')
			(*it).erase(0,1);
		else
		{
			send_error(client, 403, 1, (*it).c_str());
			continue;
		}
		index = 0;
		for(s_channel_iterator s_it = channels.begin(); s_it != channels.end(); s_it++)
		{
			if((*it) == s_it->first)
			{
				for(channel_iterator ch_it = s_it->second.get_map_begin(); ch_it != s_it->second.get_map_end(); ch_it++)
				{
					if(client.get_nick() == (*(ch_it->first)).get_nick())
					{
						index = 1;
						break;
					}
				}
				if(index != 1)
					index = 2;
			}
		}
		if(index == 1)
			part_sendmsg(client, it, ve);
		else if(!index)
			send_error(client, 403, 1, (str + *it).c_str());
		else if(index == 2)
			send_error(client, 442, 1, (str + *it).c_str());
	}
	return(0);
}

int	Server::join_channel(Client &client, std::vector<std::string> &ve, int index)
{
	(void)index;
	std::vector<std::string> ch;
	std::vector<std::string> key;
	int i;
	
	
	if(ve.size() == 1)
		return(send_error(client, 461, 1, "JOIN"));
	if(ve.size() == 2 && ve[1] == "0")
		return(join_zero(client));
	ch.push_back(ve[1]);
	vector_split(ch, ',');
	if(ve.size() > 2)
	{
		key.push_back(ve[2]);
		vector_split(key, ',');
	}
	
	for(std::vector<std::string>::iterator it = ch.begin(); it != ch.end(); it++)
	{
		if(((*it).at(0) == '#' || (*it).at(0) == '&'))
		{
			(*it).erase(0,1);
			i = 0;
			for(std::vector<std::string>::iterator k_it = key.begin(); key.size() > 0 && it != key.end(); k_it++)
			{
				check_channel(*it, client, *k_it);
				key.erase(k_it);
				i = 2;
				break;
			}
			if(!i)
				check_channel(*it, client);
		}
		else
			break;
		
	}
	return(0);	
}


void Server::send_message_single_channel(Client &client, Channel &ch, std::string reply)
{
	for(channel_iterator it = ch.get_map_begin(); it != ch.get_map_end(); it++)
	{
		if(it->first->get_nick() != client.get_nick())
			send(it->first->get_fd(), reply.c_str(), reply.size(), 0);
	}
	
}

void Server::send_message_all_channels(Client &client, std::string reply)
{
	for(client_ch_iterator it = client.get_vec_begin(); it != client.get_vec_end(); it++)
		send_message_single_channel(client, *(*it), reply);
}



int	Server::check_nick(Client &client, std::vector<std::string> &ve, int index )
{
	std::string	reply;
	if (ve.size() == 1)
		return (send_error(client, 431, 0));
	if (ve[index + 1].size() > 20)
		ve[index + 1] = ve[index + 1].substr(0, 20);
	else if (existing_nick(client, ve[index + 1]))
		return(1);
	else
	{
		reply = client.client_id() + " NICK :" + ve[index + 1] + "\r\n"; //viene inviato anche se il nick non è mai stato settato
		client.set_nick(ve[index + 1]);
		send(client.get_fd(), reply.c_str(), reply.size(), 0);
		send_message_all_channels(client, reply);
	}
	return (0);
}

int	Server::check_pass( Client &client, std::vector<std::string> &ve, int index )
{
	if (client.get_auth() == 1)
		return (send_error(client, 462, 1, client.get_nick().c_str()));
	else if (ve.size() == 1 || (ve[index + 1].compare(1, this->_password.size(), 
			this->_password) != 0 && this->_password.size() != ve[index + 1].size()))
		return (send_error(client, 464, 1, client.get_nick().c_str()));
	else
		client.set_pass();
	return (0);
}

int	Server::check_user( Client &client, std::vector<std::string> &ve, int index)
{
	if(ve.size() < 2)
		return(send_error(client, 461, 1, "USER"));
	if (!client.get_user().empty())
		return (send_error(client, 462, 1, client.get_nick().c_str()));
	client.set_user(ve[index + 1]);
	return (0);
}

int		Server::quit(Client &client, std::vector<std::string> &ve, int index )
{
	(void)index;
	Client *_client = &client;
	std::string		msg;
	channel_iterator tmp;
						
	msg = client.client_id() + " QUIT :Quit ";
	if(ve.size() > 1)
		msg += create_msg(ve, 1);
	else
	 	msg += "\r\n";
	send(client.get_fd(), msg.c_str(), msg.size(), 0);
	clean_all_channels(_client, msg);
	close(client.get_fd());
	FD_CLR(client.get_fd(), &master);
	clients.erase(clients.find(client.get_fd()));
    return (0);
}

int	Server::ping(Client &client, std::vector<std::string> &ve, int index )
{
	if (ve.size() == 1)
		return (send_error(client, 409, 1, client.get_nick().c_str()));
	std::string msg = ":myserv PONG " + ve[index + 1] + "\r\n";
	send(client.get_fd(), msg.c_str(), msg.size(), 0);
	return(0);
}

int	Server::privmsg(Client &client, std::vector<std::string> &ve, int index)
{
	if (ve.size() > 1) //se ci sono parametri
	{
		if (ve.size() == 2)
			return send_error(client, 412, 0);
		if (ve[index + 1].find('#') != std::string::npos) //PRIVMSG to channel
		{
			ve[index + 1].erase(0, 1); //remove '#'
			for (s_channel_iterator it = channels.begin(); it != channels.end(); it++)
			{
				if (ve[index + 1] == it->second.get_name()) //trova canale
				{
					for (channel_iterator ch_it = it->second.get_map_begin(); ch_it != it->second.get_map_end(); ch_it++)
					{
						std::string ret = client.client_id() + " " + ve[index] + " #" + ve[1] + " "  + ve[2] + "\r\n";
						if ((*(ch_it->first)).get_fd() != client.get_fd()) //check per non mandare a se stesso
							send((*(ch_it->first)).get_fd(), ret.c_str(), ret.size(), 0);
					}
					return 0;
				}
			}
			return (send_error(client, 403, 1, ve[index + 1].c_str()));
		}
		for (client_iterator it = clients.begin(); it != clients.end(); it++) //PRIVMSG to client
		{
			if (ve[1] == it->second.get_nick() && !ve[2].empty())
			{
				std::string ret = client.client_id() + " " + ve[0] + " " + ve[1] +  " " + ve[ve.size() - 1] + "\r\n";
				return send(it->second.get_fd(), ret.c_str(), ret.size(), 0);
			}
		}
		if (ve[index] == "PRIVMSG")
			return (send_error(client, 401, 1, ve[index + 1].c_str()));
	}
	return send_error(client, 411, 1, ve[index].c_str());
}

int Server::notice(Client &client, std::vector<std::string> &ve, int index)
{
	if (ve.size() == 1)
		return send_error(client, 411, 1, ve[index].c_str());
	if (ve.size() > 2 && !ve[2].empty())
	{
		if (ve[1].find('#') != std::string::npos)
			return 0;
		return privmsg(client, ve, index);
	}
	return send_error(client, 412, 0);
}


int	Server::mode(Client &client, std::vector<std::string> &ve, int index)
{
	(void)index;
	size_t						pos = 0;
	Channel						*Ch;
	channel_iterator			applicant;
	std::vector<std::string>	ret;
	std::string					modes;
	if (ve.size() < 2)
		return (send_error(client, 461, 1, "MODE"));
	if (ve.size() == 2)
	{
		if (ve[1][0] == '#')
		{
			ve[1].erase(0, 1);
			if ((Ch = find_channel(ve[1])) == NULL)
				return (send_error(client, 403, 1, ve[1].c_str()));
			modes = Ch->gt_ch_modes();
			if (!Ch->get_key().empty())
				modes.insert(1, "k");
			send_error(client, 324, 2, Ch->get_name().c_str(), modes.c_str());
			return (0);
		}
		return (send_error(client, 403, 1, ve[1].c_str()));
	}
	//da qui in poi si ragiona con argomenti >=3 
	else if (ve[1][0] == '#') //viene utilizzato per cambiare il canale
	{
		if (!parse_mode(client, ve))
		{
			if (ve.size() == 2 || (ve.size() == 3 && ve[2] == "b"))
				return (0);
			Ch = find_channel(ve[1]);
			if ((applicant = find_client_in_channel(*Ch, client.get_nick())) == Ch->get_map_end())
				return (send_error(client, 442, 1, ve[1].c_str()));
			std::vector<std::string> names;
			if (applicant->second.is_Op())
			{
				for (size_t i = 0; i < (ve.size() - 2) / 2; i++)
				{
					pos += 2;
					if (!change_permission(ve, pos, Ch))
					{
						modes += ve[pos];
						names.push_back(ve[pos+1]);
					}
					else
						send_error(client, 441, 2, ve[pos + 1].c_str(), ve[1].c_str());
				}
				send_message(":" + client.get_nick() + "myserv MODE " + "#" +ve[1] + space, modes, names, Ch);
			}
			else
				return (send_error(client, 482, 1, ve[1].c_str()));
		}
		else
			return (1);	
	}
	else if (fd_byname(ve[1]) > 0)// viene utilizzato per il mode generale
		return (send_error(client, 502, 0));
	else
		return(send_error(client, 403, 1, ve[1].c_str()));
	return (0);
}

int Server::who(Client &client, std::vector<std::string> &ve, int index)
{
	(void)index;
	Channel *Ch;
	Client *cli;
	std::string	cli_name;
	std::string	admin;
	std::string hop;
	if (ve.size() == 1)
	{
		if ((Ch = client.get_last_channel(&client)) != NULL)
		{
			for (channel_iterator begin = Ch->get_map_begin(); begin != Ch->get_map_end(); begin++)
			{
				admin = begin->second.is_Op() ? "@" : "";
				cli_name =  begin->first->get_nick();
				hop = client.get_nick() == begin->first->get_nick() ? "0" : "3";
				cli =  cl_byname(cli_name);
				send_error(client, 352, 7, ("#" + Ch->get_name()).c_str(), begin->first->get_user().c_str(), inet_ntoa(client.get_info().sin_addr), "myserv", cli_name.c_str(), admin.c_str(), hop.c_str());
			}
			return (send_error(client, 315, 1, (client.get_nick() + space + Ch->get_name()).c_str()));
			}
		return (0);
	}
	else if (ve.size() == 3 && ve[2] == "b")
		send_error(client, 368, 1, ve[1].c_str());
	else if (ve[1][0] == '#')
	{
		ve[1].erase(0, 1);
		if ((Ch = find_channel(ve[1])) == NULL)
		{
			send_error(client, 315, 1, (client.get_nick() + space + "#" + ve[1]).c_str());
			return (0);
		}
		for (channel_iterator begin = Ch->get_map_begin(); begin != Ch->get_map_end(); begin++)
		{
			admin = begin->second.is_Op() ? "@" : "";
			cli_name =  begin->first->get_nick();
			hop = client.get_nick() == begin->first->get_nick() ? "0" : "3";
			cli =  cl_byname(cli_name);
			send_error(client, 352, 7, ("#" + ve[1]).c_str(), begin->first->get_user().c_str(), inet_ntoa(client.get_info().sin_addr), "myserv", cli_name.c_str(), admin.c_str(), hop.c_str());
		}
		return (send_error(client, 315, 1, (client.get_nick() + space + ve[1]).c_str()));
	}
	else
	{
		if (fd_byname(ve[1]))
		{
			admin = !find_is_Opbyname(ve[1]) ? "@" : "";
			cli = cl_byname(ve[1]);
			Ch = cli->get_last_channel(cli);
			std::string ch_name =  Ch != NULL ? Ch->get_name() : "*";
			send_error(client, 352, 7, ch_name.c_str(), client.get_user().c_str(), inet_ntoa(client.get_info().sin_addr), "myserv", ve[1].c_str(), admin.c_str(), "3");
			send_error(client, 315, 1, (client.get_nick() + space + ve[1]).c_str());
			return (0);
		}
		else
			return (send_error(client, 315, 1, (client.get_nick() + space + ve[1]).c_str()));
	}
	return (0);
}
