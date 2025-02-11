using System.Net;
using System.Net.Mail;
using System.Text;

namespace MailServer.GMail
{
    class GMailSender
    {
        private static string sFrom;
        private static string sAppPassword;

        static public void Initialize(string _sFrom, string _sAppPassword)
        {
            sFrom = _sFrom;
            sAppPassword = _sAppPassword;
        }

        static public bool SendMail(string sTo, string sSubject, string sBody, string? sAttachmentFileName = null)
        {
            MailMessage newMail = new();

            newMail.From = new MailAddress(sFrom, "Watch Dog");

            newMail.To.Add(sTo);

            newMail.IsBodyHtml = true;
            newMail.Subject = sSubject;
            newMail.Body = "<h1>" + sBody + "</h1>";
            if(sAttachmentFileName != null)
                newMail.Attachments.Add(new Attachment(sAttachmentFileName));

            SmtpClient client = new("smtp.gmail.com", 587)
            {
                Credentials = new NetworkCredential(sFrom, sAppPassword),
                EnableSsl = true,
            };

            try
            {
                client.Send(newMail);
                client.Dispose();
                newMail.Dispose();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
                return false;
            }
            Console.WriteLine("Email Has Been Successfully Sent! - To : {0}", sTo);
            return true;
        }
    }
}
