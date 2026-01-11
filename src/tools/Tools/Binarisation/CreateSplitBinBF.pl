#!/bin/perl

use strict;
use File::Copy;
use File::Copy::Recursive;
use File::Find;
use File::Path;

my @asBinMachine;

# Add all bin machines here
push(@asBinMachine,	"mtp-icaroulle");

# The main machine is the computer with compiled engines
my $sBinDir				= "bin"; 
my $sMainMachineWorkDir = "\\\\".$asBinMachine[0]."/".$sBinDir;   
my $sRootDir			= "D:/WORK/Binarization/";
my $sWorkDir			= $sRootDir."Merge/";
my $sDeployDir			= $sRootDir."Final/";
my $sProductionDir		= "D:/WORK/Binarization/Production/";

# You can override the deploy dir by giving it as a parameter
if ($#ARGV == 0)	
{
	$sDeployDir = $ARGV[0];
}

my $sEditor			= $sWorkDir."/Jade_edd_xe.exe";
my $sEditorArgs		= "/nocheck /noperforce /nokeyserver /G5";   
my $sDataDir		= $sWorkDir."/data";  
my $sTextureDir     = $sWorkDir."/data_tex"; 
my $sBfTitle		= "Rayman4";
my $sTextureBfName  = "RM4Textures.bf";
my $sMapBfName		= "RM4Maps.bf";
my $sTextureBf		= $sWorkDir."/".$sTextureBfName;
my $sMapBf			= $sWorkDir."/".$sMapBfName;

mkdir($sRootDir);
mkdir($sWorkDir);
mkdir($sWorkDir."/Shaders");
mkdir($sDataDir);
mkdir($sTextureDir);
chdir($sWorkDir);

my @asFileList;

# First copy shaders so Jade wont complain when starting
CopyShaders($sMainMachineWorkDir, $sWorkDir);

copy($sMainMachineWorkDir."/Jade_edd_xe_NO_AI.exe", $sWorkDir."/Jade_edd_xe.exe");
chmod(0777, $sWorkDir."/Jade_edd_xe.exe");

# Then copy all _clean.bf files to _clean_GROUP_X.bf to our work folder
my $nCount = 0;
foreach my $esMachine (@asBinMachine)
{
	$nCount++;
    my $sBfGroupFile = "".$sBfTitle."_clean_GROUP_".$nCount.".bf";
	# make sure the file is not already there
	unlink($sBfGroupFile);
    
    my $sOriginalBfName = "\\\\".$esMachine."/".$sBinDir."/".$sBfTitle."_clean.bf";
    my $sNewBfName = $sWorkDir."/".$sBfGroupFile;
    
	print("Copying ".$sOriginalBfName." to ".$sNewBfName."\n");
	if (!-f($sOriginalBfName))
	{
		print("File not found $sOriginalBfName.. cannot continue!\n"); 
		exit;
	}
	else
	{
		if (copy($sOriginalBfName, $sNewBfName))
		{      
		    my $sCommand = $sEditor." ".$sEditorArgs." /exporttokeys ".$sDataDir." ".$sBfGroupFile;
			print($sCommand."\n");
			system($sCommand);           
		}
	}
}

# Now that all files are exported, move the textures files (0xFF8*) to another dir
BuildFileList($sDataDir);

foreach my $sFile (@asFileList)
{
    if ( $sFile =~ /0xFF8/ )
    {
        my $sNewName = $sFile;
        $sNewName =~ s/$sDataDir/$sTextureDir/;
        print("Moving ".$sFile." to ".$sNewName."\n");
        my $sPath = $sNewName;
        $sPath =~ s/0x[A-F,0-9]{8}//;
        mkpath($sPath);
        rename($sFile, $sNewName);
    }
}

# Then create the new BFs
my $sCommand = $sEditor." ".$sEditorArgs." /importfromkeys ".$sDataDir." ".$sMapBf;
print($sCommand."\n");
system($sCommand);

$sCommand = $sEditor." ".$sEditorArgs." /importfromkeys ".$sTextureDir." ".$sTextureBf;
print($sCommand."\n");
system($sCommand);

# Deployment
rmdir($sDeployDir);
mkdir($sDeployDir);
chdir($sDeployDir);
## TODO: clean deploy dir

# Move merged BFs
rename($sTextureBf,	$sTextureBfName);
rename($sMapBf,		$sMapBfName);

# Copy Engine
print("Copying Engine\n");
my $sEngine = "Jade_XeF.exe";
my $sEngineR = "Jade_XeR.exe";
copy($sMainMachineWorkDir."/".$sEngine, $sEngine);
copy($sMainMachineWorkDir."/".$sEngineR, $sEngineR);

# Copy Shaders
CopyShaders($sMainMachineWorkDir, $sDeployDir);

# Copy ini file
copy($sProductionDir."/Jade_xe.ini", "Jade_xe.ini");

# Copy Sounds
print("Copying Sounds\n");
my $sSoundDir = $sDeployDir."Sound/";
mkdir($sSoundDir);
File::Copy::Recursive::dircopy($sProductionDir."/Sound", $sSoundDir);

# Copy Videos
print("Copying Videos\n");
my $sVideoDir = $sDeployDir."Video/";
mkdir($sVideoDir);
File::Copy::Recursive::dircopy($sProductionDir."/Video", $sVideoDir);

# Copy Loading
print("Copying Loading\n");
my $sLoadingDir = $sDeployDir."Loading/";
mkdir($sLoadingDir);
File::Copy::Recursive::dircopy($sProductionDir."/Loading", $sLoadingDir);

# Copy images
print("Copying Images\n");
my $sImagesDir = $sDeployDir."Images/";
mkdir($sImagesDir);
File::Copy::Recursive::dircopy($sProductionDir."/Images", $sImagesDir);

# Copy Xlast
print("Copying Xlast\n");
my $sXlastDir = $sDeployDir."Xlast/";
mkdir($sXlastDir);
File::Copy::Recursive::dircopy($sProductionDir."/Xlast", $sImagesDir);

# Copy AI
print("Copying AI\n");
my $sAiDir = $sDeployDir."AIinterp/";
mkdir($sAiDir);
File::Copy::Recursive::dircopy($sMainMachineWorkDir."/AIinterp", $sAiDir);

# --------------------------------------------------------------------------------

sub CopyShaders($$)
{
	my ($src, $dst) = @_;
	
	mkdir($dst."/Shaders");
	print("Copying shaders\n");	
	CopyRecursive($src."/Shaders", $dst."/Shaders");
	unlink($dst."/Shaders/PCShaders.BIN");
}

sub CopyRecursive($$)
{
	my ($src, $dst)	= @_;	
	
	opendir(DIR, $src) or die "Can't opendir on a bin machine: $!";
	my $fileOrDir;
	while( ($fileOrDir=readdir(DIR)) )
	{
		my $fileName = $src."/".$fileOrDir;
		copy($fileName, $dst."/".$fileOrDir);
	}
	closedir(DIR);
}

sub BuildFileList($)
{
	my ($path) = @_;
	find(\&AddFileToList, $path);
}

sub AddFileToList()
{
	return unless -f;
	push(@asFileList, $File::Find::name);
}

