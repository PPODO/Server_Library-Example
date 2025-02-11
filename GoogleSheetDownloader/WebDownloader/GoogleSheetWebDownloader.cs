using System.Net;

namespace SheetDownloader.WebDownloader
{
    internal class GoogleSheetWebDownloader : WebClient
    {
        private readonly CookieContainer m_container = new CookieContainer();
        public GoogleSheetWebDownloader(CookieContainer container)
        {
            this.m_container = container;
        }
        public string DownloadCSVToPath(String sSpreadSheetID, String sSheetName, string sPath)
        {
            this.Headers.Add("User-Agent", "Mozilla/5.0 (Windows NT 6.2; WOW64; rv:22.0) Gecko/20100101 Firefox/22.0");
            this.Headers.Add("DNT", "1");
            this.Headers.Add("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
            this.Headers.Add("Accept-Encoding", "deflate");
            this.Headers.Add("Accept-Language", "en-US,en;q=0.5");

            var sFilePath = String.Format("{0}{1}.csv", sPath, sSheetName);
            this.DownloadFile(
                String.Format("https://docs.google.com/spreadsheets/d/{0}/gviz/tq?tqx=out:csv&sheet={1}", sSpreadSheetID, sSheetName),
                sFilePath);

            return sFilePath;
        }


        protected override WebRequest GetWebRequest(Uri address)
        {
            WebRequest r = base.GetWebRequest(address);
            var request = r as HttpWebRequest;
            if (request != null)
            {
                request.CookieContainer = m_container;
            }
            return r;
        }
        protected override WebResponse GetWebResponse(WebRequest request, IAsyncResult result)
        {
            WebResponse response = base.GetWebResponse(request, result);
            ReadCookies(response);
            return response;
        }
        protected override WebResponse GetWebResponse(WebRequest request)
        {
            WebResponse response = base.GetWebResponse(request);
            ReadCookies(response);
            return response;
        }
        private void ReadCookies(WebResponse r)
        {
            var response = r as HttpWebResponse;
            if (response != null)
            {
                CookieCollection cookies = response.Cookies;
                m_container.Add(cookies);
            }
        }
    }
}
