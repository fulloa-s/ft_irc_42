/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vspinell <vspinell@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 15:04:59 by vspinell          #+#    #+#             */
/*   Updated: 2022/03/19 17:08:12 by vspinell         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"
#include "client.hpp"
#include "server.hpp"

#define	MESS(x) {ret = x; break;}

std::string	assign_macros(std::string ret, char* arg)
{
	std::string	replace = arg;
	size_t first;
	size_t last;

	first = ret.find('<');
	last = ret.find('>');
	if( first != std::string::npos)
		ret.replace(first, last - first + 1, replace);
	return (ret);
}

std::string	add_prefix(std::string err_msg, int err, std::string client_name, bool args)
{
	char	buff[4];
	sprintf(buff, "%d", err);
	std::string err_n = buff;
	if (client_name.empty())
		client_name = "*";
	std::string prefix = ":myserv " + err_n + space;
	if (args == true)
		prefix += client_name + space;
	return(prefix + err_msg + "\r\n");
}

int Server::send_error(Client &requester, int err, int n_param, ...)
{
	std::string ret;
	va_list	ap;
	va_start(ap, n_param);
	
	switch (err)
	{
		// case 001 : MESS( "001 Welcome to the Internet Relay Network " + NICK + "!" + USER + '@' + HOST );
		// case 002 : MESS( "Your host is" +  SERVERNAME + ", running version <ver>" );//////////
		case 003 : MESS( "This server was created <date>" );//////////
		case 004 : MESS( "<servername> <version> <available user modes> <available channel modes>" );///////
		case 200 : MESS( "Link <version & debug level> <destination> <next server> V<protocol version> <link uptime in seconds> <backstream sendq> <upstream sendq>" );
		case 201 : MESS( "Try. <class> <server>" );
		case 202 : MESS( "H.S. <class> <server>" );
		case 203 : MESS( "???? <class> [<client IP address in dot form>]" );
		case 204 : MESS( "Oper <class> <nick>" );
		case 205 : MESS( "User <class> <nick>" );
		case 206 : MESS( "Serv <class> <int>S <int>C <server> <nick!user|*!*>@<host|server> V<protocol version>");
		case 207 : MESS( "Service <class> <name> <type> <active type>" );
		case 208 : MESS( "<newtype> 0 <client name>" );
		case 209 : MESS( "Class <class> <count>" );
		case 211 : MESS( "<linkname> <sendq> <sent messages> <sent Kbytes> <received messages> <received Kbytes> <time open>");
		case 212 : MESS( "<command> <count> <byte count> <remote count>" );
		case 219 : MESS( "<stats letter> :End of STATS report" );
		case 221 : MESS( "<user mode string>" );
		case 234 : MESS( "<name> <server> <mask> <type> <hopcount> <info>" );
		case 235 : MESS( "<mask> <type> :End of service listing" );
		case 242 : MESS( ":Server Up %d days %d:%02d:%02d" );
		case 243 : MESS( "O <hostmask> * <name>" );
		case 251 : MESS( ":There are <integer> users and <integer> services on <integer> servers" );
		case 252 : MESS( "<integer> :operator(s) online" );
		case 253 : MESS( "<integer> :unknown connection(s)" );
		case 254 : MESS( "<integer> :channels formed" );
		case 255 : MESS( ":I have <integer> clients and <integer> servers" );
		case 256 : MESS( "<server> :Administrative info" );
		case 257 : MESS( ":<admin info>" );
		case 258 : MESS( ":<admin info>" );
		case 259 : MESS( ":<admin info>" );
		case 261 : MESS( "File <logfile> <debug level>" );
		case 262 : MESS( "<server name> <version & debug level> :End of TRACE" );
		case 263 : MESS( "<command> :Please wait a while and try again." );
		case 301 : MESS( "<nick> :<away message>" );
		case 302 : MESS( ":*1<reply> *( " " <reply> )" );
		case 303 : MESS( ":*1<nick> *( " " <nick> )" );
		case 305 : MESS( ":You are no longer marked as being away" );
		case 306 : MESS( ":You have been marked as being away" );
		case 311 : MESS( "<nick> <user> <host> * :<real name>" );
		case 312 : MESS( "<nick> <server> :<server info>" );
		case 313 : MESS( "<nick> :is an IRC operator" );
		case 314 : MESS( "<nick> <user> <host> * :<real name>" );
		case 315 : MESS( "<name> :End of WHO list" );
		case 317 : MESS( "<nick> <integer> :seconds idle" );
		case 318 : MESS( "<nick> :End of WHOIS list" );
		case 319 : MESS( "<nick> :*( ( '@' / '+' ) <channel> " " )" );
		case 322 : MESS( "<channel> <# visible> :<topic>" );
		case 323 : MESS( ":End of LIST" );
		case 324 : MESS( "<channel> <mode> " );
		case 325 : MESS( "<channel> <nickname>" );
		case 331 : MESS( "<channel> :No topic is set" );
		case 332 : MESS( "<channel> :<topic>" );
		case 341 : MESS( "<channel> <nick>" );
		case 342 : MESS( "<user> :Summoning user to IRC" );
		case 346 : MESS( "<channel> <invitemask>" );
		case 347 : MESS( "<channel> :End of channel invite list" );
		case 348 : MESS( "<channel> <exceptionmask>" );
		case 349 : MESS( "<channel> :End of channel exception list" );
		case 351 : MESS( "<version>.<debuglevel> <server> :<comments>" );
		case 352 : MESS( "<channel> <user> <host> <server> <nick> H<flag> :<0> ");
		case 353 : MESS( "( '=' / '*' / '@' ) <channel> :[ '@' / '+' ] <nick> *( " " [ '@' / '+' ] <nick> )" );
		case 364 : MESS( "<mask> <server> :<hopcount> <server info>" );
		case 365 : MESS( "<mask> :End of LINKS list" );
		case 366 : MESS( "<channel> :End of NAMES list" );
		case 367 : MESS( "<channel> <banmask>" );
		case 368 : MESS( "<channel> :End of channel ban list" );
		case 369 : MESS( "<nick> :End of WHOWAS" );
		case 371 : MESS( ":<string>" );
		case 372 : MESS( ":- <text>" );
		case 374 : MESS( ":End of INFO list" );
		case 375 : MESS( ":- <server> Message of the day - " );
		case 376 : MESS( ":End of MOTD command" );
		case 381 : MESS( ":You are now an IRC operator" );
		case 382 : MESS( "<config file> :Rehashing" );
		case 383 : MESS( "You are service <servicename>" );
		case 391 : MESS( "<server> :<string showing server's local time>" );
		case 392 : MESS( ":UserID   Terminal  Host" );
		case 393 : MESS( ":<username> <ttyline> <hostname>" );
		case 394 : MESS( ":End of users" );
		case 395 : MESS( ":Nobody logged in" );
		case 401 : MESS( "<nickname> :No such nick" );
		case 402 : MESS( "<server name> :No such server" );
		case 403 : MESS( "<channel name> :No such channel" );
		case 404 : MESS( "<channel name> :Cannot send to channel" );
		case 405 : MESS( "<channel name> :You have joined too many channels" );
		case 406 : MESS( "<nickname> :There was no such nickname" );
		case 407 : MESS( "<target> :<error code> recipients. <abort message>" )
		case 408 : MESS( "<service name> :No such service" );
		case 409 : MESS( ":No origin specified" );
		case 411 : MESS( ":No recipient given (<command>)" )
		case 412 : MESS( ":No text to send" );
		case 413 : MESS( ":No toplevel domain specified" );
		case 414 : MESS( "<mask> :Wildcard in toplevel domain" );
		case 415 : MESS( "<mask> :Bad Server/host mask" );
		case 421 : MESS( "<command> :Unknown command" );
		case 422 : MESS( ":MOTD File is missing" );
		case 423 : MESS( "<server> :No administrative info available" );
		case 424 : MESS( ":File error doing <file op> on <file>" )
		// ----- done ------ //
		case 431 : MESS( ":No nickname given" );
		case 432 : MESS( "<nick> :Erroneous nickname" ); //da implementaree
		// ----- done ------ //
		case 433 : MESS( "<nick> :Nickname is already in use" );
		case 436 : MESS( "<nick> :Nickname collision KILL from <user>@<host>" );///////////// da non fareregistered of a NICK that already exists by another server)
		// ----- done ------ //
		case 462 : MESS( ":Unauthorized command (already registered)" );
		case 437 : MESS( "<nick/channel>:Nick/channel is temporarily unavailable" );
		case 441 : MESS( "<nick> <channel> :They aren't on that channel" ); //da fare 
		case 442 : MESS( "<channel> :You're not on that channel" );
		case 443 : MESS( "<user> <channel> :is already on channel" )
		case 444 : MESS( "<user> :User not logged in" );
		case 445 : MESS( ":SUMMON has been disabled" );
		case 446 : MESS( ":USERS has been disabled" );
		case 451 : MESS( ":You have not registered" );
		case 461 : MESS( "<command> :Not enough parameters" ); 
		case 463 : MESS( ":Your host isn't among the privileged" );
		// ----- done ------ //
		case 464 : MESS( ":Password incorrect" );
		case 465 : MESS( ":You are banned from this server" );
		case 467 : MESS( "<channel> :Channel key already set" );
		case 471 : MESS( "<channel> :Cannot join channel (+l)" );
		case 472 : MESS( "<char> :is unknown mode char to me for <channel>" )
		case 473 : MESS( "<channel> :Cannot join channel (+i)" );
		case 474 : MESS( "<channel> :Cannot join channel (+b)" );
		case 475 : MESS( "<channel> :Cannot join channel (+k)" );
		case 476 : MESS( "<channel> :Bad Channel Mask" );
		case 477 : MESS( "<channel> :Channel doesn't support modes" );
		case 478 : MESS( "<channel> <channel> <char> :Channel list is full" )
		case 481 : MESS( ":Permission Denied- You're not an IRC operator" );
		case 482 : MESS( "<channel> :You're not channel operator" ); //se qualcuno prova a cambiare il mode non essendo operator
		case 483 : MESS( ":You can't kill a server!" );
		case 484 : MESS( ":Your connection is restricted!" );
		case 485 : MESS( ":You're not the original channel operator" );
		case 491 : MESS( ":No O-lines for your host" ); //da  If a client sends an OPER message and the server has not been configured to allow connections from the client's host as an operator, this error must be returned.
		case 501 : MESS( ":Unknown MODE flag" );  //implementa quando viene lanciato mode al di fuori di un canale (+o non fa niente invece)
		case 502 : MESS( ":Cannot change mode for other users" ); ///implementa quando si usa mode fuori da un canale per cambiare lo status di un altro
	}
	for (int i = 0; i < n_param; i++)
	{
		ret = assign_macros(ret, va_arg(ap, char*));
	}
	va_end(ap);
	ret = add_prefix(ret, err, requester.get_nick(), n_param > 0);
	// ret += "\r\n";
	// std:: cout << "\n////////////////////////////////////\n\n";
	// std::cout << ret << std::endl;
	// std::cout  << "////////////////////////////////////\n\n";
	send(requester.get_fd(), ret.c_str(), ret.size(), 0);
	return(1);
}

///APPUNTI

//:paolo!kvirc@host-2-114-8-5.business.telecomitalia.it MODE #e +o alfio
//paolo is the operator && alfio is the user