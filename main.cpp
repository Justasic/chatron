/* irc_client.cpp */

#include "Socket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
#include <time.h>

/*List the constants used throughout the script. 
If you wanna edit the server, channel, nick, etc that the bot
connects to, do so here, NOT in the main code.
*/
const std::string server = "irc.vemnet.us";
const std::string port = "6667";
const std::string channel = "#Computers";
const std::string nick = "Catron";
const std::string usrname = "chatron";
const std::string TimeZone = "GMT";
const std::string welcome_msg = nick+" has connected. Type !time to see the time."+" "+nick+"'s current timezone is: "+TimeZone;
const std::string channel_greeting = "Feel free to ask any questions. And have fun :)";

//Function used to get the time, format it, and return it
std::string tell_time()
{
	time_t rawtime;
	time(&rawtime);
	return ctime(&rawtime);
}

int main (int argc, char *argv[])
{
	try
	{
		//Make the socket used to connect to the server
		Socket sock(server, port);
		std::cout << "Hello! Connecting to server~" << std::endl;
		//Incase there is no connection, say so
		if (!sock.get_address())
			throw SocketException("Could not find irc server.");
		
		if (!sock.connect())
			throw SocketException("Could not connect to irc server.");
		
		std::cout << "Socket connected! Initiating IRC negotiation..." << std::endl;
		
		/*Create the variable std::string 'reply'
		'Reply' means 'What the server is saying back'
		Note the 'reply' is the raw IRC server reply incase you want
		to expand on my code
		*/
		std::string reply;

		//Accept some server replies after connecting
		sock >> reply;
		sock >> reply;
		std::cout << "Sending USER and NICK commands..." << std::endl;
		//Set the username and nick
		sock << "USER "+usrname+" d d "+usrname+"\n";
		sock << "NICK "+nick+"\n";
		sock >> reply;

		//Reply to the ping from the server
		std::string::size_type loc = reply.find( "PING :", 0 );
		if( loc != std::string::npos )
		{
			std::string pong = "PONG :" + reply.substr(6,-1);
			sock << pong;
		}
		
		//You will see why im making a bool variable here later  
		bool in_channel = false;
		
		std::cout << "Joining channel..." << std::endl;
		//Join the 
		sock << "JOIN " + channel + "\n";
		sock << "PRIVMSG " + channel + " " + welcome_msg + "\n";

		//infi loop to stay connected
		while (true)
		{ 
			sock >> reply; //keep showing what the server says
			
			//Another ping reply
			std::string::size_type loc = reply.find( "PING :", 0 );
			if( loc != std::string::npos )
			{
				std::string pong = "PONG :" + reply.substr(6,-1);
				sock << pong;
			}
			
			/*Find the channel_greeting
			If found it makes in_channel true.
			This is so you can make things happen only if you know you are in 
			a channel.
			*/
			int i = reply.find("353");
			if(i != std::string::npos)
			{
				std::cout << "Channel join confirmation... CHECK\n";
				in_channel = true;
			}
			
			//fun :P
			int b = reply.find(nick);
			if (b != std::string::npos && in_channel)
			{
				sock << "PRIVMSG " + channel + " Someone said my name!" + "\n";
			}
			
			/*If it looks for !time in a server reply (aka if anyone in the channel
			says it) and if it finds it, it tells the time.
			*/
			int t = reply.find("!time");
			if (t != std::string::npos && in_channel)
			{
				sock << "PRIVMSG " + channel + " " + tell_time() + "\n";
			}
		}
	}
	catch (SocketException& e)
	{
		std::cout << "Exception was caught: " << e.description() << "\n";
	}
	
	// Return success
	return 0;
}
