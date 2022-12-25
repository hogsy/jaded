using System;
using System.Collections;
using System.IO;
using System.Windows.Forms;

namespace Binarisation
{
	/// <summary>
	/// Summary description for Binarisation.
	/// </summary>
    public class BinProcess
    {
		// The HackAI2C function is called after generating the AI in C.. and before building it..
		// Add file modifications here if you want a quick hack to the process
		private void HackAI2C()
		{
		//FileSystem.CopyFile(setupInfo.sWorkDir + @"..\Interface_MenuInGame.h", sAICodeLocation + @"AI2C_ofc\");
		}

        public class Setup
        {
            public bool bQuiet;
            public bool bCleanDir;
            public bool bLogOnTop;

			public bool bPerfoceSync;
            public bool bPerforceSyncToHead;
            public bool bPerforceSyncToChangelist;
            public int  iPerfoceSyncToChangelistNumber;
            
			public bool bBuildDebugEditors;
            public bool bBuildSpeedEditors;
            public bool bBuildDebugXenon;
            public bool bBuildReleaseXenon;
            public bool bBuildFinalXenon;
            public bool bBuildReleaseXProfile;
            public bool bGenerateAI2C;

            public bool bGetBF;
            public bool bCompileAI;
            public bool bPreprocessMaps;
            public bool bDeleteAllBinData;
            public bool bBinMaps;
            public bool bCleanBF;

            public string sWorkDir;
            public string sDevEnv;
            public string sCodeLocation;
            public string sBFLocation;
            public string sMapList;
            public string sEditorLocation;
            public string sEditorWithAI2CLocation;
            
            public string sLogFileName;
            public string sLogDetailsFileName;

            public string sLanguages;

            public ArrayList alMapList;
			public string sDefaultLevel;
        }



        #region members
        private Setup  setupInfo;
        static private Logging logIF;
        static private Logging appLogIF;

        static private bool processFailed = false;
        static public bool ProcessFailed { get { return processFailed; } }        
        private string sEditorPath;
        public string EditorPath
        {
            get { return sEditorPath; }
        }
        public string BuildOutputEditorPath
        {
			get { return setupInfo.sCodeLocation + @"Output\DebugEditorsXenon\Jade_edd_xe.exe"; }
		}
		public string BuildOutputSpeedEditorPath
		{
			get { return setupInfo.sCodeLocation + @"Output\SpeedEditorsXenon\Jade_edr_xe.exe"; }
		}

        public string MapListPath
        {
            get { return setupInfo.sMapList; }
        }

        public string WorkDir
        {
            get { return setupInfo.sWorkDir; }
        }
        private string sWorkBFLocation;
        private string sAICodeLocation;
        private string sJadeSolution;		
        private string sShaderDirSource;
        private string sShaderDirDest;
        #endregion

        #region build targets
        private string sBuildTargetXenonDebug   = @"DebugXenon";
        private string sBuildTargetXenonRelease = @"ReleaseXenon";
        private string sBuildTargetXenonFinal   = @"FinalXenon";
        
		private string sBuildTargetDebugEditors = @"DebugEditorsXenon";
		private string sBuildTargetSpeedEditors = @"SpeedEditorsXenon";

		private string sBuildTargetReleaseXProfile = @"ReleaseXProfile";
        #endregion

        #region constants
		private const string sMainFixLevel = "_main/_main_bootup/_main_bootup.wol";
        #endregion

		private	string sCommonEditorArgs = @"/nocheck /nokeyserver /noperforce /Z /G5 "; 
		public string CommonEditorArgs
		{
			get { return sCommonEditorArgs; }
		}
		
		// -------------------------------------------------
        //
        static public void Binarize(Setup setupInfo)
        {
            BinProcess binProcess = new BinProcess(setupInfo);    

            binProcess.PerformSteps();

            // Make a backup of the binerr file
			string sTime = System.DateTime.Now.ToShortDateString() + @"_" + System.DateTime.Now.ToShortTimeString();
			sTime = sTime.Replace("/", "");
			sTime = sTime.Replace(':', 'h');
			
            string newFile = Path.GetDirectoryName(Application.ExecutablePath) + @"\binerr_" + sTime + ".log";
            FileSystem.CopyIfExist(setupInfo.sWorkDir + "binerr.log", newFile);
        }

