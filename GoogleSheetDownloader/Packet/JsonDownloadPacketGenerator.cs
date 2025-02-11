using FlatPacket.JsonPacket;
using FlatPacket;
using Google.FlatBuffers;
using ICSharpCode.SharpZipLib.Zip.Compression.Streams;
using ICSharpCode.SharpZipLib.Zip.Compression;
using System.Text;

namespace SheetDownloader.Packet
{
    internal class JsonDownloadPacketGenerator
    {
        private FlatBufferBuilder m_builder;

        public JsonDownloadPacketGenerator()
        {
            m_builder = new FlatBufferBuilder(1);
        }

        public PACKET_STRUCT? CreateNewDownloadPacket(PacketType packetType, string sSerializedJsonString)
        {
            m_builder.Clear();

            var sCompressedJsonString = Compression(sSerializedJsonString);
            if (sCompressedJsonString == null) return null;
            
            var vectorOffset = JsonDownloadPacket.CreateCompressedJsonDataVector(m_builder, sCompressedJsonString);

            JsonDownloadPacket.StartJsonDownloadPacket(m_builder);
            JsonDownloadPacket.AddCompressedJsonData(m_builder, vectorOffset);
            m_builder.Finish(JsonDownloadPacket.EndJsonDownloadPacket(m_builder).Value);

            return new PACKET_STRUCT(new PACKET_INFORMATION((byte)packetType, (ushort)m_builder.SizedByteArray().Length), m_builder.SizedByteArray());
        }

        public PACKET_STRUCT? CreateNewDownloadNotificationPacket(PacketType packetType, string sSerializedJsonString)
        {
            m_builder.Clear();

            var sCompressedJsonString = Compression(sSerializedJsonString);
            if (sCompressedJsonString == null) return null;

            JsonDownloadNotifyPacket.StartJsonDownloadNotifyPacket(m_builder);
            JsonDownloadNotifyPacket.AddCompressedJsonLength(m_builder, sCompressedJsonString.Length);
            JsonDownloadNotifyPacket.AddOriginalJsonLength(m_builder, sSerializedJsonString.Length);
            m_builder.Finish(JsonDownloadNotifyPacket.EndJsonDownloadNotifyPacket(m_builder).Value);

            return new PACKET_STRUCT(new PACKET_INFORMATION((byte)packetType, (ushort)m_builder.SizedByteArray().Length), m_builder.SizedByteArray());
        }

        private byte[] Compression(string str)
        {
            byte[] inputBytes = Encoding.UTF8.GetBytes(str);
            using (MemoryStream outputStream = new MemoryStream())
            {
                using (DeflaterOutputStream zipStream = new DeflaterOutputStream(outputStream, new Deflater(Deflater.BEST_COMPRESSION)))
                {
                    zipStream.Write(inputBytes, 0, inputBytes.Length);
                }
                return outputStream.ToArray();
            }
        }

    }
}
