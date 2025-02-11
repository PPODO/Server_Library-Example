using System.Runtime.InteropServices;
using System.Text;

namespace SheetDownloader.Packet
{
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct PACKET_INFORMATION
    {
        public Byte m_iPacketType;
        public UInt16 m_iPacketDataSize;
        public UInt16 m_iPacketNumber;

        public PACKET_INFORMATION(Byte iPacketType, UInt16 iPacketDataSize)
        {
            m_iPacketType = iPacketType;
            m_iPacketDataSize = iPacketDataSize;
            m_iPacketNumber = 0;
        }

        public int GetSize()
        {
            return Marshal.SizeOf(this);
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct PACKET_STRUCT
    {
        public static ulong BUFFER_LENGTH = 2048;

        public PACKET_INFORMATION m_packetInfo;
        public Byte[] m_sPacketData;

        public PACKET_STRUCT(PACKET_INFORMATION packetInfo, string sPacketData)
        {
            m_sPacketData = new Byte[BUFFER_LENGTH];
            this.m_packetInfo = packetInfo;

            Encoding.UTF8.GetBytes(sPacketData, 0, sPacketData.Length, m_sPacketData, 0);
        }

        public PACKET_STRUCT(PACKET_INFORMATION packetInfo, Byte[] sPacketData)
        {
            m_sPacketData = new Byte[BUFFER_LENGTH];
            this.m_packetInfo = packetInfo;

            Array.Copy(sPacketData, m_sPacketData, sPacketData.Length);
        }


        public int GetSize()
        {

            return this.m_packetInfo.GetSize() + m_packetInfo.m_iPacketDataSize;
        }

        public byte[] GetBuffer()
        {
            IntPtr buffer = Marshal.AllocHGlobal(this.m_packetInfo.GetSize());
            Marshal.StructureToPtr(this.m_packetInfo, buffer, false);

            byte[] data = new byte[GetSize()];
            Marshal.Copy(buffer, data, 0, this.m_packetInfo.GetSize());
            Marshal.FreeHGlobal(buffer);

            Buffer.BlockCopy(this.m_sPacketData, 0, data, this.m_packetInfo.GetSize(), m_packetInfo.m_iPacketDataSize);

            return data;
        }

        public bool ExtractPacketDataFromReceiveBuffer(byte[] receiveBuffer, ref int recvBytes)
        {
            m_sPacketData = new Byte[BUFFER_LENGTH];

            unsafe
            {
                fixed (byte* bufferPtr = receiveBuffer)
                {
                    this.m_packetInfo = Marshal.PtrToStructure<PACKET_INFORMATION>((IntPtr)bufferPtr);
                    if (recvBytes >= this.m_packetInfo.m_iPacketDataSize)
                    {
                        Array.Copy(receiveBuffer, this.m_packetInfo.GetSize(), m_sPacketData, 0, this.m_packetInfo.m_iPacketDataSize);
                        return true;
                    }
                }
            }
            return false;
        }
    }
}