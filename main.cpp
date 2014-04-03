/* irc_client.cpp */

#include "Socket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
#include <time.h>

using namespace std;

/*List the constants used throughout the script. 
If you wanna edit the server, channel, nick, etc that the bot
connects to, do so here, NOT in the main code.
*/
const string server = "irc.flux-net.net";
const string port = "6667";
const string channel = "#computers";
const string nick = "Time_Bot";
const string usrname = "Time_Bot";
const string TimeZone = "GMT";
const string welcome_msg = nick+" has connected. Type !time to see the time."+" "+nick+"'s current timezone is: "+TimeZone;
const string channel_greeting = "Feel free to ask any questions. And have fun :)";

//Function used to get the time, format it, and return it
string tell_time(){
  time_t rawtime;
  time(&rawtime);
  return ctime(&rawtime);
}

int main ()
{

  
  try
  {
    //Make the socket used to connect to the server
    Socket sock(server,port);
    //Incase there is no connection, say so
    if ( !sock.get_address() ) throw SocketException ( "Could not find irc server." );
    if ( !sock.connect() ) throw SocketException ( "Could not connect to irc server." );
    
    /*Create the variable string 'reply'
    'Reply' means 'What the server is saying back'
    Note the 'reply' is the raw IRC server reply incase you want
    to expand on my code
    */
    string reply;

    //Accept some server replies after connecting
    sock >> reply;
    sock >> reply;
    //Set the username and nick
    sock << "USER "+usrname+" d d "+usrname+"\r\n";
    sock << "NICK "+nick+"\r\n";
    sock >> reply;

    //Reply to the ping from the server
    string::size_type loc = reply.find( "PING :", 0 );
    if( loc != string::npos ){
      string pong = "PONG :" + reply.substr(6,-1);
      sock << pong;
      }
    
    //You will see why im making a bool variable here later  
    bool in_channel = false;
      
    //Join the 
    sock << "JOIN " + channel + "\r\n";
    sock << "PRIVMSG " + channel + " " + welcome_msg + "\r\n";

    while (true){ //infi loop to stay connected
      
      sock >> reply; //keep showing what the server says
      
      //Another ping reply
      string::size_type loc = reply.find( "PING :", 0 );
      if( loc != string::npos ){
	string pong = "PONG :" + reply.substr(6,-1);
	sock << pong;
      }
      
      /*Find the channel_greeting
      If found it makes in_channel true.
      This is so you can make things happen only if you know you are in 
      a channel.
      */
      int i = reply.find(channel_greeting);
      if(i != string::npos) {
	cout << "Channel join confirmation... CHECK";
	in_channel = true;
      }
      
      //fun :P
      int b = reply.find(nick);
      if (b != string::npos && in_channel){
	sock << "PRIVMSG " + channel + " Someone said my name!" + "\r\n";
      }
      
      /*If it looks for !time in a server reply (aka if anyone in the channel
      says it) and if it finds it, it tells the time.
      */
      int t = reply.find("!time");
      if (t != string::npos && in_channel){
	sock << "PRIVMSG " + channel + " " + tell_time() + "\r\n";
      }
      
    } 
    
    
  }
  catch ( SocketException& e )
  {
    cout << "Exception was caught:" << e.description() << "\n";
  }
  return 0;
}