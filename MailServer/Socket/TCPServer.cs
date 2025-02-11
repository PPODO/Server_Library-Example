using MailServer.Packet;
using System.Net;
using System.Net.Sockets;

namespace MailServer.Socket
{
    class MailServer
    {
        private TcpListener listener;

        public MailServer(string ipAddress, Int16 portNumber)
        {
            listener = new TcpListener(IPAddress.Parse(ipAddress), portNumber);
            listener.Start();

            Console.WriteLine("Mail Server Started. Waiting For Client Connections");
        }

        public void Run()
        {
            while (true)
            {
                var clientSocket = new TCPClient(listener.AcceptTcpClient());

                Console.WriteLine("Client Connected!");

                clientSocket.RecvAsyncStart();
            }
        }
    }
}
