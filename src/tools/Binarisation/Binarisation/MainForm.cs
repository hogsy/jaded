using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Threading;
using System.Windows.Forms;
using System.Data;
using System.IO;

namespace Binarisation
{
    /// <summary>
    /// Summary description for Form1.
    /// </summary>
    public class MainForm : System.Windows.Forms.Form
    {
        CmdLineParams cmdLineParams;
        Thread mainThread;
        Thread monitorThread;
        string configFileName;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox textBoxEditor;
        private System.Windows.Forms.Button buttonBrowseEditor;
        private System.Windows.Forms.Button buttonBrowseEditorWithAI2C;
        private System.Windows.Forms.TextBox textBoxEditorWithAI2C;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label labelEditorWarning;
        private System.Windows.Forms.Label labelEditorWithAI2CWarning;
        private System.Windows.Forms.CheckBox checkBoxLogOnTop;
        private System.Windows.Forms.CheckBox checkBoxBuildReleaseXProfile;
        private System.Windows.Forms.RadioButton buttonBinUS;
        private System.Windows.Forms.RadioButton buttonBinJapan;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox textBoxLanguages;
		private System.Windows.Forms.Label label10;
		private System.Windows.Forms.ComboBox comboBoxMapList;
		private System.Windows.Forms.Label label2;

        internal class CmdLineParams
        {
            public CmdLineParams(string[] args)
            {
                string errMsg = "";

                for(int i = 0; i < args.Length; i++)
                {            
                    switch(args[i].ToLower())
                    {
                        case @"-?":
                        case @"--help":
                        case @"-help":
                            Usage();
                            break;

                        case @"-sync":
                            i++;
                            if (i < args.Length)
                                syncTo = Int32.Parse(args[i]);
                            else
                                errMsg += "ERROR: Missing changelist number\n";

                            if (syncTo <= 0)
                                errMsg += "ERROR: Invalid changelist number\n";
                            break;

                        case @"-bfpath":
                            i++;
                            if (i < args.Length)
                                bfPath = args[i];
                            else
                                errMsg += "ERROR: Missing BF Path\n";
                            break;

                        case @"-ini":
                            i++;
                            if (i < args.Length)
                                loadIni = args[i];
                            else
                                errMsg += "ERROR: Missing INI File Path\n";
                            break;

                        case @"-auto":
                            autoRun = true;
                            break;

                        case @"-quiet":
                            quiet = true;
                            break;

                        case @"-log":
                            i++;
                            if (i < args.Length)
                                logFileName = args[i];
                            else
                                errMsg += "ERROR: Missing filename\n";
                            break;

                        case @"-report":
                            i++;
                            if (i < args.Length)
                                logDetailsFileName = args[i];
                            else
                                errMsg += "ERROR: Missing filename\n";
                            break;

                        default:
                            errMsg += String.Format("Invalid parameter : {0}\n", args[i]);
                            Usage();
                            break;

                    }

                    if (errMsg.Length > 0)                
                        throw(new ArgumentException(errMsg));
                }
            }

            private void Usage()
            {
                   String msg = "Usage: Binarisation.exe [-sync <changelistNumber>] [-bfpath <bigFilePath>] [-ini <iniFilePath>] [-auto] [-quiet]\n" +
                                "\t-sync   <changelistNumber>\t: Sync code to that changelist\n" +
                                "\t-bfpath <bigFilePath>     \t: Override ini file BF Path\n" +
                                "\t-ini    <iniFilePath>     \t: Load ini file at startup\n" +
                                "\t-auto                   \t\t: Start bin automatically, and quit when its done\n" +
                                "\t-log    <fileName>        \t: Save log to <fileName>\n" +
                                "\t-report <fileName>        \t: Save detail log to <fileName>\n" +
                                "\t-quiet                  \t\t: No pop-up!\n";
;                
                   MessageBox.Show(msg, "USAGE",MessageBoxButtons.OK,MessageBoxIcon.Warning);
             }

            private int    syncTo = 0;
            public  int    SyncTo  { get{ return syncTo; } }
			private string bfPath;
			public  string BFPath  
			{
				get{ 
					if (bfPath != null)
						return bfPath; 
					else 
						return "";
				}
			}

			private string loadIni;
			public  string LoadIni 
			{
				get{ 
					if (loadIni != null) 
						return loadIni; 
					else 
						return "";
				} 
			}

            private bool   autoRun = false;
            public  bool   AutoRun { get{ return autoRun; } }
            private bool   quiet   = false;
            public  bool   Quiet   { get{ return quiet; } }
            private string logFileName;
            public  string LogFileName { get { return logFileName; } }
            private string logDetailsFileName;
            public  string LogDetailsFileName { get { return logDetailsFileName; } }
        }


		public MainForm(string[] args)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
			
			#region BinConfigArrayDeclaration

            // TODO complete this section
            SetupTag = new String[] { 
                        "[CodeSetup-Source]",
						"[CodeOption-SyncChangelistNumber]",
                        "[DataSetup-MainBF]",
						"[DataSetup-MapList]", 
                        "[ExecSetup-BinWorkFolder]",
						"[ExecSetup-MSVSpath]",
                        "[EditorLocation]",
                        "[EditorWithAI2CLocation]",
                        "[Languages]" };

            SetupBox = new System.Windows.Forms.TextBox[] { 
                        textBoxCodeLocation,
						textBoxPerforceCodeChangelistNumber,
                        textBoxBigFile,
						textBoxMapList,
                        textBoxBinDir,
						textBoxDevEnv,
						textBoxEditor,
						textBoxEditorWithAI2C, 
                        textBoxLanguages };

			OptionTag = new String[] { 
                        "[CodeOption-Sync]", 
                        "[CodeOption-BuildTargetDebugEditors]", 
                        "[CodeOption-BuildTargetSpeedEditors]", 
                        "[CodeOption-BuildTargetDebugXenon]", 
                        "[CodeOption-BuildTargetReleaseXenon]", 
                        "[CodeOption-BuildTargetFinalXenon]", 
                        "[CodeOption-BuildTargetReleaseXProfile]", 
                        "[CodeOption-AI2C]",
                        "[DataOption-GetBF]",
                        "[DataOption-CompileAI]",
                        "[DataOption-Preprocess]",
                        "[DataOption-DeleteAllBinData]",
                        "[DataOption-BinAll]",
                        "[DataOption-Clean]",
                        "[ExecOption-Clean]",
                        "[LogOnTop]"};

            OptionBox = new System.Windows.Forms.CheckBox[] { 
                        checkBoxSyncPerforce, 
                        checkBoxBuildDebugEditors,
                        checkBoxBuildSpeedEditors,
                        checkBoxBuildDebugXenon,
                        checkBoxBuildReleaseXenon,
                        checkBoxBuildFinalXenon,
                        checkBoxBuildReleaseXProfile,                                                                
                        checkBoxCompileCAI,
                        checkBoxGetBF,
                        checkBoxCompileAI,
                        checkBoxPPMaps,
                        checkBoxDeleteAllBin,
                        checkBoxBinAll,
                        checkBoxCleanBF,
                        checkBoxCleanDir, 
                        checkBoxLogOnTop};

            ButtonTag = new String[] { 
                        "[CodeOption-SyncToHead]", 
                        "[CodeOption-SyncToChange]" };

            OptionButton = new System.Windows.Forms.RadioButton[] {
                        buttonPerforceSyncToHead,
                        buttonPerforceSyncToChangelist };       
            
            MainFormData = new string[] { 
                        "[MainForm-Size-Height]",
                        "[MainForm-Size-Width]" };

			

            #endregion
			
            // Reset Form to valid state (handle menu item states, radio buttons, ...)
            InitializeForm();

            try
            {         
                cmdLineParams = new CmdLineParams(args);

                // Set config file name
                if (cmdLineParams.LoadIni != String.Empty)                                   
                    configFileName = cmdLineParams.LoadIni;                
                else
                    configFileName = Path.GetDirectoryName(Application.ExecutablePath) + @"\binarisation.ini";                    

                // Initialize interface to saved config
                ReadConfigFile(configFileName);     
                string commonFileName = Path.GetDirectoryName(configFileName) + @"\common.ini";
                ReadConfigFile(commonFileName);

                // Fill Map List box
                if (textBoxMapList.Text != null)
                {
                    if (File.Exists(textBoxMapList.Text))
                        FillMapListBox(false);
                }

                HandleCmdLineParams();

                if (cmdLineParams.AutoRun)
                    menuItemRun.PerformClick();
            }

            catch(Exception e)
            {
                if (args.Length > 0)
                    Console.WriteLine(e.Message);
                else
                    MessageBox.Show(e.Message,"ERROR",MessageBoxButtons.OK,MessageBoxIcon.Error);
                menuItemQuit.PerformClick();
            }
      	}

        private void HandleCmdLineParams()
        {
            if (cmdLineParams.SyncTo > 0)
            {
                textBoxPerforceCodeChangelistNumber.Text = cmdLineParams.SyncTo.ToString();
                checkBoxSyncPerforce.Checked = true;
                buttonPerforceSyncToChangelist.Checked = true;
            }

            if (cmdLineParams.BFPath != String.Empty)
                textBoxBigFile.Text = cmdLineParams.BFPath;
        }

