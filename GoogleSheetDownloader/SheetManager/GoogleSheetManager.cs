using Google.Apis.Auth.OAuth2;
using Google.Apis.Services;
using Google.Apis.Sheets.v4;
using Google.Apis.Util.Store;

namespace SheetDownloader.SheetManager
{
    struct ItemData
    {
        public short item_id;
        public short item_count;

        public ItemData()
        {
            item_id = 0;
            item_count = 0;
        }
        public ItemData(short iItemID, short iItemCount)
        {
            item_id = iItemID;
            item_count = iItemCount;
        }

    }

    struct AttendanceData
    {
        public bool Is_Special_Reward;
        public bool Send_To_Mail;
        public List<ItemData> listOfItems;

        public AttendanceData()
        {
            Is_Special_Reward = false;
            Send_To_Mail = false;
            listOfItems = new List<ItemData>();
        }

        public AttendanceData(bool bIsSpecial, bool bSendToMail)
        {
            Is_Special_Reward = bIsSpecial;
            Send_To_Mail = bSendToMail;
            listOfItems = new List<ItemData>();
        }
    }

    internal class AttendanceSheetData
    {
        private short EventType;
        private short EventID;
        private string? EventStartDate;
        private string? EventEndDate;
        private string? SheetName;
        List<AttendanceData> attendanceDatas;

        public AttendanceSheetData()
        {
            attendanceDatas = new List<AttendanceData>();
            EventID = 0;
            EventType = 0;
            EventEndDate = null;
            EventStartDate = null;
        }

        public AttendanceSheetData(short iEventType, short iEventID, string eventStartDate, string eventEndDate, string sSheetName)
        {
            attendanceDatas = new List<AttendanceData>();
            EventType = iEventType;
            EventID = iEventID;
            EventStartDate = eventStartDate;
            EventEndDate = eventEndDate;
            SheetName = sSheetName;
        }

        public short Event_Type
        {
            get { return EventType; }
        }
        public short Event_ID
        {
            get { return EventID; }
        }
        public string? Event_Start_Date
        {
            get { return EventStartDate; }
        }
        public string? Event_End_Date
        {
            get { return EventEndDate; }
        }
        public string? Sheet_Name
        {
            get { return SheetName; }
        }

        public List<AttendanceData> AttendanceDatas
        {
            get { return attendanceDatas; }
            set { attendanceDatas = value; }
        }

        public override string? ToString()
        {
            return string.Format("{0}, {1}, {2}, {3}, {4}", EventType, EventID, EventStartDate, EventEndDate, SheetName);
        }

    }

    internal class EventSheetData
    {
        private int eventID;
        private string? eventDescription;

        public EventSheetData()
        {
            eventID = 0;
        }

        public EventSheetData(int eventID, string eventDescription)
        {
            this.eventID = eventID;
            this.eventDescription = eventDescription;
        }

        public int EventID
        {
            get { return eventID; }
        }

        public string? EventDescription
        {
            get { return eventDescription; }
        }

    }

    internal class GoogleSheetManager
    {
        private SheetsService? m_serviceInstance;

        public GoogleSheetManager()
        {
        }

        public void Initialize(String sCredentialsPath)
        {
            string[] sScopes = { SheetsService.Scope.SpreadsheetsReadonly };
            UserCredential credential;

            using (var stream = new FileStream(sCredentialsPath, FileMode.Open, FileAccess.Read))
            {
                string sCredPath = System.Environment.GetFolderPath(System.Environment.SpecialFolder.Personal);
                Path.Combine(sCredPath, ".credentials/sheets.googleapis.com-dotnet-quickstart.json");

                credential = GoogleWebAuthorizationBroker.AuthorizeAsync(GoogleClientSecrets.FromStream(stream).Secrets, sScopes, "user", CancellationToken.None, new FileDataStore(sCredPath, true)).Result;
            }
            m_serviceInstance = new SheetsService(new BaseClientService.Initializer() { HttpClientInitializer = credential, ApplicationName = "Google Sheet Manager" });
        }

        public AttendanceSheetData? GetAttendanceSheetInformationByID( String sSpreadSheetID, int iSheetID, Dictionary<int, AttendanceSheetData> sheetInformation)
        {
            if (m_serviceInstance != null)
            {
                var sheetInfo = m_serviceInstance.Spreadsheets.Get(sSpreadSheetID).Execute();
                if (sheetInfo == null) return null;

                AttendanceSheetData outSheetData = new AttendanceSheetData();
                sheetInformation.TryGetValue(iSheetID, out outSheetData);
                if(outSheetData == null) return null;

                var sheetInfoRequest = m_serviceInstance.Spreadsheets.Values.Get(sSpreadSheetID, outSheetData.Sheet_Name).Execute();
                foreach (var value in sheetInfoRequest.Values.Select((value, index) => (value, index)))
                {
                    if (value.index == 0) continue;
                    AttendanceData attendanceData = new AttendanceData();

                    attendanceData.Is_Special_Reward = Convert.ToBoolean(value.value[1]);
                    attendanceData.Send_To_Mail = Convert.ToBoolean(value.value[2]);
                    for (int i = 3; i < value.value.Count; i += 2)
                        attendanceData.listOfItems.Add(new ItemData(Convert.ToInt16(value.value[i]), Convert.ToInt16(value.value[i + 1])));

                    outSheetData.AttendanceDatas.Add(attendanceData);
                }
                return outSheetData;
            }
            return null;
        }

        public void GetAttendanceInformationFromFirstSheet(String sSpreadSheetID, Dictionary<int, AttendanceSheetData> sheetInformation)
        {
            if (m_serviceInstance != null)
            {
                var sheetInfo = m_serviceInstance.Spreadsheets.Get(sSpreadSheetID).Execute();
                if (sheetInfo == null) return;

                var firstElement = sheetInfo.Sheets.ElementAt(0);
                if (firstElement != null)
                {
                    var sheetInfoRequest = m_serviceInstance.Spreadsheets.Values.Get(sSpreadSheetID, firstElement.Properties.Title).Execute();
                    foreach (var value in sheetInfoRequest.Values.Select((value, index) => (value, index)))
                    {
                        if (value.index == 0) continue;
                        sheetInformation.Add(value.index, new AttendanceSheetData(Convert.ToInt16(value.value[0]), Convert.ToInt16(value.value[1]), value.value[2].ToString(), value.value[3].ToString(), value.value[4].ToString()));
                    }
                }
            }
        }

        public void GetEventInformation(String sSpreadSheetID, List<EventSheetData> eventInformation)
        {
            if (m_serviceInstance != null)
            {
                var sheetInfo = m_serviceInstance.Spreadsheets.Get(sSpreadSheetID).Execute();
                if(sheetInfo == null) return;

                foreach (var sheet in sheetInfo.Sheets)
                {
                    var sheetInfoRequest = m_serviceInstance.Spreadsheets.Values.Get(sSpreadSheetID, sheet.Properties.Title).Execute();

                    foreach (var value in sheetInfoRequest.Values.Select((value, index) => (value, index)))
                        eventInformation.Add(new EventSheetData(Convert.ToInt16(value.value[0]), Convert.ToString(value.value[1])));
                }
            }
        }


    }
}
