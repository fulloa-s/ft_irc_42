/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_mode.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vspinell <vspinell@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 15:45:44 by vspinell          #+#    #+#             */
/*   Updated: 2022/05/06 13:21:51 by vspinell         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void	print(std::vector<std::string> ve)
{
	std::vector<std::string>::iterator begin = ve.begin();
	std::vector<std::string>::iterator end = ve.end();
	for (; begin != end; begin++)
		std::cout << *begin << std::endl;
}

int	find_incharset(std::string charset, char c)
{
	if (charset.find(c) == std::string::npos)
		return(1);
	return (0);
}

void	clear_flags(std::vector<std::string> &ve, int index)
{
	while (!find_incharset("+-", ve[index][0]) && !find_incharset("+-", ve[index][1]))
		ve[index].erase(0, 1);
	if (ve[index].size() > 2)
		ve[index].erase(2);
}

std::vector<std::string>	join_string(std::vector<std::string> ve, std::string charset)
{
	std::string ret;
	std::vector<std::string>::iterator begin;
	for (size_t i = 0; i < ve.size(); i++)
	{
		size_t y = -1;
		while (++y < ve[i].size())
		{
			if (find_incharset(charset, ve[i].at(y)))
			{
				y = 0;
				break;
			}
		}
		if (y > 0)
		{
			begin = ve.begin();
			ret.insert(0, 1, ve[i].at(y -1));
			ret.append(ve[i+1]);
			while (*begin != ve[i])
				begin++;
			ve.insert(begin, ret);
			begin = ve.begin();
			while (*begin != ve[i + 1])
				begin++;
			ve.erase(begin);
			ve.erase(begin);
		}
	}
	return ve;
}

int	Server::fd_byname(std::string &name)
{
	client_iterator end = clients.end();
	for (client_iterator begin = clients.begin(); begin != end; begin++)
	{
		if (begin->second.get_nick() == name)
			return (begin->second.get_fd());
	}
	return (0);
}

int	Server::mode_arg(std::string flags, Client &client, std::string ch_name)
{
	size_t i = 0;
	while (i < flags.size())
	{
		if (find_incharset("o-+", flags[i]))
		{
			std::string invalid_char(1, flags[i]);
				return (send_error(client, 472, 2, invalid_char.c_str(), ch_name.c_str()));
		}
		i++;
	}
	if (find_incharset(flags, 'o'))
		return (1);
	return (0);
	
}

int	Server::dest_nick(std::string nick, Client &client)
{
	if (!fd_byname(nick))
	{
		return (send_error(client, 401, 1, nick.c_str()));
	}
	return (0);
}


int	Server::parse_mode(Client &client, std::vector<std::string> &ve)
{
	s_channel_iterator begin = channels.begin();
	s_channel_iterator end = channels.end();
	ve = join_string(ve, "+-");
	std::vector<std::string>::iterator it;
	ve[1].erase(0, 1);
	while (begin != end)
	{
		if (begin->second.get_name() == ve[1]) //se il nome del canale esiste
		{
			//mettere il controllo che se il terzo elemento é b e la size é 3 va bene e ritorna
			if ((ve.size() == 3 && ve[2] == "b") || ve.size() == 2)
				return (0);
			for (size_t i = 2; i < ve.size(); i++)
			{
				bool arg2 = mode_arg(ve[i], client, ve[1]);
				//nel caso ci sia una flag come ultimo elemento forzo la rimozione impostando arg3 == 1
				bool arg3 = (((i + 1) < ve.size()) ? 0 : 1);
				if (i + 1 < ve.size())
					arg3 = dest_nick(ve[i + 1], client);
				if (arg2 || arg3)
				{
					it = ve.begin();
					ve.erase(it + i);
					if (i + 1 <= ve.size())
						ve.erase(it + i);
					i = 1; //perché il ciclo for incrementa di 1 alla fine
				}
				else
					i++;
			}
			break;
		}
		begin++;
	}
	if (begin == end)
		return (send_error(client, 403, 1, ve[1].c_str()));
	else if (ve.size() == 2)
		return (1);
	return (0);
}

int	Server::change_permission(std::vector<std::string> &ve, size_t pos, Channel *Ch)
{
	std::string					ch_name = ve[1];
	channel_iterator			dest;	
	ch_name.erase(0, 1);
	clear_flags(ve, pos);
	dest = find_client_in_channel(*Ch, ve[pos + 1]);
	if (dest == Ch->get_map_end())
		return (1);
	if (ve[pos][0] == '-')
	{
		if (dest->second.is_Op())
			dest->second.set_Op('-');
	}
	else
	{
		if (!dest->second.is_Op())
			dest->second.set_Op('+');
	}
	return (0);
}

void	Server::send_message(std::string prefix, std::string modes, std::vector<std::string> names, Channel *Ch)
{
	int	plus = 0;
	int minus = 0;
	std::string name_str;
	std::string final;
	for (size_t i = 0; i < modes.size(); i++)
	{
		if (modes[i] == '+' && plus == 0)
		{
			plus = 1;
			minus = 0;
		}
		else if (modes[i] == '-' && minus == 0)
		{
			minus = 1;
			plus = 0;
		}
		else if (modes[i] == '+' && plus == 1)
			modes.erase(i, 1);
		else if (modes[i] == '-' && minus == 1)
			modes.erase(i, 1);
	}
	for (size_t i = 0; i < names.size(); i++)
	{
		name_str += names[i];
		if (i < names.size() - 1)
			name_str += space;
	}
	final = prefix + modes + space + name_str + "\r\n";
	channel_iterator end = Ch->get_map_end();
	for (channel_iterator begin = Ch->get_map_begin(); begin!= end; begin++)
		send(begin->first->get_fd(), final.c_str(), final.size(), 0);
}