        private void InitializeForm()
        {
            // set default state for radio buttons
            checkBoxSyncPerforce_CheckedChanged(this, null);
        }

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
        private System.Windows.Forms.TabPage tabMaps;
        private System.Windows.Forms.TextBox textBoxMapList;
        private System.Windows.Forms.Button buttonBrowseMapList;
        private System.Windows.Forms.Label labelMapList;
        private System.Windows.Forms.CheckedListBox listBoxMapList;
        private System.Windows.Forms.Button buttonGenerateMapList;
        private System.Windows.Forms.Button buttonSaveMapList;
        private System.Windows.Forms.CheckBox checkBoxDeleteAllBin;
        private System.Windows.Forms.Button buttonMapListToggleAll;
        private System.Windows.Forms.Button buttonLoadMapList;
        private System.Windows.Forms.MenuItem menuItemShowLogs;
        private System.Windows.Forms.CheckBox checkBoxCompileCAI;
        private System.Windows.Forms.TextBox textBoxPerforceCodeChangelistNumber;
        private System.Windows.Forms.CheckBox checkBoxSyncPerforce;
        private System.Windows.Forms.RadioButton buttonPerforceSyncToHead;
        private System.Windows.Forms.RadioButton buttonPerforceSyncToChangelist;
        private System.Windows.Forms.CheckBox checkBoxBuildReleaseXenon;
        private System.Windows.Forms.CheckBox checkBoxBuildDebugEditors;
        private System.Windows.Forms.CheckBox checkBoxBuildSpeedEditors;
        private System.Windows.Forms.CheckBox checkBoxBuildDebugXenon;
        private System.Windows.Forms.TextBox textBoxBinDir;
        private System.Windows.Forms.Label labelBinDir;
        private System.Windows.Forms.Button buttonBrowseBinRootDir;
        private System.Windows.Forms.Button buttonBrowseMSDev;
        private System.Windows.Forms.TextBox textBoxDevEnv;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox checkBoxBuildFinalXenon;
        private System.Windows.Forms.MainMenu mainMenu1;
        private System.Windows.Forms.MenuItem menuItem4;
        private System.Windows.Forms.MenuItem menuItemFile;
        private System.Windows.Forms.MenuItem menuItemLoadConfig;
        private System.Windows.Forms.MenuItem menuItemSaveConfig;
        private System.Windows.Forms.MenuItem menuItemQuit;
        private System.Windows.Forms.MenuItem menuItemSaveConfigAs;
        private System.Windows.Forms.TabControl tabControl;
        private System.Windows.Forms.MenuItem menuItemAction;
        private System.Windows.Forms.MenuItem menuItemRun;
        private System.Windows.Forms.MenuItem menuItemCancel;
        private System.Windows.Forms.TabPage tabSetup;
        private System.Windows.Forms.GroupBox groupBoxEnvSetup;
        private System.Windows.Forms.GroupBox groupCodeSetup;
        private System.Windows.Forms.TextBox textBoxCodeLocation;
        private System.Windows.Forms.Label labelp4;
        private System.Windows.Forms.Button buttonBrowseJadeDir;
        private System.Windows.Forms.GroupBox groupDataSetup;
        private System.Windows.Forms.TextBox textBoxBigFile;
        private System.Windows.Forms.Button buttonBrowseBF;
        private System.Windows.Forms.Label labelBF;
        private System.Windows.Forms.TabPage tabSteps;
        private System.Windows.Forms.MenuItem menuItem3;
        private System.Windows.Forms.CheckBox checkBoxCleanBF;
        private System.Windows.Forms.CheckBox checkBoxBinAll;
        private System.Windows.Forms.CheckBox checkBoxPPMaps;
        private System.Windows.Forms.CheckBox checkBoxCompileAI;
        private System.Windows.Forms.CheckBox checkBoxGetBF;
        private System.Windows.Forms.CheckBox checkBoxCleanDir;
        private System.Windows.Forms.StatusBar statusBar;
        private System.ComponentModel.Container components = null;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.tabSteps = new System.Windows.Forms.TabPage();
			this.panel2 = new System.Windows.Forms.Panel();
			this.textBoxLanguages = new System.Windows.Forms.TextBox();
			this.label9 = new System.Windows.Forms.Label();
			this.label8 = new System.Windows.Forms.Label();
			this.label7 = new System.Windows.Forms.Label();
			this.buttonBinJapan = new System.Windows.Forms.RadioButton();
			this.buttonBinUS = new System.Windows.Forms.RadioButton();
			this.panel1 = new System.Windows.Forms.Panel();
			this.textBoxPerforceCodeChangelistNumber = new System.Windows.Forms.TextBox();
			this.buttonPerforceSyncToHead = new System.Windows.Forms.RadioButton();
			this.buttonPerforceSyncToChangelist = new System.Windows.Forms.RadioButton();
			this.checkBoxBuildReleaseXProfile = new System.Windows.Forms.CheckBox();
			this.labelEditorWithAI2CWarning = new System.Windows.Forms.Label();
			this.labelEditorWarning = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.checkBoxCleanDir = new System.Windows.Forms.CheckBox();
			this.checkBoxSyncPerforce = new System.Windows.Forms.CheckBox();
			this.checkBoxBuildDebugEditors = new System.Windows.Forms.CheckBox();
			this.checkBoxBuildSpeedEditors = new System.Windows.Forms.CheckBox();
			this.checkBoxCompileCAI = new System.Windows.Forms.CheckBox();
			this.label2 = new System.Windows.Forms.Label();
			this.checkBoxGetBF = new System.Windows.Forms.CheckBox();
			this.checkBoxCompileAI = new System.Windows.Forms.CheckBox();
			this.checkBoxPPMaps = new System.Windows.Forms.CheckBox();
			this.checkBoxDeleteAllBin = new System.Windows.Forms.CheckBox();
			this.checkBoxBinAll = new System.Windows.Forms.CheckBox();
			this.checkBoxCleanBF = new System.Windows.Forms.CheckBox();
			this.checkBoxBuildDebugXenon = new System.Windows.Forms.CheckBox();
			this.checkBoxBuildReleaseXenon = new System.Windows.Forms.CheckBox();
			this.checkBoxBuildFinalXenon = new System.Windows.Forms.CheckBox();
			this.tabControl = new System.Windows.Forms.TabControl();
			this.tabSetup = new System.Windows.Forms.TabPage();
			this.checkBoxLogOnTop = new System.Windows.Forms.CheckBox();
			this.groupDataSetup = new System.Windows.Forms.GroupBox();
			this.label6 = new System.Windows.Forms.Label();
			this.buttonBrowseEditorWithAI2C = new System.Windows.Forms.Button();
			this.textBoxEditorWithAI2C = new System.Windows.Forms.TextBox();
			this.label5 = new System.Windows.Forms.Label();
			this.buttonBrowseEditor = new System.Windows.Forms.Button();
			this.textBoxEditor = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.textBoxBigFile = new System.Windows.Forms.TextBox();
			this.buttonBrowseBF = new System.Windows.Forms.Button();
			this.labelBF = new System.Windows.Forms.Label();
			this.groupCodeSetup = new System.Windows.Forms.GroupBox();
			this.textBoxCodeLocation = new System.Windows.Forms.TextBox();
			this.labelp4 = new System.Windows.Forms.Label();
			this.buttonBrowseJadeDir = new System.Windows.Forms.Button();
			this.groupBoxEnvSetup = new System.Windows.Forms.GroupBox();
			this.textBoxBinDir = new System.Windows.Forms.TextBox();
			this.labelBinDir = new System.Windows.Forms.Label();
			this.buttonBrowseBinRootDir = new System.Windows.Forms.Button();
			this.buttonBrowseMSDev = new System.Windows.Forms.Button();
			this.textBoxDevEnv = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.tabMaps = new System.Windows.Forms.TabPage();
			this.label10 = new System.Windows.Forms.Label();
			this.comboBoxMapList = new System.Windows.Forms.ComboBox();
			this.buttonLoadMapList = new System.Windows.Forms.Button();
			this.buttonMapListToggleAll = new System.Windows.Forms.Button();
			this.buttonSaveMapList = new System.Windows.Forms.Button();
			this.listBoxMapList = new System.Windows.Forms.CheckedListBox();
			this.buttonGenerateMapList = new System.Windows.Forms.Button();
			this.labelMapList = new System.Windows.Forms.Label();
			this.textBoxMapList = new System.Windows.Forms.TextBox();
			this.buttonBrowseMapList = new System.Windows.Forms.Button();
			this.mainMenu1 = new System.Windows.Forms.MainMenu();
			this.menuItemFile = new System.Windows.Forms.MenuItem();
			this.menuItemLoadConfig = new System.Windows.Forms.MenuItem();
			this.menuItemSaveConfig = new System.Windows.Forms.MenuItem();
			this.menuItemSaveConfigAs = new System.Windows.Forms.MenuItem();
			this.menuItem4 = new System.Windows.Forms.MenuItem();
			this.menuItemQuit = new System.Windows.Forms.MenuItem();
			this.menuItemShowLogs = new System.Windows.Forms.MenuItem();
			this.menuItemAction = new System.Windows.Forms.MenuItem();
			this.menuItemRun = new System.Windows.Forms.MenuItem();
			this.menuItem3 = new System.Windows.Forms.MenuItem();
			this.menuItemCancel = new System.Windows.Forms.MenuItem();
			this.statusBar = new System.Windows.Forms.StatusBar();
			this.tabSteps.SuspendLayout();
			this.panel2.SuspendLayout();
			this.panel1.SuspendLayout();
			this.tabControl.SuspendLayout();
			this.tabSetup.SuspendLayout();
			this.groupDataSetup.SuspendLayout();
			this.groupCodeSetup.SuspendLayout();
			this.groupBoxEnvSetup.SuspendLayout();
			this.tabMaps.SuspendLayout();
			this.SuspendLayout();
			// 
			// tabSteps
			// 
			this.tabSteps.Controls.Add(this.panel2);
			this.tabSteps.Controls.Add(this.panel1);
			this.tabSteps.Controls.Add(this.checkBoxBuildReleaseXProfile);
			this.tabSteps.Controls.Add(this.labelEditorWithAI2CWarning);
			this.tabSteps.Controls.Add(this.labelEditorWarning);
			this.tabSteps.Controls.Add(this.label3);
			this.tabSteps.Controls.Add(this.checkBoxCleanDir);
			this.tabSteps.Controls.Add(this.checkBoxSyncPerforce);
			this.tabSteps.Controls.Add(this.checkBoxBuildDebugEditors);
			this.tabSteps.Controls.Add(this.checkBoxBuildSpeedEditors);
			this.tabSteps.Controls.Add(this.checkBoxCompileCAI);
			this.tabSteps.Controls.Add(this.label2);
			this.tabSteps.Controls.Add(this.checkBoxGetBF);
			this.tabSteps.Controls.Add(this.checkBoxCompileAI);
			this.tabSteps.Controls.Add(this.checkBoxPPMaps);
			this.tabSteps.Controls.Add(this.checkBoxDeleteAllBin);
			this.tabSteps.Controls.Add(this.checkBoxBinAll);
			this.tabSteps.Controls.Add(this.checkBoxCleanBF);
			this.tabSteps.Controls.Add(this.checkBoxBuildDebugXenon);
			this.tabSteps.Controls.Add(this.checkBoxBuildReleaseXenon);
			this.tabSteps.Controls.Add(this.checkBoxBuildFinalXenon);
			this.tabSteps.Location = new System.Drawing.Point(4, 22);
			this.tabSteps.Name = "tabSteps";
			this.tabSteps.Size = new System.Drawing.Size(640, 606);
			this.tabSteps.TabIndex = 0;
			this.tabSteps.Text = "Steps";
			// 
			// panel2
			// 
			this.panel2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.panel2.Controls.Add(this.textBoxLanguages);
			this.panel2.Controls.Add(this.label9);
			this.panel2.Controls.Add(this.label8);
			this.panel2.Controls.Add(this.label7);
			this.panel2.Controls.Add(this.buttonBinJapan);
			this.panel2.Controls.Add(this.buttonBinUS);
			this.panel2.Location = new System.Drawing.Point(400, 8);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(224, 160);
			this.panel2.TabIndex = 42;
			// 
			// textBoxLanguages
			// 
			this.textBoxLanguages.Location = new System.Drawing.Point(16, 96);
			this.textBoxLanguages.Name = "textBoxLanguages";
			this.textBoxLanguages.Size = new System.Drawing.Size(192, 20);
			this.textBoxLanguages.TabIndex = 44;
			this.textBoxLanguages.Text = "";
			// 
			// label9
			// 
			this.label9.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label9.Location = new System.Drawing.Point(16, 72);
			this.label9.Name = "label9";
			this.label9.TabIndex = 43;
			this.label9.Text = "Languages";
			// 
			// label8
			// 
			this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label8.Location = new System.Drawing.Point(88, 16);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(56, 16);
			this.label8.TabIndex = 42;
			this.label8.Text = "Preset";
			// 
			// label7
			// 
			this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label7.Location = new System.Drawing.Point(16, 16);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(56, 16);
			this.label7.TabIndex = 41;
			this.label7.Text = "Version";
			// 
			// buttonBinJapan
			// 
			this.buttonBinJapan.Location = new System.Drawing.Point(144, 32);
			this.buttonBinJapan.Name = "buttonBinJapan";
			this.buttonBinJapan.TabIndex = 40;
			this.buttonBinJapan.Text = "Japan";
			this.buttonBinJapan.CheckedChanged += new System.EventHandler(this.buttonBinJapan_CheckedChanged);
			// 
			// buttonBinUS
			// 
			this.buttonBinUS.Location = new System.Drawing.Point(88, 32);
			this.buttonBinUS.Name = "buttonBinUS";
			this.buttonBinUS.TabIndex = 39;
			this.buttonBinUS.Text = "US";
			this.buttonBinUS.CheckedChanged += new System.EventHandler(this.buttonBinUS_CheckedChanged);
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.textBoxPerforceCodeChangelistNumber);
			this.panel1.Controls.Add(this.buttonPerforceSyncToHead);
			this.panel1.Controls.Add(this.buttonPerforceSyncToChangelist);
			this.panel1.Location = new System.Drawing.Point(80, 88);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(216, 64);
			this.panel1.TabIndex = 41;
			// 
			// textBoxPerforceCodeChangelistNumber
			// 
			this.textBoxPerforceCodeChangelistNumber.Location = new System.Drawing.Point(96, 32);
			this.textBoxPerforceCodeChangelistNumber.Name = "textBoxPerforceCodeChangelistNumber";
			this.textBoxPerforceCodeChangelistNumber.Size = new System.Drawing.Size(72, 20);
			this.textBoxPerforceCodeChangelistNumber.TabIndex = 14;
			this.textBoxPerforceCodeChangelistNumber.Text = "";
			// 
			// buttonPerforceSyncToHead
			// 
			this.buttonPerforceSyncToHead.Location = new System.Drawing.Point(8, 8);
			this.buttonPerforceSyncToHead.Name = "buttonPerforceSyncToHead";
			this.buttonPerforceSyncToHead.Size = new System.Drawing.Size(80, 16);
			this.buttonPerforceSyncToHead.TabIndex = 12;
			this.buttonPerforceSyncToHead.Text = "Head";
			this.buttonPerforceSyncToHead.CheckedChanged += new System.EventHandler(this.buttonPerforceSyncToHead_CheckedChanged);
			// 
			// buttonPerforceSyncToChangelist
			// 
			this.buttonPerforceSyncToChangelist.Location = new System.Drawing.Point(8, 32);
			this.buttonPerforceSyncToChangelist.Name = "buttonPerforceSyncToChangelist";
			this.buttonPerforceSyncToChangelist.Size = new System.Drawing.Size(80, 16);
			this.buttonPerforceSyncToChangelist.TabIndex = 13;
			this.buttonPerforceSyncToChangelist.Text = "Changelist";
			this.buttonPerforceSyncToChangelist.CheckedChanged += new System.EventHandler(this.buttonPerforceSyncToChangelist_CheckedChanged);
			// 
			// checkBoxBuildReleaseXProfile
			// 
			this.checkBoxBuildReleaseXProfile.Location = new System.Drawing.Point(312, 376);
			this.checkBoxBuildReleaseXProfile.Name = "checkBoxBuildReleaseXProfile";
			this.checkBoxBuildReleaseXProfile.Size = new System.Drawing.Size(120, 24);
			this.checkBoxBuildReleaseXProfile.TabIndex = 38;
			this.checkBoxBuildReleaseXProfile.Text = "ReleaseXProfile";
			// 
			// labelEditorWithAI2CWarning
			// 
			this.labelEditorWithAI2CWarning.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Italic))), System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.labelEditorWithAI2CWarning.Location = new System.Drawing.Point(344, 192);
			this.labelEditorWithAI2CWarning.Name = "labelEditorWithAI2CWarning";
			this.labelEditorWithAI2CWarning.Size = new System.Drawing.Size(200, 48);
			this.labelEditorWithAI2CWarning.TabIndex = 37;
			// 
			// labelEditorWarning
			// 
			this.labelEditorWarning.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Italic))), System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.labelEditorWarning.Location = new System.Drawing.Point(240, 156);
			this.labelEditorWarning.Name = "labelEditorWarning";
			this.labelEditorWarning.Size = new System.Drawing.Size(304, 16);
			this.labelEditorWarning.TabIndex = 36;
			// 
			// label3
			// 
			this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label3.Location = new System.Drawing.Point(136, 280);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(368, 24);
			this.label3.TabIndex = 35;
			this.label3.Text = "If you uncheck, you can re-bin some maps (and keep the others)";
			// 
			// checkBoxCleanDir
			// 
			this.checkBoxCleanDir.Location = new System.Drawing.Point(16, 8);
			this.checkBoxCleanDir.Name = "checkBoxCleanDir";
			this.checkBoxCleanDir.TabIndex = 21;
			this.checkBoxCleanDir.Text = "Clean Bin Dir";
			// 
			// checkBoxSyncPerforce
			// 
			this.checkBoxSyncPerforce.Location = new System.Drawing.Point(16, 72);
			this.checkBoxSyncPerforce.Name = "checkBoxSyncPerforce";
			this.checkBoxSyncPerforce.Size = new System.Drawing.Size(96, 24);
			this.checkBoxSyncPerforce.TabIndex = 3;
			this.checkBoxSyncPerforce.Text = "Sync";
			this.checkBoxSyncPerforce.CheckedChanged += new System.EventHandler(this.checkBoxSyncPerforce_CheckedChanged);
			// 
			// checkBoxBuildDebugEditors
			// 
			this.checkBoxBuildDebugEditors.Location = new System.Drawing.Point(16, 152);
			this.checkBoxBuildDebugEditors.Name = "checkBoxBuildDebugEditors";
			this.checkBoxBuildDebugEditors.TabIndex = 15;
			this.checkBoxBuildDebugEditors.Text = "Debug Editors";
			// 
			// checkBoxBuildSpeedEditors
			// 
			this.checkBoxBuildSpeedEditors.Location = new System.Drawing.Point(120, 152);
			this.checkBoxBuildSpeedEditors.Name = "checkBoxBuildSpeedEditors";
			this.checkBoxBuildSpeedEditors.Size = new System.Drawing.Size(96, 24);
			this.checkBoxBuildSpeedEditors.TabIndex = 4;
			this.checkBoxBuildSpeedEditors.Text = "Speed Editors";
			// 
			// checkBoxCompileCAI
			// 
			this.checkBoxCompileCAI.Location = new System.Drawing.Point(16, 216);
			this.checkBoxCompileCAI.Name = "checkBoxCompileCAI";
			this.checkBoxCompileCAI.TabIndex = 9;
			this.checkBoxCompileCAI.Text = "Generate AI2C";
			// 
			// label2
			// 
			this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label2.Location = new System.Drawing.Point(136, 224);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(200, 16);
			this.label2.TabIndex = 10;
			this.label2.Text = "Sync must be enabled for AI2C !";
			// 
			// checkBoxGetBF
			// 
			this.checkBoxGetBF.Location = new System.Drawing.Point(16, 40);
			this.checkBoxGetBF.Name = "checkBoxGetBF";
			this.checkBoxGetBF.TabIndex = 20;
			this.checkBoxGetBF.Text = "Get BF";
			// 
			// checkBoxCompileAI
			// 
			this.checkBoxCompileAI.Location = new System.Drawing.Point(16, 184);
			this.checkBoxCompileAI.Name = "checkBoxCompileAI";
			this.checkBoxCompileAI.Size = new System.Drawing.Size(136, 24);
			this.checkBoxCompileAI.TabIndex = 21;
			this.checkBoxCompileAI.Text = "Compile Interpreted AI";
			// 
			// checkBoxPPMaps
			// 
			this.checkBoxPPMaps.Location = new System.Drawing.Point(16, 248);
			this.checkBoxPPMaps.Name = "checkBoxPPMaps";
			this.checkBoxPPMaps.Size = new System.Drawing.Size(112, 24);
			this.checkBoxPPMaps.TabIndex = 23;
			this.checkBoxPPMaps.Text = "Preprocess Maps";
			// 
			// checkBoxDeleteAllBin
			// 
			this.checkBoxDeleteAllBin.Location = new System.Drawing.Point(16, 280);
			this.checkBoxDeleteAllBin.Name = "checkBoxDeleteAllBin";
			this.checkBoxDeleteAllBin.Size = new System.Drawing.Size(120, 24);
			this.checkBoxDeleteAllBin.TabIndex = 34;
			this.checkBoxDeleteAllBin.Text = "Delete All Bin data";
			// 
			// checkBoxBinAll
			// 
			this.checkBoxBinAll.Location = new System.Drawing.Point(16, 312);
			this.checkBoxBinAll.Name = "checkBoxBinAll";
			this.checkBoxBinAll.TabIndex = 24;
			this.checkBoxBinAll.Text = "Bin Maps";
			// 
			// checkBoxCleanBF
			// 
			this.checkBoxCleanBF.Location = new System.Drawing.Point(16, 344);
			this.checkBoxCleanBF.Name = "checkBoxCleanBF";
			this.checkBoxCleanBF.Size = new System.Drawing.Size(72, 24);
			this.checkBoxCleanBF.TabIndex = 25;
			this.checkBoxCleanBF.Text = "Clean BF";
			// 
			// checkBoxBuildDebugXenon
			// 
			this.checkBoxBuildDebugXenon.Location = new System.Drawing.Point(16, 376);
			this.checkBoxBuildDebugXenon.Name = "checkBoxBuildDebugXenon";
			this.checkBoxBuildDebugXenon.Size = new System.Drawing.Size(96, 24);
			this.checkBoxBuildDebugXenon.TabIndex = 11;
			this.checkBoxBuildDebugXenon.Text = "Debug Xenon";
			// 
			// checkBoxBuildReleaseXenon
			// 
			this.checkBoxBuildReleaseXenon.Location = new System.Drawing.Point(120, 376);
			this.checkBoxBuildReleaseXenon.Name = "checkBoxBuildReleaseXenon";
			this.checkBoxBuildReleaseXenon.TabIndex = 16;
			this.checkBoxBuildReleaseXenon.Text = "Release Xenon";
			// 
			// checkBoxBuildFinalXenon
			// 
			this.checkBoxBuildFinalXenon.Location = new System.Drawing.Point(224, 376);
			this.checkBoxBuildFinalXenon.Name = "checkBoxBuildFinalXenon";
			this.checkBoxBuildFinalXenon.TabIndex = 18;
			this.checkBoxBuildFinalXenon.Text = "Final Xenon";
			// 
			// tabControl
			// 
			this.tabControl.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.tabControl.Controls.Add(this.tabSetup);
			this.tabControl.Controls.Add(this.tabSteps);
			this.tabControl.Controls.Add(this.tabMaps);
			this.tabControl.ItemSize = new System.Drawing.Size(42, 18);
			this.tabControl.Location = new System.Drawing.Point(0, 0);
			this.tabControl.Name = "tabControl";
			this.tabControl.SelectedIndex = 0;
			this.tabControl.Size = new System.Drawing.Size(648, 632);
			this.tabControl.TabIndex = 8;
			// 
			// tabSetup
			// 
			this.tabSetup.Controls.Add(this.checkBoxLogOnTop);
			this.tabSetup.Controls.Add(this.groupDataSetup);
			this.tabSetup.Controls.Add(this.groupCodeSetup);
			this.tabSetup.Controls.Add(this.groupBoxEnvSetup);
			this.tabSetup.Location = new System.Drawing.Point(4, 22);
			this.tabSetup.Name = "tabSetup";
			this.tabSetup.Size = new System.Drawing.Size(640, 606);
			this.tabSetup.TabIndex = 3;
			this.tabSetup.Text = "Setup";
			// 
			// checkBoxLogOnTop
			// 
			this.checkBoxLogOnTop.Location = new System.Drawing.Point(16, 368);
			this.checkBoxLogOnTop.Name = "checkBoxLogOnTop";
			this.checkBoxLogOnTop.Size = new System.Drawing.Size(168, 24);
			this.checkBoxLogOnTop.TabIndex = 28;
			this.checkBoxLogOnTop.Text = "Log Windows Always on Top";
			this.checkBoxLogOnTop.CheckedChanged += new System.EventHandler(this.checkBoxLogOnTop_CheckedChanged);
			// 
			// groupDataSetup
			// 
			this.groupDataSetup.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.groupDataSetup.Controls.Add(this.label6);
			this.groupDataSetup.Controls.Add(this.buttonBrowseEditorWithAI2C);
			this.groupDataSetup.Controls.Add(this.textBoxEditorWithAI2C);
			this.groupDataSetup.Controls.Add(this.label5);
			this.groupDataSetup.Controls.Add(this.buttonBrowseEditor);
			this.groupDataSetup.Controls.Add(this.textBoxEditor);
			this.groupDataSetup.Controls.Add(this.label4);
			this.groupDataSetup.Controls.Add(this.textBoxBigFile);
			this.groupDataSetup.Controls.Add(this.buttonBrowseBF);
			this.groupDataSetup.Controls.Add(this.labelBF);
			this.groupDataSetup.Location = new System.Drawing.Point(8, 168);
			this.groupDataSetup.Name = "groupDataSetup";
			this.groupDataSetup.Size = new System.Drawing.Size(624, 184);
			this.groupDataSetup.TabIndex = 27;
			this.groupDataSetup.TabStop = false;
			this.groupDataSetup.Text = "Data";
			// 
			// label6
			// 
			this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Italic))), System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label6.Location = new System.Drawing.Point(104, 64);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(368, 16);
			this.label6.TabIndex = 33;
			this.label6.Text = "If you specify editors below, they will not be built during the bin process";
			// 
			// buttonBrowseEditorWithAI2C
			// 
			this.buttonBrowseEditorWithAI2C.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.buttonBrowseEditorWithAI2C.Location = new System.Drawing.Point(544, 120);
			this.buttonBrowseEditorWithAI2C.Name = "buttonBrowseEditorWithAI2C";
			this.buttonBrowseEditorWithAI2C.TabIndex = 32;
			this.buttonBrowseEditorWithAI2C.Text = "Browse...";
			this.buttonBrowseEditorWithAI2C.Click += new System.EventHandler(this.buttonBrowseEditorWithAI2C_Click);
			// 
			// textBoxEditorWithAI2C
			// 
			this.textBoxEditorWithAI2C.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.textBoxEditorWithAI2C.Location = new System.Drawing.Point(128, 120);
			this.textBoxEditorWithAI2C.Name = "textBoxEditorWithAI2C";
			this.textBoxEditorWithAI2C.Size = new System.Drawing.Size(400, 20);
			this.textBoxEditorWithAI2C.TabIndex = 31;
			this.textBoxEditorWithAI2C.Text = "";
			this.textBoxEditorWithAI2C.TextChanged += new System.EventHandler(this.textBoxEditorWithAI2C_TextChanged);
			// 
			// label5
			// 
			this.label5.Location = new System.Drawing.Point(8, 120);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(100, 16);
			this.label5.TabIndex = 30;
			this.label5.Text = "Editor with AI2C";
			// 
			// buttonBrowseEditor
			// 
			this.buttonBrowseEditor.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.buttonBrowseEditor.Location = new System.Drawing.Point(544, 88);
			this.buttonBrowseEditor.Name = "buttonBrowseEditor";
			this.buttonBrowseEditor.TabIndex = 29;
			this.buttonBrowseEditor.Text = "Browse...";
			this.buttonBrowseEditor.Click += new System.EventHandler(this.buttonBrowseEditor_Click);
			// 
			// textBoxEditor
			// 
			this.textBoxEditor.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.textBoxEditor.Location = new System.Drawing.Point(128, 88);
			this.textBoxEditor.Name = "textBoxEditor";
			this.textBoxEditor.Size = new System.Drawing.Size(400, 20);
			this.textBoxEditor.TabIndex = 28;
			this.textBoxEditor.Text = "";
			this.textBoxEditor.TextChanged += new System.EventHandler(this.textBoxEditor_TextChanged);
			// 
			// label4
			// 
			this.label4.Location = new System.Drawing.Point(8, 88);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(100, 16);
			this.label4.TabIndex = 27;
			this.label4.Text = "Editor";
			// 
			// textBoxBigFile
			// 
			this.textBoxBigFile.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.textBoxBigFile.Location = new System.Drawing.Point(127, 24);
			this.textBoxBigFile.Name = "textBoxBigFile";
			this.textBoxBigFile.Size = new System.Drawing.Size(400, 20);
			this.textBoxBigFile.TabIndex = 25;
			this.textBoxBigFile.Text = "";
			// 
			// buttonBrowseBF
			// 
			this.buttonBrowseBF.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.buttonBrowseBF.Location = new System.Drawing.Point(543, 24);
			this.buttonBrowseBF.Name = "buttonBrowseBF";
			this.buttonBrowseBF.TabIndex = 26;
			this.buttonBrowseBF.Text = "Browse...";
			this.buttonBrowseBF.Click += new System.EventHandler(this.buttonBrowseBF_Click);
			// 
			// labelBF
			// 
			this.labelBF.Location = new System.Drawing.Point(7, 24);
			this.labelBF.Name = "labelBF";
			this.labelBF.Size = new System.Drawing.Size(100, 16);
			this.labelBF.TabIndex = 24;
			this.labelBF.Text = "Main BF";
			// 
			// groupCodeSetup
			// 
			this.groupCodeSetup.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.groupCodeSetup.Controls.Add(this.textBoxCodeLocation);
			this.groupCodeSetup.Controls.Add(this.labelp4);
			this.groupCodeSetup.Controls.Add(this.buttonBrowseJadeDir);
			this.groupCodeSetup.Location = new System.Drawing.Point(8, 95);
			this.groupCodeSetup.Name = "groupCodeSetup";
			this.groupCodeSetup.Size = new System.Drawing.Size(624, 72);
			this.groupCodeSetup.TabIndex = 26;
			this.groupCodeSetup.TabStop = false;
			this.groupCodeSetup.Text = "Code";
			// 
			// textBoxCodeLocation
			// 
			this.textBoxCodeLocation.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.textBoxCodeLocation.Location = new System.Drawing.Point(127, 32);
			this.textBoxCodeLocation.Name = "textBoxCodeLocation";
			this.textBoxCodeLocation.Size = new System.Drawing.Size(400, 20);
			this.textBoxCodeLocation.TabIndex = 4;
			this.textBoxCodeLocation.Text = "";
			// 
			// labelp4
			// 
			this.labelp4.Location = new System.Drawing.Point(8, 24);
			this.labelp4.Name = "labelp4";
			this.labelp4.Size = new System.Drawing.Size(120, 40);
			this.labelp4.TabIndex = 3;
			this.labelp4.Text = "Source code location (.../Souce/Jade directory)";
			// 
			// buttonBrowseJadeDir
			// 
			this.buttonBrowseJadeDir.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.buttonBrowseJadeDir.Location = new System.Drawing.Point(543, 32);
			this.buttonBrowseJadeDir.Name = "buttonBrowseJadeDir";
			this.buttonBrowseJadeDir.TabIndex = 5;
			this.buttonBrowseJadeDir.Text = "Browse...";
			this.buttonBrowseJadeDir.Click += new System.EventHandler(this.buttonBrowseJadeDir_Click);
			// 
			// groupBoxEnvSetup
			// 
			this.groupBoxEnvSetup.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.groupBoxEnvSetup.Controls.Add(this.textBoxBinDir);
			this.groupBoxEnvSetup.Controls.Add(this.labelBinDir);
			this.groupBoxEnvSetup.Controls.Add(this.buttonBrowseBinRootDir);
			this.groupBoxEnvSetup.Controls.Add(this.buttonBrowseMSDev);
			this.groupBoxEnvSetup.Controls.Add(this.textBoxDevEnv);
			this.groupBoxEnvSetup.Controls.Add(this.label1);
			this.groupBoxEnvSetup.Location = new System.Drawing.Point(8, 8);
			this.groupBoxEnvSetup.Name = "groupBoxEnvSetup";
			this.groupBoxEnvSetup.Size = new System.Drawing.Size(624, 88);
			this.groupBoxEnvSetup.TabIndex = 25;
			this.groupBoxEnvSetup.TabStop = false;
			this.groupBoxEnvSetup.Text = "Environment";
			// 
			// textBoxBinDir
			// 
			this.textBoxBinDir.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.textBoxBinDir.Location = new System.Drawing.Point(127, 24);
			this.textBoxBinDir.Name = "textBoxBinDir";
			this.textBoxBinDir.Size = new System.Drawing.Size(400, 20);
			this.textBoxBinDir.TabIndex = 22;
			this.textBoxBinDir.Text = "";
			// 
			// labelBinDir
			// 
			this.labelBinDir.Location = new System.Drawing.Point(7, 24);
			this.labelBinDir.Name = "labelBinDir";
			this.labelBinDir.Size = new System.Drawing.Size(112, 16);
			this.labelBinDir.TabIndex = 21;
			this.labelBinDir.Text = "BIN Working Folder";
			// 
			// buttonBrowseBinRootDir
			// 
			this.buttonBrowseBinRootDir.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.buttonBrowseBinRootDir.Location = new System.Drawing.Point(543, 24);
			this.buttonBrowseBinRootDir.Name = "buttonBrowseBinRootDir";
			this.buttonBrowseBinRootDir.TabIndex = 23;
			this.buttonBrowseBinRootDir.Text = "Browse...";
			this.buttonBrowseBinRootDir.Click += new System.EventHandler(this.buttonBrowseBinRootDir_Click);
			// 
			// buttonBrowseMSDev
			// 
			this.buttonBrowseMSDev.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.buttonBrowseMSDev.Location = new System.Drawing.Point(543, 56);
			this.buttonBrowseMSDev.Name = "buttonBrowseMSDev";
			this.buttonBrowseMSDev.TabIndex = 20;
			this.buttonBrowseMSDev.Text = "Browse...";
			this.buttonBrowseMSDev.Click += new System.EventHandler(this.buttonBrowseMSDev_Click);
			// 
			// textBoxDevEnv
			// 
			this.textBoxDevEnv.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.textBoxDevEnv.Location = new System.Drawing.Point(127, 56);
			this.textBoxDevEnv.Name = "textBoxDevEnv";
			this.textBoxDevEnv.Size = new System.Drawing.Size(400, 20);
			this.textBoxDevEnv.TabIndex = 19;
			this.textBoxDevEnv.Text = "";
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(7, 56);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(120, 16);
			this.label1.TabIndex = 18;
			this.label1.Text = "MS Visual Studio path";
			// 
			// tabMaps
			// 
			this.tabMaps.Controls.Add(this.label10);
			this.tabMaps.Controls.Add(this.comboBoxMapList);
			this.tabMaps.Controls.Add(this.buttonLoadMapList);
			this.tabMaps.Controls.Add(this.buttonMapListToggleAll);
			this.tabMaps.Controls.Add(this.buttonSaveMapList);
			this.tabMaps.Controls.Add(this.listBoxMapList);
			this.tabMaps.Controls.Add(this.buttonGenerateMapList);
			this.tabMaps.Controls.Add(this.labelMapList);
			this.tabMaps.Controls.Add(this.textBoxMapList);
			this.tabMaps.Controls.Add(this.buttonBrowseMapList);
			this.tabMaps.Location = new System.Drawing.Point(4, 22);
			this.tabMaps.Name = "tabMaps";
			this.tabMaps.Size = new System.Drawing.Size(640, 606);
			this.tabMaps.TabIndex = 4;
			this.tabMaps.Text = "Maps";
			// 
			// label10
			// 
			this.label10.Location = new System.Drawing.Point(8, 456);
			this.label10.Name = "label10";
			this.label10.Size = new System.Drawing.Size(144, 23);
			this.label10.TabIndex = 36;
			this.label10.Text = "Default AI compilation level";
			// 
			// comboBoxMapList
			// 
			this.comboBoxMapList.Location = new System.Drawing.Point(152, 448);
			this.comboBoxMapList.Name = "comboBoxMapList";
			this.comboBoxMapList.Size = new System.Drawing.Size(480, 21);
			this.comboBoxMapList.TabIndex = 35;
			this.comboBoxMapList.SelectedIndexChanged += new System.EventHandler(this.comboBoxMapList_SelectedIndexChanged);
			// 
			// buttonLoadMapList
			// 
			this.buttonLoadMapList.Location = new System.Drawing.Point(168, 40);
			this.buttonLoadMapList.Name = "buttonLoadMapList";
			this.buttonLoadMapList.Size = new System.Drawing.Size(104, 24);
			this.buttonLoadMapList.TabIndex = 34;
			this.buttonLoadMapList.Text = "Load Map List";
			this.buttonLoadMapList.Click += new System.EventHandler(this.buttonLoadMapList_Click);
			// 
			// buttonMapListToggleAll
			// 
			this.buttonMapListToggleAll.Location = new System.Drawing.Point(8, 48);
			this.buttonMapListToggleAll.Name = "buttonMapListToggleAll";
			this.buttonMapListToggleAll.TabIndex = 33;
			this.buttonMapListToggleAll.Text = "Toggle All";
			this.buttonMapListToggleAll.Click += new System.EventHandler(this.buttonMapListToggleAll_Click);
			// 
			// buttonSaveMapList
			// 
			this.buttonSaveMapList.Location = new System.Drawing.Point(280, 40);
			this.buttonSaveMapList.Name = "buttonSaveMapList";
			this.buttonSaveMapList.Size = new System.Drawing.Size(104, 24);
			this.buttonSaveMapList.TabIndex = 16;
			this.buttonSaveMapList.Text = "Save Map List";
			this.buttonSaveMapList.Click += new System.EventHandler(this.buttonSaveMapList_Click);
			// 
			// listBoxMapList
			// 
			this.listBoxMapList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.listBoxMapList.CheckOnClick = true;
			this.listBoxMapList.HorizontalScrollbar = true;
			this.listBoxMapList.Location = new System.Drawing.Point(8, 72);
			this.listBoxMapList.Name = "listBoxMapList";
			this.listBoxMapList.ScrollAlwaysVisible = true;
			this.listBoxMapList.Size = new System.Drawing.Size(624, 364);
			this.listBoxMapList.TabIndex = 15;
			// 
			// buttonGenerateMapList
			// 
			this.buttonGenerateMapList.Location = new System.Drawing.Point(152, 352);
			this.buttonGenerateMapList.Name = "buttonGenerateMapList";
			this.buttonGenerateMapList.Size = new System.Drawing.Size(112, 23);
			this.buttonGenerateMapList.TabIndex = 14;
			this.buttonGenerateMapList.Text = "Generate Map List";
			this.buttonGenerateMapList.Click += new System.EventHandler(this.buttonGenerateMapList_Click);
			// 
			// labelMapList
			// 
			this.labelMapList.Location = new System.Drawing.Point(8, 16);
			this.labelMapList.Name = "labelMapList";
			this.labelMapList.Size = new System.Drawing.Size(100, 16);
			this.labelMapList.TabIndex = 30;
			this.labelMapList.Text = "Map List";
			// 
			// textBoxMapList
			// 
			this.textBoxMapList.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.textBoxMapList.Location = new System.Drawing.Point(128, 16);
			this.textBoxMapList.Name = "textBoxMapList";
			this.textBoxMapList.Size = new System.Drawing.Size(400, 20);
			this.textBoxMapList.TabIndex = 31;
			this.textBoxMapList.Text = "";
			// 
			// buttonBrowseMapList
			// 
			this.buttonBrowseMapList.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.buttonBrowseMapList.Location = new System.Drawing.Point(544, 16);
			this.buttonBrowseMapList.Name = "buttonBrowseMapList";
			this.buttonBrowseMapList.TabIndex = 32;
			this.buttonBrowseMapList.Text = "Browse...";
			this.buttonBrowseMapList.Click += new System.EventHandler(this.buttonBrowseMapList_Click);
			// 
			// mainMenu1
			// 
			this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItemFile,
																					  this.menuItemShowLogs,
																					  this.menuItemAction});
			// 
			// menuItemFile
			// 
			this.menuItemFile.Index = 0;
			this.menuItemFile.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																						 this.menuItemLoadConfig,
																						 this.menuItemSaveConfig,
																						 this.menuItemSaveConfigAs,
																						 this.menuItem4,
																						 this.menuItemQuit});
			this.menuItemFile.Text = "&File";
			// 
			// menuItemLoadConfig
			// 
			this.menuItemLoadConfig.Index = 0;
			this.menuItemLoadConfig.Text = "&Load Config...";
			this.menuItemLoadConfig.Click += new System.EventHandler(this.menuItemLoadConfig_Click);
			// 
			// menuItemSaveConfig
			// 
			this.menuItemSaveConfig.Index = 1;
			this.menuItemSaveConfig.Text = "&Save Config";
			this.menuItemSaveConfig.Click += new System.EventHandler(this.menuItemSaveConfig_Click);
			// 
			// menuItemSaveConfigAs
			// 
			this.menuItemSaveConfigAs.Index = 2;
			this.menuItemSaveConfigAs.Text = "Save Config &As...";
			this.menuItemSaveConfigAs.Click += new System.EventHandler(this.menuItemSaveConfigAs_Click);
			// 
			// menuItem4
			// 
			this.menuItem4.Index = 3;
			this.menuItem4.Text = "-";
			// 
			// menuItemQuit
			// 
			this.menuItemQuit.Index = 4;
			this.menuItemQuit.Text = "&Quit";
			this.menuItemQuit.Click += new System.EventHandler(this.menuItemQuit_Click);
			// 
			// menuItemShowLogs
			// 
			this.menuItemShowLogs.Index = 1;
			this.menuItemShowLogs.Text = "&Show Logs";
			this.menuItemShowLogs.Click += new System.EventHandler(this.menuItemShowLogs_Click);
			// 
			// menuItemAction
			// 
			this.menuItemAction.Index = 2;
			this.menuItemAction.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																						   this.menuItemRun,
																						   this.menuItem3,
																						   this.menuItemCancel});
			this.menuItemAction.Text = "&Action";
			// 
			// menuItemRun
			// 
			this.menuItemRun.Index = 0;
			this.menuItemRun.Text = "&Run";
			this.menuItemRun.Click += new System.EventHandler(this.menuItemRun_Click);
			// 
			// menuItem3
			// 
			this.menuItem3.Index = 1;
			this.menuItem3.Text = "-";
			// 
			// menuItemCancel
			// 
			this.menuItemCancel.Index = 2;
			this.menuItemCancel.Text = "&Cancel";
			this.menuItemCancel.Click += new System.EventHandler(this.menuItemCancel_Click);
			// 
			// statusBar
			// 
			this.statusBar.Location = new System.Drawing.Point(0, 633);
			this.statusBar.Name = "statusBar";
			this.statusBar.Size = new System.Drawing.Size(648, 16);
			this.statusBar.TabIndex = 9;
			// 
			// MainForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(648, 649);
			this.Controls.Add(this.statusBar);
			this.Controls.Add(this.tabControl);
			this.Menu = this.mainMenu1;
			this.MinimumSize = new System.Drawing.Size(544, 472);
			this.Name = "MainForm";
			this.Text = "Binarisation";
			this.Closed += new System.EventHandler(this.MainForm_Closed);
			this.tabSteps.ResumeLayout(false);
			this.panel2.ResumeLayout(false);
			this.panel1.ResumeLayout(false);
			this.tabControl.ResumeLayout(false);
			this.tabSetup.ResumeLayout(false);
			this.groupDataSetup.ResumeLayout(false);
			this.groupCodeSetup.ResumeLayout(false);
			this.groupBoxEnvSetup.ResumeLayout(false);
			this.tabMaps.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		static int Main(string[] args) 
		{
            MainForm form = new MainForm(args);

			Application.Run(form);

            if (BinProcess.ProcessFailed)
                return 1;
            else
                return 0;
		}

        #region .INI File Load / Save
        private string[] SetupTag;
        private System.Windows.Forms.TextBox[] SetupBox;
        private string[] OptionTag;
        private System.Windows.Forms.CheckBox[] OptionBox;
        private string[] ButtonTag;
        private System.Windows.Forms.RadioButton[] OptionButton;
        private string[] MainFormData;

        private void menuItemLoadConfig_Click(object sender, System.EventArgs e)
        {
            string fileName = configFileName;

            GetUserFilePathInput("Config file to load", ref fileName, false);                        
            ReadConfigFile(fileName);

            // now default config file is this one
            configFileName = fileName;
        }

        private void menuItemSaveConfig_Click(object sender, System.EventArgs e)
        {
            SaveConfigFile(configFileName);      
            statusBar.Text = "Config saved in " + configFileName; 
        }

        private void menuItemSaveConfigAs_Click(object sender, System.EventArgs e)
        {
            string fileName = configFileName;

            GetUserFilePathInput("Config file to save", ref fileName, true);                        

            SaveConfigFile(fileName);
        }

		private void ReadConfigFile(string fileName)
		{
			if (File.Exists(fileName))
			{
				System.IO.StreamReader iniFile = new System.IO.StreamReader(fileName);
				string myString = null; 
                int newHeight = -1;
                int newWidth  = -1;

				// Display the file contents.
				while ((myString = iniFile.ReadLine()) != null)
				{					
					for (int i = 0; i < SetupTag.Length; i++)
					{
						if (myString.StartsWith(SetupTag[i]))
						{
							SetupBox[i].Text = iniFile.ReadLine();
						}
					}

					for (int i = 0; i < OptionTag.Length; i++)
					{
						if (myString.StartsWith(OptionTag[i]))
						{
							if (iniFile.Peek() == '1')							
								OptionBox[i].Checked = true;
							else
                                OptionBox[i].Checked = false;
						}
					}

                    for (int i = 0; i < ButtonTag.Length; i++)
                    {
                        if (myString.StartsWith(ButtonTag[i]))
                        {
                            if (iniFile.Peek() == '1')                            
                                OptionButton[i].Checked = true;
                           	else
						        OptionButton[i].Checked = false;
                        }
                    }

					if (myString == "[AiGeneration-DefaultLevel]")
					{
						comboBoxMapList.Text = iniFile.ReadLine();
					}

                    // Get size of main window
                    if (myString.StartsWith(MainFormData[0]))
                        newWidth = Int32.Parse(iniFile.ReadLine());
                    if (myString.StartsWith(MainFormData[1]))
                        newHeight = Int32.Parse(iniFile.ReadLine());
                }

                if (newHeight > 0 && newWidth > 0)
                    Size = new Size(newWidth, newHeight);

				iniFile.Close();
			}	
            else
            {
                // TODO: error file name found
            }
		}

        private void SaveConfigFile(string fileName)
        {
            // Write the string to a file.
            System.IO.StreamWriter file = new System.IO.StreamWriter(fileName);
			
            for (int i = 0; i < SetupTag.Length; i++)
            {
                file.WriteLine(SetupTag[i]);
                file.WriteLine(SetupBox[i].Text.ToString());
            }

            for (int i = 0; i < OptionTag.Length; i++)
            {
                file.WriteLine(OptionTag[i]);
                if (OptionBox[i].Checked)
                    file.WriteLine("1");
			    else
                    file.WriteLine("0");
            }

            for (int i = 0; i < ButtonTag.Length; i++)
            {
                file.WriteLine(ButtonTag[i]);
                if (OptionButton[i].Checked)
                    file.WriteLine("1");
                else
                    file.WriteLine("0");
            }

			file.WriteLine("[AiGeneration-DefaultLevel]");
			file.WriteLine(comboBoxMapList.Text);

            // Save window size and position
            file.WriteLine(MainFormData[0]);
            file.WriteLine(Size.Width);            
            file.WriteLine(MainFormData[1]);
            file.WriteLine(Size.Height);
            file.Close();
        }
        #endregion

        #region User Input Dialogs
        private void GetUserDirectoryInput(string title, System.Windows.Forms.TextBox textBox)
        {
            FolderBrowserDialog dialog = new FolderBrowserDialog();

            dialog.SelectedPath = textBox.Text;
            if (dialog.ShowDialog() == DialogResult.OK)
                textBox.Text = dialog.SelectedPath;
        }

        private void GetUserFilePathInput(string title, ref string fileName, bool save)
        {
            if (save)
            {            
                SaveFileDialog dialog = new SaveFileDialog();

                dialog.FileName = fileName;
                if (dialog.ShowDialog() == DialogResult.OK)
                    fileName = dialog.FileName;
            }
            else
            {
                OpenFileDialog dialog = new OpenFileDialog();

                if (Path.GetFileName(fileName) != String.Empty)
                    dialog.FileName = fileName;
                if (dialog.ShowDialog() == DialogResult.OK)
                    fileName = dialog.FileName;
            }
        }
        #endregion

        #region BrowseButton
        private void buttonBrowseEditor_Click(object sender, System.EventArgs e)
        {
            string fileName = textBoxEditor.Text;
            GetUserFilePathInput("Editor Location", ref fileName, false);    
            textBoxEditor.Text = fileName;        
        }

        private void buttonBrowseEditorWithAI2C_Click(object sender, System.EventArgs e)
        {
            string fileName = textBoxEditorWithAI2C.Text;
            GetUserFilePathInput("Editor with AI2C Location", ref fileName, false);    
            textBoxEditorWithAI2C.Text = fileName;                
        }

        private void buttonBrowseJadeDir_Click(object sender, System.EventArgs e)
        {
            GetUserDirectoryInput(@"Select source code location (../souces/jade)", textBoxCodeLocation);
        }

        private void buttonBrowseBF_Click(object sender, System.EventArgs e)
        {
            string fileName = textBoxBigFile.Text;
            GetUserFilePathInput("BF Location", ref fileName, false);    
            textBoxBigFile.Text = fileName;
        }

        private void buttonBrowseMSDev_Click(object sender, System.EventArgs e)
        {            
            string fileName = textBoxDevEnv.Text;
            GetUserFilePathInput("Path to MS Visual Studio devenv.exe", ref fileName, false);                
            textBoxDevEnv.Text = fileName;
        }

        private void buttonBrowseMapList_Click(object sender, System.EventArgs e)
        {
            string fileName = textBoxMapList.Text;
            GetUserFilePathInput("Maplist.txt location", ref fileName, false);  
            textBoxMapList.Text = fileName;          
        }

        private void buttonBrowseBinRootDir_Click(object sender, System.EventArgs e)
        {
            GetUserDirectoryInput("Binarization Working Dir", this.textBoxBinDir);                
        }
        #endregion

        private void checkBoxSyncPerforce_CheckedChanged(object sender, System.EventArgs e)
        {
            if (checkBoxSyncPerforce.Checked)
            {
                // enable radio buttons
                buttonPerforceSyncToHead.Enabled = true;
                buttonPerforceSyncToChangelist.Enabled = true;

                // enable changelist number
                if (buttonPerforceSyncToChangelist.Checked)
                    textBoxPerforceCodeChangelistNumber.Enabled = true;
                else
                    textBoxPerforceCodeChangelistNumber.Enabled = false;

				// enable AI2C generation (we need to know what to sync to when cleaning AI
				checkBoxCompileCAI.Enabled = true;
            }
            else
            {
                buttonPerforceSyncToHead.Enabled = false;
                buttonPerforceSyncToChangelist.Enabled = false;
                textBoxPerforceCodeChangelistNumber.Enabled = false;
				checkBoxCompileCAI.Enabled = false;
            }
        }

        private void buttonPerforceSyncToChangelist_CheckedChanged(object sender, System.EventArgs e)
        {
            if (buttonPerforceSyncToChangelist.Checked)
                textBoxPerforceCodeChangelistNumber.Enabled = true;        
        }

        private void buttonPerforceSyncToHead_CheckedChanged(object sender, System.EventArgs e)
        {
            if (buttonPerforceSyncToHead.Checked)
                textBoxPerforceCodeChangelistNumber.Enabled = false;
        }

        private void menuItemQuit_Click(object sender, System.EventArgs e)
        {
            Application.Exit();        
        }

        class BinProcessThread
        {
            private BinProcess.Setup setupInfo;
            public BinProcessThread(BinProcess.Setup setupInfo)
            {
                this.setupInfo = setupInfo;
            }

            public void StartBinProcess()
            {
                try
                {
                    BinProcess.Binarize(setupInfo);                
                }
                catch(ApplicationException e)
                {
                    MessageBox.Show(e.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void menuItemRun_Click(object sender, System.EventArgs e)
        {
            BinProcess.Setup setupInfo;

            CreateBinSetupInfo(out setupInfo);

            BinProcessThread binProcessThread = new BinProcessThread(setupInfo);
            mainThread = new Thread(new ThreadStart(binProcessThread.StartBinProcess));
            mainThread.Start();      

            monitorThread = new Thread(new ThreadStart(MonitorBinProcess));
            monitorThread.Start();
        }
        
        private void MonitorBinProcess()
        {
            tabControl.Enabled     = false;
            menuItemCancel.Enabled = true;
            menuItemRun.Enabled    = false;
            menuItemFile.Enabled   = false;
            statusBar.Text         = "Running...";

            mainThread.Join();

            tabControl.Enabled     = true;
            menuItemCancel.Enabled = false;
            menuItemRun.Enabled    = true;
            menuItemFile.Enabled   = true;
            statusBar.Text         = "Ready";
            
            if (cmdLineParams.AutoRun)
                menuItemQuit.PerformClick();
        }

        private void CreateBinSetupInfo(out BinProcess.Setup setupInfo)
        {
            setupInfo = new BinProcess.Setup();

            setupInfo.bQuiet                = cmdLineParams.Quiet;
            setupInfo.bCleanDir             = checkBoxCleanDir.Checked;
            setupInfo.bLogOnTop             = checkBoxLogOnTop.Checked;

            setupInfo.bPerfoceSync          = checkBoxSyncPerforce.Checked;
            setupInfo.bPerforceSyncToHead   = buttonPerforceSyncToHead.Checked;
            setupInfo.bPerforceSyncToChangelist = buttonPerforceSyncToChangelist.Checked;
			if (textBoxPerforceCodeChangelistNumber.Text == String.Empty)
				setupInfo.iPerfoceSyncToChangelistNumber = 0;
			else
				setupInfo.iPerfoceSyncToChangelistNumber = Int32.Parse(textBoxPerforceCodeChangelistNumber.Text);
            setupInfo.bBuildDebugEditors    = checkBoxBuildDebugEditors.Checked;
            setupInfo.bBuildSpeedEditors    = checkBoxBuildSpeedEditors.Checked;
            setupInfo.bBuildDebugXenon      = checkBoxBuildDebugXenon.Checked;
            setupInfo.bBuildReleaseXenon    = checkBoxBuildReleaseXenon.Checked;
            setupInfo.bBuildFinalXenon      = checkBoxBuildFinalXenon.Checked;
            setupInfo.bBuildReleaseXProfile = checkBoxBuildReleaseXProfile.Checked;
            setupInfo.bGenerateAI2C         = checkBoxCompileCAI.Checked && checkBoxCompileCAI.Enabled;
          
            setupInfo.bGetBF                = checkBoxGetBF.Checked;
            setupInfo.bCompileAI            = checkBoxCompileAI.Checked;
            setupInfo.bPreprocessMaps       = checkBoxPPMaps.Checked;
            setupInfo.bDeleteAllBinData     = checkBoxDeleteAllBin.Checked;
            setupInfo.bBinMaps              = checkBoxBinAll.Checked;
            setupInfo.bCleanBF              = checkBoxCleanBF.Checked;

            setupInfo.sWorkDir              = textBoxBinDir.Text;
            setupInfo.sDevEnv               = textBoxDevEnv.Text;
            setupInfo.sCodeLocation         = textBoxCodeLocation.Text;
            setupInfo.sBFLocation           = textBoxBigFile.Text;
            setupInfo.sMapList              = textBoxMapList.Text;

            setupInfo.sLogFileName          = cmdLineParams.LogFileName;
            setupInfo.sLogDetailsFileName   = cmdLineParams.LogDetailsFileName;

            setupInfo.alMapList				= new ArrayList(listBoxMapList.CheckedItems);
			setupInfo.sDefaultLevel			= comboBoxMapList.Text;
			setupInfo.sEditorLocation         = textBoxEditor.Text;
            setupInfo.sEditorWithAI2CLocation = textBoxEditorWithAI2C.Text;

            setupInfo.sLanguages            = textBoxLanguages.Text;
        }

        private void menuItemCancel_Click(object sender, System.EventArgs e)
        {
            statusBar.Text = "Aborting...";
            mainThread.Abort();
        }

        private void buttonGenerateMapList_Click(object sender, System.EventArgs e)
        {    
            FillMapListBox(true);
        }

		private void FillMapListBox(bool bGenerate)
		{
			ArrayList        alMapList;
			BinProcess.Setup setupInfo;

			CreateBinSetupInfo(out setupInfo);
			if (bGenerate)
				BinProcess.GenerateMapList(setupInfo, out alMapList);

			BinProcess.ReadMapList(setupInfo, out alMapList);

			if (alMapList.Count == 0)
			{
				return;
			}

			string sCurrentSelection = comboBoxMapList.SelectedText;

			listBoxMapList.Items.Clear();
			comboBoxMapList.Items.Clear();		

			foreach(string mapName in alMapList)
			{
				if (mapName != string.Empty)
				{
					listBoxMapList.Items.Add(mapName, !bGenerate);
					comboBoxMapList.Items.Add(mapName);
				}
			}

			comboBoxMapList.SelectedText = sCurrentSelection;
		}

        private void buttonSaveMapList_Click(object sender, System.EventArgs e)
        {            
            if (textBoxMapList.Text == String.Empty || 
                Directory.Exists(Path.GetDirectoryName(textBoxMapList.Text)) == false)
            {
                MessageBox.Show("Invalid Map List Path","ERROR",MessageBoxButtons.OK,MessageBoxIcon.Error);
                return;
            }

            // Write the string to a file.
            System.IO.StreamWriter file = new System.IO.StreamWriter(textBoxMapList.Text);

            for (int i = 0; i < listBoxMapList.Items.Count; i++)
            {            
                string mapName = (string )listBoxMapList.Items[i];
                if (listBoxMapList.GetItemChecked(i))
                {
                    file.WriteLine(mapName);
                }
                else
                {
                    file.WriteLine(";{0}", mapName);
                }
            }
            file.Close();
        }

        static bool toggle = true;
        private void buttonMapListToggleAll_Click(object sender, System.EventArgs e)
        {
            for (int i = 0; i < listBoxMapList.Items.Count; i++)
            {
                listBoxMapList.SetItemChecked(i, toggle);
            }        
            toggle = !toggle;
        }

        private void buttonLoadMapList_Click(object sender, System.EventArgs e)
        {           
            FillMapListBox(false);
        }

        bool logsShown = false;
        private void menuItemShowLogs_Click(object sender, System.EventArgs e)
        {
            BinProcess.ShowLogs(logsShown);
            if (logsShown)
                menuItemShowLogs.Text = "&Hide Logs";
            else
                menuItemShowLogs.Text = "&Show Logs";
            logsShown = !logsShown;
        }

        private void MainForm_Closed(object sender, System.EventArgs e)
        {
            Application.Exit();  
        }

        private void textBoxEditor_TextChanged(object sender, System.EventArgs e)
        {
            if (textBoxEditor.Text == string.Empty && textBoxEditorWithAI2C.Text == string.Empty)
            {
                checkBoxBuildDebugEditors.Enabled = true;
                checkBoxBuildSpeedEditors.Enabled = true;
                labelEditorWarning.Text = "";
            }
            else
            {
                checkBoxBuildDebugEditors.Enabled = false;
                checkBoxBuildSpeedEditors.Enabled = false;
                labelEditorWarning.Text = "Disabled because Editor is set in Setup tab";
            }
        }

        private void textBoxEditorWithAI2C_TextChanged(object sender, System.EventArgs e)
        {
            if (textBoxEditorWithAI2C.Text != string.Empty)
            {                
                checkBoxCompileAI.Enabled  = false;
                checkBoxCompileCAI.Enabled = false;
                labelEditorWithAI2CWarning.Text = "Disabled because EditorWithAI2C is set in Setup tab";
                checkBoxBuildDebugXenon.Enabled   = false;
                checkBoxBuildReleaseXenon.Enabled = false;
                checkBoxBuildFinalXenon.Enabled   = false;
                checkBoxBuildReleaseXProfile.Enabled = false;
                textBoxEditor.Enabled = false;
            }
            else
            {            
                checkBoxCompileAI.Enabled  = true;
                checkBoxCompileCAI.Enabled = true;
                labelEditorWithAI2CWarning.Text = "";
                checkBoxBuildDebugXenon.Enabled   = true;
                checkBoxBuildReleaseXenon.Enabled = true;
                checkBoxBuildFinalXenon.Enabled   = true;
                checkBoxBuildReleaseXProfile.Enabled = true;
                textBoxEditor.Enabled = true;
            }    
            textBoxEditor_TextChanged(sender, e);
        }

        private void checkBoxLogOnTop_CheckedChanged(object sender, System.EventArgs e)
        {
            BinProcess.LogOnTop(checkBoxLogOnTop.Checked);
        }

        private void buttonBinUS_CheckedChanged(object sender, System.EventArgs e)
        {
            if (buttonBinUS.Checked)
                textBoxLanguages.Text = "fr,en,it,es,de,da,nl,fi,sv,no";
        }

        private void buttonBinJapan_CheckedChanged(object sender, System.EventArgs e)
        {
            if (buttonBinJapan.Checked)
                textBoxLanguages.Text = "en,ja";        
        }

		private void comboBoxMapList_SelectedIndexChanged(object sender, System.EventArgs e)
		{
		}


	}
}
