using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.IO;
using NTFS;

namespace GetKong
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Synchroniser : System.Windows.Forms.Form
	{
		private System.Windows.Forms.StatusBar FRMST_STATUSBAR;
		private System.Windows.Forms.CheckBox FRM_CHK_GETBFGUEST;
		private System.Windows.Forms.CheckBox FRM_CHK_GETBFDEMO;
		//private System.Windows.Forms.CheckBox FRM_CHK_MAKEBACKUP;
		private System.Windows.Forms.CheckBox FRM_CHK_COMPILE_AI;
		private System.ComponentModel.IContainer components;
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.ProgressBar FRMPRG_PROGRESS;
		private System.Windows.Forms.Button FRMBTN_OK;
		private System.Windows.Forms.CheckBox FRMCHK_GETBF;
		private System.Windows.Forms.CheckBox FRMCHK_AUTOLAUNCH;
		private System.Windows.Forms.CheckBox FRMCHK_GETJADE;
		private System.Windows.Forms.CheckBox FRMCHK_IMPORTSOUNDS;
		private System.Windows.Forms.RadioButton RADIO_CurrentBF;
		private System.Windows.Forms.RadioButton RADIO_ValidBF;
		private System.Windows.Forms.Timer Timer_VerifDate;

        private string m_strLockFile;
        //private string m_strBfChangelistFile;

		private string ValidVersion="";

		public Synchroniser()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(Synchroniser));
			this.FRMST_STATUSBAR = new System.Windows.Forms.StatusBar();
			this.FRM_CHK_GETBFGUEST = new System.Windows.Forms.CheckBox();
			this.FRM_CHK_GETBFDEMO = new System.Windows.Forms.CheckBox();
			this.FRM_CHK_COMPILE_AI = new System.Windows.Forms.CheckBox();
			this.button1 = new System.Windows.Forms.Button();
			this.panel1 = new System.Windows.Forms.Panel();
			this.RADIO_ValidBF = new System.Windows.Forms.RadioButton();
			this.RADIO_CurrentBF = new System.Windows.Forms.RadioButton();
			this.FRMCHK_GETJADE = new System.Windows.Forms.CheckBox();
			this.FRMCHK_IMPORTSOUNDS = new System.Windows.Forms.CheckBox();
			this.FRMCHK_AUTOLAUNCH = new System.Windows.Forms.CheckBox();
			this.FRMCHK_GETBF = new System.Windows.Forms.CheckBox();
			this.FRMBTN_OK = new System.Windows.Forms.Button();
			this.FRMPRG_PROGRESS = new System.Windows.Forms.ProgressBar();
			this.Timer_VerifDate = new System.Windows.Forms.Timer(this.components);
			this.panel1.SuspendLayout();
			this.SuspendLayout();
			// 
			// FRMST_STATUSBAR
			// 
			this.FRMST_STATUSBAR.Location = new System.Drawing.Point(0, 264);
			this.FRMST_STATUSBAR.Name = "FRMST_STATUSBAR";
			this.FRMST_STATUSBAR.Size = new System.Drawing.Size(298, 24);
			this.FRMST_STATUSBAR.SizingGrip = false;
			this.FRMST_STATUSBAR.TabIndex = 3;
			this.FRMST_STATUSBAR.Text = "Ready";
			// 
			// FRM_CHK_GETBFGUEST
			// 
			this.FRM_CHK_GETBFGUEST.BackColor = System.Drawing.SystemColors.ActiveBorder;
			this.FRM_CHK_GETBFGUEST.Location = new System.Drawing.Point(16, 176);
			this.FRM_CHK_GETBFGUEST.Name = "FRM_CHK_GETBFGUEST";
			this.FRM_CHK_GETBFGUEST.RightToLeft = System.Windows.Forms.RightToLeft.No;
			this.FRM_CHK_GETBFGUEST.Size = new System.Drawing.Size(128, 24);
			this.FRM_CHK_GETBFGUEST.TabIndex = 0;
			this.FRM_CHK_GETBFGUEST.Text = "Get BF ( Guest ) ";
			this.FRM_CHK_GETBFGUEST.Visible = false;
			// 
			// FRM_CHK_GETBFDEMO
			// 
			this.FRM_CHK_GETBFDEMO.BackColor = System.Drawing.SystemColors.ActiveBorder;
			this.FRM_CHK_GETBFDEMO.Location = new System.Drawing.Point(16, 200);
			this.FRM_CHK_GETBFDEMO.Name = "FRM_CHK_GETBFDEMO";
			this.FRM_CHK_GETBFDEMO.RightToLeft = System.Windows.Forms.RightToLeft.No;
			this.FRM_CHK_GETBFDEMO.Size = new System.Drawing.Size(128, 24);
			this.FRM_CHK_GETBFDEMO.TabIndex = 0;
			this.FRM_CHK_GETBFDEMO.Text = "Get BF ( DEMO ) ";
			this.FRM_CHK_GETBFDEMO.Visible = false;
			// 
			// FRM_CHK_COMPILE_AI
			// 
			this.FRM_CHK_COMPILE_AI.BackColor = System.Drawing.SystemColors.ActiveBorder;
			this.FRM_CHK_COMPILE_AI.Location = new System.Drawing.Point(152, 176);
			this.FRM_CHK_COMPILE_AI.Name = "FRM_CHK_COMPILE_AI";
			this.FRM_CHK_COMPILE_AI.Size = new System.Drawing.Size(128, 24);
			this.FRM_CHK_COMPILE_AI.TabIndex = 1;
			this.FRM_CHK_COMPILE_AI.Text = "Compile AI ";
			this.FRM_CHK_COMPILE_AI.Visible = false;
			// 
			// button1
			// 
			this.button1.Enabled = false;
			this.button1.Location = new System.Drawing.Point(240, 16);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(48, 24);
			this.button1.TabIndex = 6;
			this.button1.Text = "Test";
			this.button1.Visible = false;
			this.button1.Click += new System.EventHandler(this.button1_Click);
			// 
			// panel1
			// 
			this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.panel1.Controls.Add(this.RADIO_ValidBF);
			this.panel1.Controls.Add(this.RADIO_CurrentBF);
			this.panel1.Controls.Add(this.FRMCHK_GETJADE);
			this.panel1.Controls.Add(this.FRMCHK_IMPORTSOUNDS);
			this.panel1.Controls.Add(this.FRMCHK_AUTOLAUNCH);
			this.panel1.Controls.Add(this.FRMCHK_GETBF);
			this.panel1.Controls.Add(this.FRMBTN_OK);
			this.panel1.Controls.Add(this.FRMPRG_PROGRESS);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panel1.Location = new System.Drawing.Point(0, 152);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(298, 112);
			this.panel1.TabIndex = 7;
			// 
			// RADIO_ValidBF
			// 
			this.RADIO_ValidBF.CheckAlign = System.Drawing.ContentAlignment.TopLeft;
			this.RADIO_ValidBF.Enabled = false;
			this.RADIO_ValidBF.Location = new System.Drawing.Point(96, 48);
			this.RADIO_ValidBF.Name = "RADIO_ValidBF";
			this.RADIO_ValidBF.Size = new System.Drawing.Size(192, 32);
			this.RADIO_ValidBF.TabIndex = 12;
			this.RADIO_ValidBF.Text = "Last validated version";
			this.RADIO_ValidBF.TextAlign = System.Drawing.ContentAlignment.TopLeft;
			// 
			// RADIO_CurrentBF
			// 
			this.RADIO_CurrentBF.CheckAlign = System.Drawing.ContentAlignment.TopLeft;
			this.RADIO_CurrentBF.Checked = true;
			this.RADIO_CurrentBF.Enabled = false;
			this.RADIO_CurrentBF.Location = new System.Drawing.Point(96, 32);
			this.RADIO_CurrentBF.Name = "RADIO_CurrentBF";
			this.RADIO_CurrentBF.Size = new System.Drawing.Size(192, 17);
			this.RADIO_CurrentBF.TabIndex = 11;
			this.RADIO_CurrentBF.TabStop = true;
			this.RADIO_CurrentBF.Text = "Latest version (PROD)";
			this.RADIO_CurrentBF.TextAlign = System.Drawing.ContentAlignment.TopLeft;
			// 
			// FRMCHK_GETJADE
			// 
			this.FRMCHK_GETJADE.BackColor = System.Drawing.SystemColors.Control;
			this.FRMCHK_GETJADE.ForeColor = System.Drawing.SystemColors.ControlText;
			this.FRMCHK_GETJADE.Location = new System.Drawing.Point(2, 8);
			this.FRMCHK_GETJADE.Name = "FRMCHK_GETJADE";
			this.FRMCHK_GETJADE.Size = new System.Drawing.Size(70, 24);
			this.FRMCHK_GETJADE.TabIndex = 10;
			this.FRMCHK_GETJADE.Text = "Get jade";
			// 
			// FRMCHK_IMPORTSOUNDS
			// 
			this.FRMCHK_IMPORTSOUNDS.BackColor = System.Drawing.SystemColors.Control;
			this.FRMCHK_IMPORTSOUNDS.ForeColor = System.Drawing.SystemColors.ControlText;
			this.FRMCHK_IMPORTSOUNDS.Location = new System.Drawing.Point(2, 32);
			this.FRMCHK_IMPORTSOUNDS.Name = "FRMCHK_IMPORTSOUNDS";
			this.FRMCHK_IMPORTSOUNDS.Size = new System.Drawing.Size(102, 24);
			this.FRMCHK_IMPORTSOUNDS.TabIndex = 9;
			this.FRMCHK_IMPORTSOUNDS.Text = "Import sounds";
			// 
			// FRMCHK_AUTOLAUNCH
			// 
			this.FRMCHK_AUTOLAUNCH.BackColor = System.Drawing.SystemColors.Control;
			this.FRMCHK_AUTOLAUNCH.Location = new System.Drawing.Point(2, 56);
			this.FRMCHK_AUTOLAUNCH.Name = "FRMCHK_AUTOLAUNCH";
			this.FRMCHK_AUTOLAUNCH.Size = new System.Drawing.Size(86, 24);
			this.FRMCHK_AUTOLAUNCH.TabIndex = 8;
			this.FRMCHK_AUTOLAUNCH.Text = "Auto launch";
			// 
			// FRMCHK_GETBF
			// 
			this.FRMCHK_GETBF.BackColor = System.Drawing.SystemColors.Control;
			this.FRMCHK_GETBF.Location = new System.Drawing.Point(96, 8);
			this.FRMCHK_GETBF.Name = "FRMCHK_GETBF";
			this.FRMCHK_GETBF.RightToLeft = System.Windows.Forms.RightToLeft.No;
			this.FRMCHK_GETBF.Size = new System.Drawing.Size(192, 24);
			this.FRMCHK_GETBF.TabIndex = 7;
			this.FRMCHK_GETBF.Text = "Get BF";
			this.FRMCHK_GETBF.Click += new System.EventHandler(this.FRMCHK_GETBF_Click);
			// 
			// FRMBTN_OK
			// 
			this.FRMBTN_OK.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.FRMBTN_OK.Location = new System.Drawing.Point(8, 80);
			this.FRMBTN_OK.Name = "FRMBTN_OK";
			this.FRMBTN_OK.Size = new System.Drawing.Size(72, 24);
			this.FRMBTN_OK.TabIndex = 6;
			this.FRMBTN_OK.Text = "GO";
			this.FRMBTN_OK.Click += new System.EventHandler(this.FRMBTN_OK_Click);
			// 
			// FRMPRG_PROGRESS
			// 
			this.FRMPRG_PROGRESS.Location = new System.Drawing.Point(88, 83);
			this.FRMPRG_PROGRESS.Name = "FRMPRG_PROGRESS";
			this.FRMPRG_PROGRESS.Size = new System.Drawing.Size(200, 16);
			this.FRMPRG_PROGRESS.TabIndex = 5;
			// 
			// Timer_VerifDate
			// 
			this.Timer_VerifDate.Enabled = true;
			this.Timer_VerifDate.Interval = 60000;
			this.Timer_VerifDate.Tick += new System.EventHandler(this.Timer_VerifDate_Tick);
			// 
			// Synchroniser
			// 
			this.AutoScale = false;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("$this.BackgroundImage")));
			this.ClientSize = new System.Drawing.Size(298, 288);
			this.Controls.Add(this.panel1);
			this.Controls.Add(this.button1);
			this.Controls.Add(this.FRM_CHK_COMPILE_AI);
			this.Controls.Add(this.FRM_CHK_GETBFGUEST);
			this.Controls.Add(this.FRMST_STATUSBAR);
			this.Controls.Add(this.FRM_CHK_GETBFDEMO);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "Synchroniser";
			this.Text = "Get Rayman4 - V1.0";
			this.Load += new System.EventHandler(this.Synchroniser_Load);
			this.panel1.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Synchroniser());
		}

		private void FRMBTN_OK_Click(object sender, System.EventArgs e)
		{
			FRMBTN_OK.Enabled = false;
			m_strLockFile = "";
			string strDestination = @"X:\";
			FRMPRG_PROGRESS.Step = -100;
			FRMPRG_PROGRESS.PerformStep();

			FRMPRG_PROGRESS.Minimum = 0;
			FRMPRG_PROGRESS.Maximum = 0;
			FRMPRG_PROGRESS.Step = 1;

			if ( FRMCHK_GETBF.Checked ) 
			{
				FRMPRG_PROGRESS.Maximum += 5;
			}

			if ( FRMCHK_GETJADE.Checked ) 
			{
				FRMPRG_PROGRESS.Maximum += 15;

				//string strSource = @"\\srvdata2-mtp\Sally_Game\Sally_Tools\Jade\";
				string strSource = @"\\Srvdata4-mtp\Sally_game\Sally_Tools\Jade\";

				Copy(strSource,strDestination,"jade_edd.exe",false);
				FRMPRG_PROGRESS.PerformStep();
				Copy(strSource,strDestination,"jade_edr.exe",false);
				FRMPRG_PROGRESS.PerformStep();
				Copy(strSource,strDestination,"jade_edd_xe.exe",false);
				FRMPRG_PROGRESS.PerformStep();
				Copy(strSource,strDestination,"jade_edr_xe.exe",false);
				FRMPRG_PROGRESS.PerformStep();
				Copy(strSource,strDestination,"Jade_end.exe",false);
				FRMPRG_PROGRESS.PerformStep();
				Copy(strSource,strDestination,"Jade_enr.exe",false);
				FRMPRG_PROGRESS.PerformStep();
				Copy(strSource,strDestination,"jade.exe",false);
				FRMPRG_PROGRESS.PerformStep();
				Copy(strSource,strDestination,"mainedi.pdb",false);
				FRMPRG_PROGRESS.PerformStep();
				Copy(strSource,strDestination,"maineng.pdb",false);
				FRMPRG_PROGRESS.PerformStep();
				Copy(strSource,strDestination,"msvcr70.dll",false);
				FRMPRG_PROGRESS.PerformStep();
				Copy(strSource,strDestination,"msvcr71.dll",false);
				FRMPRG_PROGRESS.PerformStep();		
				Copy(strSource,strDestination,"BFObjectMerge.exe",false);
				FRMPRG_PROGRESS.PerformStep();	
				Copy(strSource,strDestination,"BFObjectMerge.ini",false);
				FRMPRG_PROGRESS.PerformStep();		
				Copy(strSource,strDestination,"BFObjectXmlConv_.exe",false);
				FRMPRG_PROGRESS.PerformStep();		
				Copy(strSource,strDestination,"ExamDiff.exe",false);
				FRMPRG_PROGRESS.PerformStep();	
			}

			if ( FRMCHK_GETBF.Checked && Lock() ) 
			{
				// avant
				//string strFilename = "KingKongThegame.bf";				
				//string strSource = @"\\Srvdata2-mtp\Sally_Game\KingKongGetBF\PS2-Production\";
				string strFilename = "Rayman4.bf";				
				//string strSource = @"D:\Jade\Tools\GetRayman\Temp\"; 
				string strSource = @"\\Srvdata3-mtp\Bigfile\Rayman4\rayman4GetBF\";

				if ( Copy(strSource,strDestination,strFilename,false)) //FRM_CHK_MAKEBACKUP.Checked) ) 
				{
					// Read changelist number of copied bf 
					/*int iChangelistNb = -1;

						if (File.Exists(m_strBfChangelistFile))
						{
							StreamReader SR = new StreamReader(m_strBfChangelistFile);
							System.String MyString = SR.ReadLine();
							iChangelistNb = System.Convert.ToInt32(MyString);
							SR.Close();

							if (iChangelistNb > 0)
							{
								// Flush so that "have list" in server is coherent with files copied from copied bf.
								iChangelistNb = BigfileP4Flush(iChangelistNb,strFilename);
							}
						}*/

					// Flush have list from bf to server.
					BigfileP4Flush(strFilename);

					// Sync with latest version
					FRMPRG_PROGRESS.PerformStep();
					
					BigfileP4Sync(strFilename, RADIO_CurrentBF.Checked);
					
					FRMPRG_PROGRESS.PerformStep();
					Unlock();

					if ( FRM_CHK_COMPILE_AI.Checked )
					{
						BigfileCompileAI(strFilename);
					}
					if ( FRMCHK_IMPORTSOUNDS.Checked ) 
					{
						BigfileImportSounds(strFilename);
					}	
				}
					// if we have not been able to copy, unlock 
				else 
				{
					Unlock();
				}
			}

			/*if ( FRM_CHK_GETBFGUEST.Checked && Lock() ) 
				{
					// pas encore mais sera utiliser plus tard
					string strFilename = "Rayman4_Guest.bf";				
					string strSource = @"\\Srvdata3-mtp\Bigfile\Rayman4\rayman4GetBF\Guest\";

					if ( Copy(strSource,strDestination,strFilename,false)) //FRM_CHK_MAKEBACKUP.Checked) ) 
					{
						FRMPRG_PROGRESS.PerformStep();
						BigfileP4Sync(true,strFilename);
						FRMPRG_PROGRESS.PerformStep();
						Unlock();

						if ( FRM_CHK_COMPILE_AI.Checked )
						{
							BigfileCompileAI(strFilename);
						}

						if ( FRM_CHK_IMPORTSOUNDS.Checked ) 
						{
							BigfileImportSounds(strFilename);
						}	
					}
					// if we have not been able to copy, unlock 
					else 
					{
						Unlock();
					}
				}

				if ( FRM_CHK_GETBFDEMO.Checked && Lock()) 
				{
					// pas encore de DEMO
					string strFilename = "Rayman4_PS2-Demo.bf";				
					string strSource = @"\\Srvdata3-mtp\Bigfile\Rayman4\rayman4GetBF\PS2-Demo\";

					if ( Copy(strSource,strDestination,strFilename,false)) //FRM_CHK_MAKEBACKUP.Checked) ) 
					{
						FRMPRG_PROGRESS.PerformStep();
						BigfileP4Sync(true,strFilename);
						FRMPRG_PROGRESS.PerformStep();
						Unlock();

						if ( FRM_CHK_COMPILE_AI.Checked )
						{
							BigfileCompileAI(strFilename);
						}

						if ( FRM_CHK_IMPORTSOUNDS.Checked ) 
						{
							BigfileImportSounds(strFilename);
						}				
					}
					// if we have not been able to copy, unlock 
					else 
					{
						Unlock();
					}
				}*/

			if(FRMCHK_AUTOLAUNCH.Checked)
			{
				AutoLaunch(@"Rayman4.bf");
			}

			FRMST_STATUSBAR.Text = "Ready";	
			FRMPRG_PROGRESS.Step = -100;
			FRMPRG_PROGRESS.PerformStep();

			System.Windows.Forms.DialogResult res = System.Windows.Forms.MessageBox.Show(this,"Do you wish to quit ?","DONE",System.Windows.Forms.MessageBoxButtons.YesNo);
		
			if ( res == System.Windows.Forms.DialogResult.Yes ) 
			{
				this.Close();
			}
			FRMBTN_OK.Enabled = true;
		}

		private bool Copy (	string	in_strSource,	string	in_strDestination,
			string	in_strFilename,	bool	in_bBackup			) 
		{
			if ( !File.Exists(in_strSource + in_strFilename) ) 
			{
				System.Windows.Forms.MessageBox.Show("Cannot access source file:" + in_strSource);
				FRMST_STATUSBAR.Text = "Cannot access source file:" + in_strSource ;
				return false; 
			}
			

			if ( in_bBackup ) 
			{
				MakeBackup(in_strDestination,in_strDestination,in_strFilename,1);
			}

			bool bRetVal = false;
			// Copy the file.
			try
			{
				ShellLib.ShellFileOperation ShellCommand = new ShellLib.ShellFileOperation();

				String[] source = new String[3];	
				String[] dest = new String[3];

				source[0] = in_strSource + in_strFilename;
				dest[0] = in_strDestination + in_strFilename;
			
				ShellCommand.Operation = ShellLib.ShellFileOperation.FileOperations.FO_COPY;
				ShellCommand.OperationFlags = ShellLib.ShellFileOperation.ShellFileOperationFlags.FOF_NOCONFIRMATION;
				ShellCommand.OwnerWindow = this.Handle;
				ShellCommand.SourceFiles = source;
				ShellCommand.DestFiles = dest;

				FRMST_STATUSBAR.Text = "Copying " + in_strFilename + " ... Please wait ...";
				bRetVal = ShellCommand.DoOperation();

				//				File.Copy(in_strSource + in_strFilename, in_strDestination + in_strFilename,true);
			}
			catch 
			{
				System.Windows.Forms.MessageBox.Show("Cannot copy file, make sure the file is not in memory: " + in_strFilename);
				FRMST_STATUSBAR.Text = "Cannot copy file " + in_strFilename ;
			}
			Console.WriteLine("{0} copied to {1}", in_strSource, in_strDestination);

			return bRetVal;
		}

		private bool MakeBackup(	string	in_strSource,
			string	in_strDestination,
			string in_strFilename, 
			int in_iBackupNumber)
		{
			string strDestinationCopy  = in_strDestination;
			if ( in_iBackupNumber > 0 ) 
				strDestinationCopy = in_strDestination + "AutoBackup" + in_iBackupNumber + @"\" ;

			if ( File.Exists(in_strSource + @"\" +  in_strFilename) )
				if ( File.Exists(strDestinationCopy + @"\" + in_strFilename) ) 
				{
					MakeBackup(strDestinationCopy ,in_strDestination,in_strFilename,in_iBackupNumber + 1 );
				}

			if ( File.Exists(in_strSource + @"\" +  in_strFilename) )
			{

				if ( in_iBackupNumber > 8 ) 
				{
					FRMST_STATUSBAR.Text = "Deleting backup 8";
					File.Delete(in_strSource + @"\" + in_strFilename);
				}
				else 
				{
					Directory.CreateDirectory(strDestinationCopy); 
					FRMST_STATUSBAR.Text = "Moving backup to " + strDestinationCopy + @"\" + in_strFilename ;
					File.Move(in_strSource + @"\" + in_strFilename ,strDestinationCopy + @"\" + in_strFilename);
					FileStreams NTFSStreams = new FileStreams(strDestinationCopy + @"\" + in_strFilename);
					FileStream NTFSStream = NTFSStreams["BFGUID.stream"].Open(FileMode.Truncate,FileAccess.Write);
					NTFSStream.Close();
				}
				FRMPRG_PROGRESS.PerformStep();
			}

			return true;
		}


        private bool LockFile(int _iFileNb)
        {
            bool bLock;
            string strLockFile;

            //Paths for lock files
            //avant 
            //string strLockFile1 = @"\\srvdata2-mtp\Sally_Game\KingKongGetBF\Lock_1.txt";
            //string strLockFile2 = @"\\srvdata2-mtp\Sally_Game\KingKongGetBF\Lock_2.txt";
            if (_iFileNb == 1)
                strLockFile = @"\\Srvdata3-mtp\Bigfile\Rayman4\rayman4GetBF\Lock_1.txt";
            else
                strLockFile = @"\\Srvdata3-mtp\Bigfile\Rayman4\rayman4GetBF\Lock_2.txt";

            FileInfo LockFile = new FileInfo(strLockFile);

            if (LockFile.Exists) 
            {
                // If file is too old, delete it.
                DateTime TimeNow = DateTime.Now;
                DateTime TimeFile = File.GetCreationTime(strLockFile);
                DateTime LimitFile = TimeFile.AddMinutes(15);
                bLock = (LimitFile.CompareTo(TimeNow) > 0);
                if (!bLock)
                    File.Delete(strLockFile);
            }
            else 
                bLock = false;

            if (!bLock)
            {
                StreamWriter SW;
                SW=File.CreateText(strLockFile);
                SW.WriteLine("CMP: " + System.Environment.GetEnvironmentVariable("COMPUTERNAME") + "  USER: " + System.Environment.GetEnvironmentVariable("USERNAME") + "  TIME: " + DateTime.Now);
                SW.Close();
                m_strLockFile = strLockFile;
            }
            return bLock;
        }

		private bool Lock ( ) 
		{
			bool bLock = false;

			//FileInfo LockFile1 = new FileInfo(strLockFile1);

			if (LockFile(1)) //lock file 1 exists
			{
				if (LockFile(2))
				{
					System.Windows.Forms.MessageBox.Show(this,"Too many requests. Please try again in a few minutes or contact your admins.","BLOCKED");
					bLock = false;
				}
				else	//The lock file does not exist, proceed
				{
					/*StreamWriter SW;
					SW=File.CreateText(strLockFile2);
					SW.WriteLine("CMP: " + System.Environment.GetEnvironmentVariable("COMPUTERNAME") + "  USER: " + System.Environment.GetEnvironmentVariable("USERNAME") + "  TIME: " + DateTime.Now);
					SW.Close();
					m_strLockFile = strLockFile2;*/
					bLock = true;
				}
			}
			else	//The lock file does not exist, proceed
			{
				/*StreamWriter SW;
				SW=File.CreateText(strLockFile1);
				SW.WriteLine("CMP: " + System.Environment.GetEnvironmentVariable("COMPUTERNAME") + "  USER: " + System.Environment.GetEnvironmentVariable("USERNAME") + "  TIME: " + DateTime.Now);
				SW.Close();	
				m_strLockFile = strLockFile1;*/
				bLock = true;
			}
			return bLock;
		}
		
		private bool Unlock()
		{
			//delete lock file
			if (File.Exists(m_strLockFile))
			{
				File.Delete(m_strLockFile);
			}
			return true;
		}


		private bool BigfileP4Sync(	string	in_strFilename, bool in_boolCurrent)
		{
			FRMST_STATUSBAR.Text = "Final sync. with Perforce";

			System.Diagnostics.Process proc = new System.Diagnostics.Process();
			proc.EnableRaisingEvents=false;
			proc.StartInfo.FileName = @"X:\Jade_edd.exe";
			if(in_boolCurrent)
			{
				proc.StartInfo.Arguments = @"X:\" + in_strFilename + " /sync:-1 /Z"; // Sync latest version
				FRMST_STATUSBAR.Text += " (Latest version)";
			}
			else
			{
				proc.StartInfo.Arguments = @"X:\" + in_strFilename + " /sync:"+ValidVersion+" /Z"; // Sync valid version
				FRMST_STATUSBAR.Text += " (Last valid ["+ValidVersion+"])";
			}

			if (!File.Exists(@"X:\Jade_edd.exe")) 
				MessageBox.Show(@"Cannot find X:\jade_edd.exe");

			if (!File.Exists(@"X:\" + in_strFilename)   )
				MessageBox.Show(@"Cannot find" + proc.StartInfo.Arguments);

			proc.Start();
			proc.WaitForExit();
			return true;
		}

        private void BigfileP4Flush(string in_strFilename)
        {
            FRMST_STATUSBAR.Text = "Flush have list to perforce server";

            System.Diagnostics.Process proc = new System.Diagnostics.Process();
            proc.EnableRaisingEvents=false;
            proc.StartInfo.FileName = @"X:\Jade_edd.exe";
            proc.StartInfo.Arguments = @"X:\" + in_strFilename + " /flush /Z";

            if (!File.Exists(@"X:\Jade_edd.exe")) 
            {
                MessageBox.Show(@"Cannot find X:\jade_edd.exe");
            }


            if (!File.Exists(@"X:\" + in_strFilename)   )
            {
                MessageBox.Show(@"Cannot find" + proc.StartInfo.Arguments);
            }

            proc.Start();
            proc.WaitForExit();
        }
        

		private bool AutoLaunch(string	in_strFilename)
		{
			FRMST_STATUSBAR.Text = "Launching Jade Engine";

			System.Diagnostics.Process proc = new System.Diagnostics.Process();
			proc.EnableRaisingEvents		= false;
			proc.StartInfo.FileName			= @"X:\Jade_enr.exe";
			proc.StartInfo.Arguments		= @"X:\" + in_strFilename;

			if (!File.Exists(proc.StartInfo.FileName)) 
				MessageBox.Show(@"Cannot find "+proc.StartInfo.FileName);

			if (!File.Exists(proc.StartInfo.Arguments)   )
				MessageBox.Show(@"Cannot find" + proc.StartInfo.Arguments);
			proc.Start();
			proc.WaitForExit();
			return true;
		}

		private bool BigfileCompileAI(	string	in_strFilename )
		{
			FRMST_STATUSBAR.Text = "Compiling AI";

			System.Diagnostics.Process proc = new System.Diagnostics.Process();
			proc.EnableRaisingEvents=false;
			proc.StartInfo.FileName = @"X:\Jade_edd.exe";
			proc.StartInfo.Arguments = "/Z /K[fr,en] /G1 /CPRODUCTION_Levels_Jack/03C_Hayes_is_back/03C_Hayes_is_back.wol" + @" X:\" + in_strFilename ;

			if (!File.Exists(@"X:\Jade_edd.exe")) 
				MessageBox.Show(@"Cannot find X:\jade_edd.exe");

			if (!File.Exists(@"X:\" + in_strFilename)   )
				MessageBox.Show(@"Cannot find" + proc.StartInfo.Arguments);

			proc.Start();
			proc.WaitForExit();
			return true;
		}

		private bool BigfileImportSounds( string in_strFilename )
		{
			FRMST_STATUSBAR.Text = "Importing PC sounds";

			System.Diagnostics.Process proc = new System.Diagnostics.Process();
			proc.EnableRaisingEvents=false;
			proc.StartInfo.FileName = @"X:\Jade_edd.exe";
			//prochain chemin S:\SOUND\Audio_Bank\Audio_Bank_PC_XBOX
			proc.StartInfo.Arguments = @"X:\" + in_strFilename + @" /import(\\Srvdata4-mtp\Rayman_4\SOUND\Audio_Bank\Audio_Bank_PC_XBOX)to(ROOT/EngineDatas/05 Audio Bank)";

			if (!File.Exists(@"X:\Jade_edd.exe")) 
				MessageBox.Show(@"Cannot find X:\jade_edd.exe");

			if (!File.Exists(@"X:\" + in_strFilename)   )
				MessageBox.Show(@"Cannot find" + proc.StartInfo.Arguments);

			proc.Start();
			proc.WaitForExit();
			return true;
		}


		private string GetValidVersion()
		{
			string in_strFilename = "\\\\srvdata3-mtp\\Bigfile\\Rayman4\\_DO_NOT_DELETE_Bigfile_Ref\\DesdacOK_CL.txt";
			
			if (!File.Exists(in_strFilename) )
			{
				MessageBox.Show(@"Cannot find " + in_strFilename);
				return "-1";
			}
			
			string resultat = "-1";
			try
			{
				using (StreamReader sr = new StreamReader(in_strFilename)) 
				{
					String line;
					if ((line = sr.ReadLine()) != null)
						resultat = line;
					sr.Close();
				}
			}
			catch (Exception e) 
			{
				Console.WriteLine("The file could not be read:");
				Console.WriteLine(e.Message);
			}
			return resultat;
		}

		private void button1_Click(object sender, System.EventArgs e)
		{
			Console.WriteLine("Valeur : "+GetValidVersion());
		}

		private void FRMCHK_GETBF_Click(object sender, System.EventArgs e)
		{
			RADIO_CurrentBF.Enabled	= FRMCHK_GETBF.Checked;
			RADIO_ValidBF.Enabled	= FRMCHK_GETBF.Checked;
			Timer_VerifDate.Enabled	= FRMCHK_GETBF.Checked;
		}

		private string GetFileDate( string in_strFilename )
		{
			System.IO.FileInfo _fi = new FileInfo(in_strFilename);
			return _fi.LastWriteTime.ToString();
		}

		private void Synchroniser_Load(object sender, System.EventArgs e)
		{
			string DateFichier	= GetFileDate("\\\\srvdata3-mtp\\Bigfile\\Rayman4\\_DO_NOT_DELETE_Bigfile_Ref\\DesdacOK_CL.txt");
			ValidVersion		= GetValidVersion();
			RADIO_ValidBF.Text	= "Last validated version [CL "+ValidVersion+"] ("+DateFichier+")";
		}

		private void Timer_VerifDate_Tick(object sender, System.EventArgs e)
		{
			string DateFichier	= GetFileDate("\\\\srvdata3-mtp\\Bigfile\\Rayman4\\_DO_NOT_DELETE_Bigfile_Ref\\DesdacOK_CL.txt");
			ValidVersion		= GetValidVersion();
			RADIO_ValidBF.Text	= "Last validated version [CL "+ValidVersion+"] ("+DateFichier+")";
		}
	}
}
