using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Net.Sockets;
using System.IO;

using EnvyFileServer.Net.EnvyNetMessage;
namespace LoginClient
{
    class EnvyConnection
    {
        private TcpClient internalSocket;
        private String targetHost;
        private int targetPort;
        private bool internalConnected;
        public EnvyConnection(String hostname, String port)
        {
            targetHost = hostname;
            targetPort = Int32.Parse(port);
        }
        public EnvyConnection(String hostname, int port)
        {
            targetHost = hostname;
            targetPort = port;
        }

        public bool Connected
        {
            get
            {
                return internalSocket.Connected;
            }
            set
            {
                if(internalSocket.Connected && !value)
                {
                    internalSocket.Close();
                }
                else if(!internalSocket.Connected && value)
                {
                    internalSocket.Connect(targetHost, targetPort);
                }

                internalConnected = value;
            }
        }

        public int Port
        {
            get
            {
                return targetPort;
            }
            set
            {
                bool connected = internalSocket.Connected;
                if(connected)
                {
                    internalSocket.Close();
                    targetPort = value;
                    internalSocket.Connect(targetHost, targetPort);
                }
                else
                {
                    targetPort = value;
                }
            }
        }
        

        public bool Connect()
        {
            if(internalSocket == null)
            {
                internalSocket = new TcpClient();
            }
            internalSocket.Connect(targetHost, targetPort);
            return internalSocket.Connected;
        }

        public void PushNetworkStream(byte[] data)
        {
            if (internalSocket.GetStream().CanWrite)
            {
                internalSocket.GetStream().Write(data, 0, data.Length);
            }
        }

        public byte[] ConsumeNetworkStream(bool async = false)
        {
            if (internalSocket.GetStream().CanRead)
            {
                // Buffer to store the response bytes.
                byte[] readBuffer = new byte[internalSocket.ReceiveBufferSize];
                using (var writer = new MemoryStream())
                {
                    while(internalSocket.GetStream().DataAvailable)
                    {
                        int numberOfBytesRead = internalSocket.GetStream().Read(readBuffer, 0, readBuffer.Length);
                        if (numberOfBytesRead <= 0)
                        {
                            break;
                        }
                        writer.Write(readBuffer, 0, numberOfBytesRead);
                    }
                    return writer.ToArray();
                }
            }
            return null;
        }
    }
}