        public BinProcess(Setup setupInfo)
        {
            // Make sure all fields are filed in correctly
            string errorInfo;
            if (!ValidateSetup(setupInfo, out errorInfo))
            {
                throw(new ApplicationException(errorInfo));
            }

            if (logIF == null)
                logIF     = new Logging(setupInfo.sLogFileName, setupInfo.bLogOnTop);
            if (appLogIF == null)
                appLogIF  = new Logging(setupInfo.sLogDetailsFileName, setupInfo.bLogOnTop);

	        this.setupInfo = setupInfo;

            logIF.LogWithTimeStamp("Binarization for following languages: " + setupInfo.sLanguages);
            sCommonEditorArgs += @"/K[" + setupInfo.sLanguages + @"] ";

            //
            // TODO: Add constructor logic here
            //
			if (!setupInfo.sWorkDir.EndsWith(@"\"))
				setupInfo.sWorkDir += @"\";
			if (!setupInfo.sCodeLocation.EndsWith(@"\"))
				setupInfo.sCodeLocation += @"\";

			sWorkBFLocation  = setupInfo.sWorkDir + @"Rayman4.bf";
			
			sAICodeLocation  = setupInfo.sCodeLocation + @"Libraries\AIinterp\Sources\";
            sJadeSolution    = setupInfo.sCodeLocation + @"Main\Jade.sln";	
			sEditorPath      = setupInfo.sWorkDir + @"Jade.exe";
            sShaderDirSource = setupInfo.sCodeLocation + @"Libraries\XenonGraphics\Shaders\";
            sShaderDirDest   = setupInfo.sWorkDir + @"Shaders\";
        }

        private void VerifyEditorIsPresent(string msg)
        {
            if (!File.Exists(EditorPath))
            {
                processFailed = true;
                throw(new ApplicationException("You need to have Jade_edd_xe built..." + msg));
            }
        }

        private void PerformSteps()
        {
            logIF.Log("");
            logIF.LogWithTimeStamp("Starting binarization");																					
            processFailed = false;

            // Creation of directories and everything
            FileSystem.CreateDirectories(setupInfo.sWorkDir, true);
    		
            // Clean directory (Working and AI)			
            if (setupInfo.bCleanDir)
            {
                logIF.LogWithTimeStamp("Cleaning directories");
                FileSystem.DeleteAllFilesInDirectory(setupInfo.sWorkDir);
            }

            // Getting the BF
            if (setupInfo.bGetBF)
            {
                logIF.LogWithTimeStamp("Copying BF");
                if (File.Exists(setupInfo.sBFLocation))
                {
                    FileSystem.CopyFile(setupInfo.sBFLocation, sWorkBFLocation);
                }
                else
                {			
                    string errMsg = "Original BF is missing!";
                    logIF.LogWithTimeStamp(errMsg);
					if (!setupInfo.bQuiet)		
                        MessageBox.Show(errMsg,"No BF",MessageBoxButtons.OK,MessageBoxIcon.Error);
                    processFailed = true;
                    return;
                }
            }

            // Sync in perforce
            if (setupInfo.bPerfoceSync)
            {
                String syncTo = "";

                if (setupInfo.bPerforceSyncToHead)
                    syncTo = "";
                else if (setupInfo.bPerforceSyncToChangelist)
                {
                    if (setupInfo.iPerfoceSyncToChangelistNumber > 0)
                    {
                        syncTo = @"@" + setupInfo.iPerfoceSyncToChangelistNumber.ToString();
                    }
                    else
                    {
                        // TODO: logIF.Log warning.. no changelist specified
                    }
                }

                logIF.LogWithTimeStamp("Synching in perforce");
                SynchroniseSources(syncTo);				
            }

            // EDITOR WITHOUT AI2C COMPILATION
            // -------------------------------
            if (setupInfo.sEditorWithAI2CLocation == String.Empty) // no need to build editor if we already have the one with AI2C
            {
                if (setupInfo.sEditorLocation != String.Empty)
                {
                    FileSystem.CopyFile(setupInfo.sEditorLocation, WorkDir + @"Jade_edd_xe_NO_AI.exe");
                }
                else
                {            
                    // Build editor
                    if (setupInfo.bBuildDebugEditors)
                    {
                        logIF.LogWithTimeStamp("Building editor");
						// todo: provide the rebuild in ui
                        if (!BuildTarget(sBuildTargetDebugEditors, true))
                        {	
                            logIF.LogWithTimeStamp("Error while compiling debug editor...");					
                            processFailed = true;
                            return;
                        }	
                        FileSystem.CopyFile(BuildOutputEditorPath, WorkDir + @"Jade_edd_xe_NO_AI.exe");
                    }

                    // Build speed editor
                    if (setupInfo.bBuildSpeedEditors)
                    {
                        logIF.LogWithTimeStamp("Building speed editor");
                        if (!BuildTarget(sBuildTargetSpeedEditors, true))
                        {	
                            logIF.LogWithTimeStamp("Error while compiling speed editor...");					
                            processFailed = true;
                            return;
                        }	
                        FileSystem.CopyFile(BuildOutputSpeedEditorPath, WorkDir + @"Jade_edr_xe_NO_AI.exe");
                    }
                }
            }

            // Copy Jade to work dir if not already there (use speed editor if its there.. otherwise debug editor)
//            if (File.Exists(WorkDir + @"Jade_edr_xe_NO_AI.exe"))
//				FileSystem.CopyFile(WorkDir + @"Jade_edr_xe_NO_AI.exe", EditorPath);
			if (File.Exists(WorkDir + @"Jade_edd_xe_NO_AI.exe"))
				FileSystem.CopyFile(WorkDir + @"Jade_edd_xe_NO_AI.exe", EditorPath);

            if (setupInfo.bGenerateAI2C ||
                setupInfo.bCompileAI || setupInfo.bPreprocessMaps || setupInfo.bDeleteAllBinData || setupInfo.bBinMaps || setupInfo.bCleanBF)
            {
                FileSystem.CopyFile(setupInfo.sCodeLocation + @"Tools\XenonTexture\Library\XenonTexture.DLL", setupInfo.sWorkDir + @"XenonTexture.DLL");
                CopyShaders();
            }


            // EDITOR WITH AI2C COMPILATION
            // ----------------------------

            if (setupInfo.sEditorWithAI2CLocation != String.Empty)
            {
                FileSystem.CopyFile(setupInfo.sEditorWithAI2CLocation, WorkDir + @"Jade_edd_xe_WITH_AI.exe");
            }
            else
            {  
                // Compiling Interpreted AI
                if (setupInfo.bCompileAI)
                {
                    VerifyEditorIsPresent("to compile AI (not in C)");

                    logIF.LogWithTimeStamp("Compiling AI (not in C)");
                    if (!CompileAIWithoutC())
                    {					
                        logIF.LogWithTimeStamp("Error with C compilation...");					
                        processFailed = true;
                        return;
                    }				
                }

                // Compiling AI in C (include building editor)
                if (setupInfo.bGenerateAI2C)
                {
                    VerifyEditorIsPresent("to compile AI in C");

                    logIF.LogWithTimeStamp("Compiling AI with C");
                    CompileAIWithC();	
                    HackAI2C();
                    // Copy AI directory to keep AI2C
                    try
                    {                    
                        FileSystem.DeleteAllDirectory(WorkDir + @"AIinterp");
                    }
                    catch(System.IO.DirectoryNotFoundException)
                    {                    
                    }
                    FileSystem.CopyDirectory(sAICodeLocation, WorkDir + @"AIinterp");


                    logIF.LogWithTimeStamp("Building Editor with C");
					
                    if (!BuildTarget(sBuildTargetDebugEditors, true))
                    {					
                        logIF.Log("Error while compiling debug editor with C...");					
                        processFailed = true;
                        return;
                    }	
                    FileSystem.CopyFile(BuildOutputEditorPath, WorkDir + @"Jade_edd_xe_WITH_AI.exe");

                    if (setupInfo.bBuildSpeedEditors)
                    {
                        logIF.LogWithTimeStamp("Building Speed Editor with C");
                        if (!BuildTarget(sBuildTargetSpeedEditors, true))
                        {					
                            logIF.Log("Error while compiling speed editor with C...");					
                            processFailed = true;
                            return;
                        }	
                        FileSystem.CopyFile(BuildOutputSpeedEditorPath, WorkDir + @"Jade_edr_xe_WITH_AI.exe");
                    }
                }
            }

			// Copy Jade to work dir if not already there
//			if (File.Exists(WorkDir + @"Jade_edr_xe_WITH_AI.exe"))
//				FileSystem.CopyFile(WorkDir + @"Jade_edr_xe_WITH_AI.exe", EditorPath);
			if (File.Exists(WorkDir + @"Jade_edd_xe_WITH_AI.exe"))
				FileSystem.CopyFile(WorkDir + @"Jade_edd_xe_WITH_AI.exe", EditorPath);

            // Preprocess Fix & All Maps
            if (setupInfo.bPreprocessMaps)
            {
                VerifyEditorIsPresent("to preprocess maps");

                logIF.LogWithTimeStamp("Preprocess maps");
                PreprocessAll(setupInfo.alMapList);				
            }

            // Binarisation
            if (setupInfo.bBinMaps)
            {
                VerifyEditorIsPresent("to binarize maps");

                logIF.LogWithTimeStamp("Binarisation of maps");
                BinAll(setupInfo.alMapList);				
            }

            // Clean
            if (setupInfo.bCleanBF)
            {
                VerifyEditorIsPresent("to clean the BF");

                logIF.LogWithTimeStamp("Cleaning BF");
                CleanFinal();				
            }
    		
            // Making Engines version (SPE, INOland + build)
            if (setupInfo.bBuildDebugXenon)
            {
                logIF.LogWithTimeStamp("Compiling Xenon Engines");
                if(!BuildTarget(sBuildTargetXenonDebug, true))
                {					
                    logIF.LogWithTimeStamp("Error while compiling engines version...");
                    processFailed = true;
                    return;
                }	
            }
            if (setupInfo.bBuildReleaseXenon)
            {
                if(!BuildTarget(sBuildTargetXenonRelease, true))
                {					
                    logIF.LogWithTimeStamp("Error while compiling engines version...");
                    processFailed = true;
                    return;
                }				
            }
            if (setupInfo.bBuildFinalXenon)
            {
                if(!BuildTarget(sBuildTargetXenonFinal, true))
                {					
                    logIF.LogWithTimeStamp("Error while compiling engines version...");
                    processFailed = true;
                    return;
                }				
            }                

            if (setupInfo.bBuildReleaseXProfile)
            {
                if (!BuildTarget(sBuildTargetReleaseXProfile, true))
                {
                    logIF.LogWithTimeStamp("Error while compiling engines version...");
                    processFailed = true;
                    return;
                }
            }
            // Deploy
            logIF.LogWithTimeStamp("Deploying engines to work directory");
            DeployTargets();
        }

        private void SynchroniseSources(string sSyncTo)
        {

            RunAppAndWait("Synchronizing Sources", "p4", @"sync " + setupInfo.sCodeLocation + @"..." + sSyncTo);		
            // Force sync on AI lib
			FileSystem.DeleteAllFilesInDirectory(sAICodeLocation + @"AI2C_ofc");
            RunAppAndWait("Synchronizing AI", "p4", @"sync -f " + sAICodeLocation + @"..." + sSyncTo);		
        }
	
        private bool BuildTarget(string sBuildTarget, bool bRebuild)
        {
            string sBuildFlag;
			bool bBuildOk = false;

            if (bRebuild)
                sBuildFlag = @"/rebuild";
            else
                sBuildFlag = @"/build";

			// incredibuild command
			bBuildOk = RunAppAndWait("Building " + sBuildTarget, setupInfo.sDevEnv, sBuildFlag + " /CFG=\"" + sBuildTarget + "\" " + sJadeSolution);
			
			// msdev command
			bBuildOk = RunAppAndWait("Building " + sBuildTarget, setupInfo.sDevEnv, sBuildFlag + " " + sBuildTarget + " " + sJadeSolution);

			if (!bBuildOk)
            {
                string errMsg = "Error while building";
                logIF.LogWithTimeStamp(errMsg);
                if (!setupInfo.bQuiet)		
                    MessageBox.Show(errMsg,"ERROR",MessageBoxButtons.OK,MessageBoxIcon.Error);
                processFailed = true;
            }

            return bBuildOk;
        }
        private bool CompileAIWithoutC()
        {   
            if (!File.Exists(sEditorPath))
            {
                string errMsg = "No editor has been build!";
                logIF.LogWithTimeStamp(errMsg);
                if (!setupInfo.bQuiet)		
                    MessageBox.Show(errMsg,"Editor Needed",MessageBoxButtons.OK,MessageBoxIcon.Error);
                processFailed = true;
                return false;
            }
			
            RunAppAndWait("Compiling AI Without C", sEditorPath, sCommonEditorArgs + "/D_FINAL_ /C" + setupInfo.sDefaultLevel + " " + sWorkBFLocation);		

            return true;
        }

        private void PreprocessAll(ArrayList alMapList)
        {
            // Main Fix	
            RunAppAndWait("Preprocessing Main Fix", sEditorPath, sCommonEditorArgs + "/O /LQ" + sMainFixLevel + " " + sWorkBFLocation);		

            // Base objects
            RunAppAndWait("Preprocessing Common Objects", sEditorPath, sCommonEditorArgs + "/xpk " + sWorkBFLocation);		

            // All Maps
            System.Collections.IEnumerator myEnumerator = alMapList.GetEnumerator();
            while ( myEnumerator.MoveNext() )
            {
                RunAppAndWait("Preprocessing Map " + myEnumerator.Current, sEditorPath,sCommonEditorArgs + "/O /LQ" + myEnumerator.Current + " " + sWorkBFLocation);		
            }

            // Wipe DDS
            RunAppAndWait("Wiping DDS", sEditorPath, sCommonEditorArgs + "/wipedds " + sWorkBFLocation);

            // Clean the BF
            RunAppAndWait("Cleaning BF after DDS wipe", sEditorPath, sCommonEditorArgs + "/clean " + sWorkBFLocation);

            // Continue using the clean BF
            FileSystem.DeleteIfExist(WorkDir + @"Rayman4_DDS.bf");
            File.Move(WorkDir + @"Rayman4.bf",       WorkDir + @"Rayman4_DDS.bf");
            File.Move(WorkDir + @"Rayman4_clean.bf", WorkDir + @"Rayman4.bf");
        }

        private void CompileAIWithC()
        {
            RunAppAndWait("Compiling AI With C", sEditorPath, sCommonEditorArgs + "/Y(" + sAICodeLocation+ @") /D_FINAL_ /C" + setupInfo.sDefaultLevel + " " + sWorkBFLocation);		
        }

        private void BinAll(ArrayList alMapList)
        {
            // If NOT asked to delete all data
            // it means user wants to RE-BIN the maps
            
            // The call to Jade /S kills all the bin data
            if (setupInfo.bDeleteAllBinData)
            {            
                RunAppAndWait("Binarizing Main Fix", sEditorPath, sCommonEditorArgs + "/LQ" + sMainFixLevel + " /S " + sWorkBFLocation);			
                RunAppAndWait("Binarizing Main Fix", sEditorPath, sCommonEditorArgs + "/LQ" + sMainFixLevel + " /R " + sWorkBFLocation);			
                FileSystem.CopyIfExist(setupInfo.sWorkDir + "INOlang.c", setupInfo.sCodeLocation + @"Libraries\SDK\Sources\INOut\INOlang.c");
            }

            // All Maps
            System.Collections.IEnumerator myEnumerator = alMapList.GetEnumerator();
            while ( myEnumerator.MoveNext() )
            {
                if (!setupInfo.bDeleteAllBinData)
                {
                    string nameWithoutKey = myEnumerator.Current.ToString();
                    int lastDotPos = nameWithoutKey.LastIndexOf(".");
                    nameWithoutKey = nameWithoutKey.Substring(0, lastDotPos);
                    RunAppAndWait("Removing Bin Data from Map " + nameWithoutKey, sEditorPath, sCommonEditorArgs + "/cleanbinfiles(" + nameWithoutKey + ") " + sWorkBFLocation);				
                }
            
                RunAppAndWait("Binarizing Map " + myEnumerator.Current, sEditorPath, sCommonEditorArgs + "/LQ" + myEnumerator.Current + " /B " + sWorkBFLocation);				
            }
        }

        private void CleanFinal()
        {
            RunAppAndWait("Cleaning BF", sEditorPath,sCommonEditorArgs + "/F " + sWorkBFLocation);			
		}        

        private void DeployTargets()
        {
            if (setupInfo.bBuildDebugEditors)
                FileSystem.CopyIfExist(setupInfo.sCodeLocation + @"Output\DebugEditors\Jade_edd_xe.exe", setupInfo.sWorkDir);
            if (setupInfo.bBuildSpeedEditors)
			    FileSystem.CopyIfExist(setupInfo.sCodeLocation + @"Output\Speed Editors\Jade_edr_xe.exe", setupInfo.sWorkDir);
            if (setupInfo.bBuildFinalXenon)
			    FileSystem.CopyIfExist(setupInfo.sCodeLocation + @"Output\FinalXenon\Jade_XeF.exe",      setupInfo.sWorkDir);
            if (setupInfo.bBuildDebugXenon)
                FileSystem.CopyIfExist(setupInfo.sCodeLocation + @"Output\DebugXenon\Jade_XeD.exe",      setupInfo.sWorkDir);
            if (setupInfo.bBuildReleaseXenon)
                FileSystem.CopyIfExist(setupInfo.sCodeLocation + @"Output\ReleaseXenon\Jade_XeR.exe",    setupInfo.sWorkDir);                                
            if (setupInfo.bBuildReleaseXProfile)
                FileSystem.CopyIfExist(setupInfo.sCodeLocation + @"Output\ReleaseXProfile\Jade_XeRProfile.exe",    setupInfo.sWorkDir);                                
        }


        static public bool ReadMapList(BinProcess.Setup setupInfo, out ArrayList alMapList)
        {
            alMapList = new ArrayList();

            if (File.Exists(setupInfo.sMapList))
            {
                System.IO.StreamReader mapFile = new System.IO.StreamReader(setupInfo.sMapList);
                string mapName = null; 
							
                // Display the file contents.
                while ((mapName = mapFile.ReadLine()) != null)
                {
                    if (!(mapName.StartsWith(";")))
                    {	
                        alMapList.Add(mapName);						
                    }				
                }
			
                mapFile.Close();
            }
            else
            {
                return false;
            }

            return true;
        }		

        bool RunAppAndWait(String sInfo, String AppName, String AppArgs)
        {
            appLogIF.Log("");
            appLogIF.LogWithTimeStamp(sInfo);
            appLogIF.Log("-----------------------------------");
            appLogIF.Log(AppName + " " + AppArgs);

            System.Diagnostics.Process proc = new System.Diagnostics.Process();
            proc.StartInfo.UseShellExecute = false;
            proc.StartInfo.RedirectStandardOutput = true;
            proc.StartInfo.RedirectStandardError = true;
            proc.StartInfo.CreateNoWindow = true;
            proc.StartInfo.FileName  = AppName;
            proc.StartInfo.Arguments = AppArgs;
            proc.Start();

            string str;
            while ((str = proc.StandardOutput.ReadLine()) != null)
                appLogIF.Log(str);

            proc.WaitForExit(); 	

			return (proc.ExitCode == 0);
        }
		
        static public bool ValidateSetup(Setup setupInfo, out string errorInfo)
        {
            string info = "";
            errorInfo = "";

            if (!File.Exists(setupInfo.sBFLocation))
                info += "Main BF path is invalid\n";
            if (!File.Exists(setupInfo.sMapList))
                info += "Map list path is invalid\n";

            if (setupInfo.sEditorLocation != String.Empty && !File.Exists(setupInfo.sEditorLocation))
                info += "Editor path is invalid\n";
            if (setupInfo.sEditorWithAI2CLocation != String.Empty && !File.Exists(setupInfo.sEditorWithAI2CLocation))
                info += "EditorWithAI2C path is invalid\n";

            if (setupInfo.sWorkDir == "")
                info += "Bin dir path is invalid\n";
            if (!File.Exists(setupInfo.sDevEnv))
                info += "DevEnv path is invalid\n";

            if (info != "")
            {
                errorInfo = "Configuration validation FAILED : \n"; 
                errorInfo += info;
                return false;
            }
            else
                return true;
        }

        static public void GenerateMapList(BinProcess.Setup setupInfo, out ArrayList alMapList)
        {
            BinProcess binProcess = new BinProcess(setupInfo);    

			if (!File.Exists(binProcess.BuildOutputEditorPath))
			{
				string errMsg = "No editor to generate map list!";
				logIF.LogWithTimeStamp(errMsg);
				if (!setupInfo.bQuiet)
				{
					MessageBox.Show(errMsg, "Editor Needed", MessageBoxButtons.OK,MessageBoxIcon.Error);
				}
				processFailed = true;

				alMapList = null;

				return;
			}

			binProcess.RunAppAndWait("Generate Map List", binProcess.BuildOutputEditorPath, binProcess.CommonEditorArgs + " /makemaplist(" + binProcess.MapListPath + ") " + setupInfo.sBFLocation); // use original BF instead of copy.. it might not be there yet			
            BinProcess.ReadMapList(setupInfo, out alMapList);
        }

        private void CopyShaders()
        {
            FileSystem.CreateDirectories(sShaderDirDest, false);
            FileSystem.CopyDirectory(sShaderDirSource, sShaderDirDest);
            string name = @"XeSharedDefines.h";
            FileSystem.CopyFile(sShaderDirSource + @"..\" + name, sShaderDirDest + name);
        }

        static public void ShowLogs(bool bShow)
        {
			if (appLogIF!=null)
                appLogIF.Show(bShow);
			if (logIF!=null)
                logIF.Show(bShow);
        }

        static public void LogOnTop(bool bOnTop)
        {
            if (appLogIF!=null)
                appLogIF.OnTop(bOnTop);
            if (logIF!=null)
                logIF.OnTop(bOnTop);
        }
    }
}
