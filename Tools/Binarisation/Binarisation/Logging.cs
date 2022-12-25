using System;
using System.Threading;
using System.IO;
using System.Windows.Forms;

namespace Binarisation
{
	/// <summary>
	/// Summary description for Log.
	/// </summary>
	public class Logging

	{

        #region Variables
        private TextWriter      stream;
        private Thread          logThread;
        private RichTextWindow  window;
        private bool            fileLoggingEnabled = false;
        private bool            onTop;
        #endregion

        public Logging()
        {    
        }

        public Logging(string sFileName, bool bOnTop)
        {
            onTop = bOnTop;

            if (sFileName != null && 
                sFileName != string.Empty)
            {
                stream = TextWriter.Synchronized(new StreamWriter(sFileName));
                fileLoggingEnabled = true;
            }
            else
                fileLoggingEnabled = false;

            logThread = new Thread(new ThreadStart(HandleLogThread));
            logThread.Start();      
      
            // wait for logging thread to start
            while(window == null)
                Thread.Sleep(100);
        }
  
        ~Logging()
        {
            if (stream != null)
                stream.Close();
        }

        public void HandleLogThread()
        {
            window = new RichTextWindow();
            window.TopMost = onTop;
            window.Hide();
            Application.Run(window);
        }

        public void LogWithTimeStamp(string sText)
        {
            string logMsg = String.Format("[{0} {1}] {2}", System.DateTime.Now.ToString("dd-MM"), System.DateTime.Now.ToString("T"), sText);

            Log(logMsg);
        }

        public void Log(string sText)
        {
            sText = sText.Replace("\0", "\n\0");

            if (fileLoggingEnabled)
            {            
                // log message to file
                stream.WriteLine(sText);
                stream.Flush();
            }

            // log message to log window
            lock(window)
            {
                window.WriteLine(sText);
            }

        }

        public void Show(bool bShouldShow)
        {
            if (bShouldShow)
                window.Show();
            else 
                window.Hide();
        }

        public void OnTop(bool bOnTop)
        {
            window.TopMost = bOnTop;
        }
    }
}
