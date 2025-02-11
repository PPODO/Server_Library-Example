using SheetDownloader.SheetManager;
using SheetDownloader.Socket;
using SheetDownloader.Packet;
using Newtonsoft.Json;
using System.Text;
using FlatPacket;

namespace SheetDownloader
{
    enum ESheetType
    {
        EST_Attendance = 1,
        EST_Event
    }

    struct SheetInfo
    {
        public delegate void SheetDownload();

        public string spreadSheetID;
        public string name;

        public SheetInfo(string spreadSheetID, string name)
        {
            this.spreadSheetID = spreadSheetID;
            this.name = name;
        }

    }

    class Program
    {
        static TCPClientSocket clientSocket = new TCPClientSocket("localhost", 3550);
        static GoogleSheetManager sheetManager = new GoogleSheetManager();

        static SheetInfo selectedSheetInfo;
        static bool bRunState = true;

        static public void DownloadAttendanceSheet()
        {
            Dictionary<int, AttendanceSheetData> sheetInformation = new Dictionary<int, AttendanceSheetData>();

            sheetManager.GetAttendanceInformationFromFirstSheet(selectedSheetInfo.spreadSheetID, sheetInformation);

            foreach (var data in sheetInformation)
                Console.WriteLine("{0} : {1}", data.Key, data.Value.ToString());
            Console.WriteLine("\n\n");
            Console.Write("Press Any Key Will Be Start Downloader : ");
            Console.ReadKey();

            List<AttendanceSheetData> sheetDatas = new List<AttendanceSheetData>();
            for (int i = 1; i <= sheetInformation.Count; i++)
            {
                var attendanceData = sheetManager.GetAttendanceSheetInformationByID(selectedSheetInfo.spreadSheetID, i, sheetInformation);
                if (attendanceData != null)
                    sheetDatas.Add(attendanceData);
            }

            var sSerializedJsonString = JsonConvert.SerializeObject(sheetDatas);

            JsonDownloadPacketGenerator packetGenerator = new JsonDownloadPacketGenerator();
            var createdPacket = packetGenerator.CreateNewDownloadNotificationPacket(PacketType.JsonDownloadStartPacket, sSerializedJsonString);
            if (createdPacket != null)
                clientSocket.Send(createdPacket);
        }

        static void ReceivedJsonDownloadStartPacket(PACKET_STRUCT packet)
        {
            Dictionary<int, AttendanceSheetData> sheetInformation = new Dictionary<int, AttendanceSheetData>();

            sheetManager.GetAttendanceInformationFromFirstSheet(selectedSheetInfo.spreadSheetID, sheetInformation);

            List<AttendanceSheetData> sheetDatas = new List<AttendanceSheetData>();
            for (int i = 1; i <= sheetInformation.Count; i++)
            {
                var attendanceData = sheetManager.GetAttendanceSheetInformationByID(selectedSheetInfo.spreadSheetID, i, sheetInformation);
                if (attendanceData != null)
                    sheetDatas.Add(attendanceData);
            }

            var sSerializedJsonString = JsonConvert.SerializeObject(sheetDatas);

            JsonDownloadPacketGenerator packetGenerator = new JsonDownloadPacketGenerator();
            var createdPacket = packetGenerator.CreateNewDownloadPacket(PacketType.JsonDownloadingPacket, sSerializedJsonString);
            if (createdPacket != null)
                clientSocket.Send(createdPacket);
        }

        static void ReceivedJsonDownloadPacket(PACKET_STRUCT packet)
        {
            JsonDownloadPacketGenerator packetGenerator = new JsonDownloadPacketGenerator();
            var createdPacket = packetGenerator.CreateNewDownloadPacket(PacketType.JsonDownloadEndPacket, "");
            if (createdPacket != null)
            {
                clientSocket.Send(createdPacket);
                bRunState = false;
            }
        }

        static void Main(string[] args)
        {
            clientSocket.AddNewPacketProcessorByPacketType(PacketType.JsonDownloadStartPacket, ReceivedJsonDownloadStartPacket);
            clientSocket.AddNewPacketProcessorByPacketType(PacketType.JsonDownloadingPacket, ReceivedJsonDownloadPacket);

            Dictionary<ESheetType, SheetInfo> sheetInfoDictionary = new Dictionary<ESheetType, SheetInfo>();
            sheetInfoDictionary.Add(ESheetType.EST_Attendance, new SheetInfo("1WEnuiMFRzcR2lxb8r9A_uxxtwUcU9VlpNNAL-2eJoQE", "Attendance"));

            Console.Write("Credentials Json File Path : ");
            string? sCredentialsFilePath = Console.ReadLine();
            Console.WriteLine("\n\n");

            if (sCredentialsFilePath == null) return;


            foreach (var data in sheetInfoDictionary)
                Console.WriteLine("{0}. {1}, {2}", (short)data.Key, data.Value.name, data.Value.spreadSheetID);

            Console.Write("\nSelect Sheet Info : ");
            ESheetType selectedKey = (ESheetType)(Console.ReadKey().KeyChar - '0');
            Console.WriteLine("\n");


            sheetManager.Initialize(sCredentialsFilePath);
            if (sheetInfoDictionary.TryGetValue(selectedKey, out selectedSheetInfo))
            {
                switch (selectedKey)
                {
                    case ESheetType.EST_Attendance:
                            DownloadAttendanceSheet();
                        break;
                    case ESheetType.EST_Event:

                        break;
                }
            }

            while (bRunState)
                clientSocket.ProcessingReceivedPacket();
        }
    }
}