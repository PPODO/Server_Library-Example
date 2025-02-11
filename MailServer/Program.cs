using MailServer.Packet;
using MailServer.Socket;
using Mail;
using Google.FlatBuffers;
using MailServer.GMail;
using System.Text;

namespace MailServer
{
    internal class Program
    {
        private static void ReceiveMail(PACKET_STRUCT receivedPacket)
        {
            var packet = MailRequest.GetRootAsMailRequest(new ByteBuffer(receivedPacket.m_sPacketData));
            string subject = "None";
            string body = "None";
            switch ((Mail.RequestType)packet.RequestType)
            {
                case Mail.RequestType.Start:
                    subject = " Process Detected!";
                    body = "Watch Dog Detects a New Process " + packet.ProgramName + "!";
                    break;
                case Mail.RequestType.Stop:
                    subject = " Process Terminated!";
                    body = "Process Name " + packet.ProgramName + " Was Terminated!";
                    break;
            }
            GMailSender.SendMail("sherlockwhooo@gmail.com", packet.ProgramName + subject, body);
        }

        private static void DumpFileReceive(PACKET_STRUCT receivedPacket)
        {
            var packet = DumpFileTransmit.GetRootAsDumpFileTransmit(new ByteBuffer(receivedPacket.m_sPacketData));
            bool bIsEOF = false;
            FileMode fileMode;
            switch ((Mail.RequestType)packet.RequestType)
            {
                case RequestType.Start:
                    fileMode = FileMode.Create;
                    break;
                case RequestType.Loop:
                    fileMode = FileMode.Append;
                    break;
                case RequestType.Stop:
                    fileMode = FileMode.Append;
                    bIsEOF = true;
                    break;
                default:
                    return;
            }

            FileStream fileStream = new FileStream(packet.DumpFileName, fileMode);
            using (BinaryWriter writer = new BinaryWriter(fileStream))
            {
                writer.Write(packet.GetDataArray(), 0, packet.GetDataArray().Length);
            }

            if (bIsEOF)
            {
                GMailSender.SendMail("sherlockwhooo@gmail.com", packet.ProgramName + " Aborted!", packet.ProgramName + " Aborted! Attached Dump File. Check It Out.", packet.DumpFileName);
                System.IO.File.Delete(packet.DumpFileName);
            }
        }

        static void Main(string[] args)
        {
            PacketProcessor.packetProcessor.Add(Mail.PacketType.MailRequest, ReceiveMail);
            PacketProcessor.packetProcessor.Add(Mail.PacketType.DumpFileTransmit, DumpFileReceive);

            GMailSender.Initialize("sherlockwhooo@gmail.com", "eexb usxo itwn fbpw");

            MailServer.Socket.MailServer server = new("127.0.0.1", 3540);

            server.Run();
        }
    }
}
