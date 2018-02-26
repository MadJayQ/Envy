using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using EnvyFileServer.Net.EnvyNetMessage;

namespace LoginClient
{
    class EnvyClientAuthProtocol
    {
        EnvyConnection activeConnection;
        public EnvyClientAuthProtocol(EnvyConnection connection)
        {
            activeConnection = connection;
        }

        public bool AuthenticateUser(EnvyUser user)
        {
            EnvyNetMessage msg = new EnvyNetMessage();
            while(msg.Type != EnvyNetMessage.Types.NetMessageType.NetCloseConnection)
            {
                byte[] data = activeConnection.ConsumeNetworkStream();
                msg = EnvyNetMessage.Parser.ParseFrom(data);
                switch (msg.Type)
                {
                    case EnvyNetMessage.Types.NetMessageType.NetOpenConnection:
                        {
                            activeConnection.PushNetworkStream(Encoding.ASCII.GetBytes("ACCEPT"));
                            break;
                        }
                    case EnvyNetMessage.Types.NetMessageType.NetAuthMessage:
                        {
                            var response = user.username + ":" + user.password;
                            activeConnection.PushNetworkStream(Encoding.ASCII.GetBytes(response));
                            break;
                        }
                }
            }
            return true;
        }
    }
}
