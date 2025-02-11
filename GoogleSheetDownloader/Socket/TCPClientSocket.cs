using System.Net.Sockets;
using FlatPacket;
using SheetDownloader.Packet;

namespace SheetDownloader.Socket
{
    internal class TCPClientSocket
    {
        public delegate void ProcessingPacketData(PACKET_STRUCT receivedPacket);
        
        private TcpClient m_tcpClient;
        private const int BUFFER_LENGTH = 2048;

        private byte[] messageBuffer = new byte[BUFFER_LENGTH];
        private int remainBytes = 0;

        private Dictionary<PacketType, ProcessingPacketData> packetProcessorDelegateDictionary = new Dictionary<PacketType, ProcessingPacketData>();

        private Mutex csForReceivedPacektQueue = new Mutex();
        private Queue<PACKET_STRUCT> receivedPacketQueue = new Queue<PACKET_STRUCT>();

        public TCPClientSocket(string sAddress, ushort iPortNumber)
        {
            m_tcpClient = new TcpClient();
            m_tcpClient.Connect(sAddress, iPortNumber);

            RecvAsyncStart();
        }
        ~TCPClientSocket()
        {
            if (m_tcpClient != null)
                m_tcpClient.Close();
        }

        public void ProcessingReceivedPacket()
        {
            csForReceivedPacektQueue.WaitOne();
            while (receivedPacketQueue.Count > 0)
            {
                var receviedPacket = receivedPacketQueue.Dequeue();
                ProcessingPacketData outputPacketProcessor;

                if (packetProcessorDelegateDictionary.TryGetValue((PacketType)receviedPacket.m_packetInfo.m_iPacketType, out outputPacketProcessor))
                    outputPacketProcessor(receviedPacket);
            }
            csForReceivedPacektQueue.ReleaseMutex();
        }

        public void Send(byte[] sBuffer)
        {
            var networkStream = m_tcpClient.GetStream();
            networkStream.Write(sBuffer);
        }

        public void Send(PACKET_STRUCT? packetStructure)
        {
            if (packetStructure.HasValue)
                this.Send(packetStructure.Value.GetBuffer());
        }
        private void RecvAsyncStart()
        {
            var networkStream = m_tcpClient.GetStream();
            if (m_tcpClient.Connected)
                networkStream.BeginRead(messageBuffer, remainBytes, BUFFER_LENGTH, this.RecvAsynEnd, m_tcpClient);
        }

        private void RecvAsynEnd(IAsyncResult result)
        {
            var socketHandle = (TcpClient)result.AsyncState;
            if (result.IsCompleted && socketHandle != null && socketHandle.Connected)
            {
                var socketStream = socketHandle.GetStream();

                int nReadSize = socketStream.EndRead(result);
                remainBytes += nReadSize;

                if (remainBytes > 0)
                {
                    PACKET_STRUCT packetStructure = new PACKET_STRUCT();
                    if (packetStructure.ExtractPacketDataFromReceiveBuffer(messageBuffer, ref remainBytes))
                    {
                        csForReceivedPacektQueue.WaitOne();
                        receivedPacketQueue.Enqueue(packetStructure);
                        csForReceivedPacektQueue.ReleaseMutex();

                        remainBytes -= packetStructure.m_packetInfo.GetSize() + packetStructure.m_packetInfo.m_iPacketDataSize;
                    }
                }
                RecvAsyncStart();
            }
        }
        public void AddNewPacketProcessorByPacketType(PacketType packetType, ProcessingPacketData processor)
        {
            packetProcessorDelegateDictionary.Add(packetType, processor);
        }
    }
}
