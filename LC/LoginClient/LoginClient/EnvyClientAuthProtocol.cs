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
        private bool terminate;
        public EnvyClientAuthProtocol(EnvyConnection connection)
        {
            activeConnection = connection;
            terminate = false;
        }
        private bool Equals(byte[] source, byte[] separator, int idx)
        {
            for(var i = 0; i < separator.Length; ++i)
            {
                if (idx + i >= source.Length || source[idx + i] != separator[i])
                    return false;
            }
            return true;
        }
        private byte[][] Separate(byte[] source, byte[] separator)
        {
            var chunks = new List<byte[]>();
            var idx = 0;
            byte[] chunk;
            for(var i = 0; i < source.Length; ++i)
            {
                if(Equals(source, separator, i))
                {
                    chunk = new byte[i - idx];
                    Array.Copy(source, idx, chunk, 0, chunk.Length);
                    chunks.Add(chunk);
                    idx = i + separator.Length;
                    i += separator.Length - 1;
                }
            }
            chunk = new byte[source.Length - idx];
            Array.Copy(source, idx, chunk, 0, chunk.Length);
            chunks.Add(chunk);
            return chunks.ToArray();
        }

        private EnvyNetMessage[] ConsumeNetMessages()
        {
            EnvyNetMessage[] ret;
            byte[] rawdata = activeConnection.ConsumeNetworkStream();
            byte[] sep = { 4 };
            byte[][] data = Separate(rawdata, sep);
            int bufferSize = 0;
            for (int i = 0; i < data.Length; i++) if (data[i].Length > 0) bufferSize++;
            ret = new EnvyNetMessage[bufferSize];
            for(var i = 0; i < data.Length; i++)
            {
                if(data[i].Length > 0)
                    ret[i] = EnvyNetMessage.Parser.ParseFrom(data[i]);
            }
            return ret;
        }

        public bool AuthenticateUser(EnvyUser user)
        {
            while(!terminate)
            {
                var messages = ConsumeNetMessages();
                foreach(var msg in messages)
                {
                    if (msg == null) continue;
                    switch (msg.Type)
                    {
                        case EnvyNetMessage.Types.NetMessageType.NetOpenConnection:
                            {
                                activeConnection.PushNetworkStream(Encoding.ASCII.GetBytes("ACCEPT\n"));
                                break;
                            }
                        case EnvyNetMessage.Types.NetMessageType.NetAuthMessage:
                            {
                                var response = user.username + ":" + user.password;
                                activeConnection.PushNetworkStream(Encoding.ASCII.GetBytes(response + "\n"));
                                break;
                            }
                        case EnvyNetMessage.Types.NetMessageType.NetAuthResponse:
                            {
                                if (msg.DataSize == -1)
                                {
                                    Console.WriteLine("INVALID LOGIN");
                                    return false;
                                }
                                else
                                {
                                    String response = msg.Data.ToStringUtf8();
                                    var info = response.Split(':');
                                    Console.WriteLine("Welcome: " + info[0] + " you are an " + info[1]);
                                    return true;
                                }
                                break;
                            }
                        case EnvyNetMessage.Types.NetMessageType.NetCloseConnection:
                            {
                                terminate = true;
                                break;
                            }
                    }
                }
            }
            return false;
        }
    }
}
