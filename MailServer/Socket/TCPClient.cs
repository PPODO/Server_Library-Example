using MailServer.Packet;
using System.Net.Sockets;
using Mail;

namespace MailServer.Socket
{
    class PacketProcessor
    {
        public delegate void ProcessingPacketData(PACKET_STRUCT receivedPacket);
        public static Dictionary<Mail.PacketType, ProcessingPacketData> packetProcessor = new Dictionary<Mail.PacketType, ProcessingPacketData>();
    }
    
    class TCPClient
    {
        private TcpClient tcpClient;
        private const int BUFFER_LENGTH = 2048;

        private byte[] messageBuffer = new byte[BUFFER_LENGTH];
        private int remainBytes = 0;

        public TCPClient(TcpClient _tcpClient)
        {
            tcpClient = _tcpClient;
            if (tcpClient.Connected)
                RecvAsyncStart();
        }

        public void RecvAsyncStart()
        {
            var networkStream = tcpClient.GetStream();
            if (tcpClient.Connected)
                networkStream.BeginRead(messageBuffer, remainBytes, BUFFER_LENGTH - remainBytes, this.RecvAsynEnd, tcpClient);
        }

        private void RecvAsynEnd(IAsyncResult result)
        {
            var socketHandle = (TcpClient)result.AsyncState;
            if (socketHandle != null)
            {
                if (result.IsCompleted && socketHandle.Connected)
                {
                    var socketStream = socketHandle.GetStream();

                    int nReadSize = socketStream.EndRead(result);
                    remainBytes += nReadSize;

                    if (remainBytes > 0)
                    {
                        PACKET_STRUCT packetStructure = new PACKET_STRUCT();
                        if (packetStructure.ExtractPacketDataFromReceiveBuffer(messageBuffer, ref remainBytes))
                        {
                            PacketProcessor.ProcessingPacketData processFunc;
                            if (PacketProcessor.packetProcessor.TryGetValue((Mail.PacketType)packetStructure.m_packetInfo.m_iPacketType, out processFunc))
                                processFunc(packetStructure);

                            remainBytes -= packetStructure.m_packetInfo.GetSize() + packetStructure.m_packetInfo.m_iPacketDataSize;
                        }
                    }
                    RecvAsyncStart();
                }
                else
                {
                    Console.WriteLine("Client Disconnected!");
                    socketHandle.Close();
                }
            }
        }
    }
}
