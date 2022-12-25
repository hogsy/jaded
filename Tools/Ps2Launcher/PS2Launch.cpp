// PS2Launch.cpp : Defines the entry point for the console application.
//

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include <tchar.h>
/*#include <iostream.h>
*/

#include <windows.h>
#include <mmsystem.h>
#include <tlhelp32.h>
#include <iostream>	
#include <string>
#include <winsock.h>
#include <time.h>
using namespace std;

#define DEBUGIPFILE "x:/DebugIP.txt"
#define DEFAULTFILE "default.txt"
#define ELF_SOURCE "Y:/Ps2Launcher/JadePs2_r.elf"
#define INI_IN_ELF "x:/JadePsx2.ini"
#define INI_TO_PATCH "x:/JadePsx2DebugStation.ini"

void strReplace(char *SourceString , char *DestString , char *SourceWord , char *Dest )
{
	int i;
	while (*SourceString)
	{
		i = 0;
		while ((SourceString[i] == SourceWord[i]) && (SourceWord[i]))
		{
			i++;
		}
		if (SourceWord[i] == 0)
		{
			char *DestSave;
			SourceString += i;
			DestSave = Dest;
			while (*Dest)
				*(DestString++) = *(Dest++);
			Dest = DestSave;
		} else
			*(DestString++) = *(SourceString++);
	}
	*(DestString++) = *(SourceString++);

}

int Patch(char *pElf , int sizeofelf , char *MyIP)
{
	char String2Find[1024] ;
	char String2Put[1024];
	char StringoutPut[1024];
	char DSTWORD[1024];
	FILE *INI_SOURCE;
	FILE *INI_DEST;
	int i,j;


	
	sprintf(String2Find, "host0:%s",INI_IN_ELF);
	sprintf(DSTWORD, "atfile:%s,",MyIP);
	INI_SOURCE	= fopen(INI_IN_ELF,"rt");
	INI_DEST	= fopen(INI_TO_PATCH,"wt");
	if (!INI_SOURCE)
	{
		cout << "Could not find " << INI_IN_ELF << endl;
		return 1;
	}
	if (!INI_DEST)
	{
		cout << "Could not open " << INI_TO_PATCH << endl;
		return 1;
	}
	while (fgets(String2Put,1024,INI_SOURCE))
	{
		strReplace(String2Put , StringoutPut , "host0:" , DSTWORD);
		fprintf(INI_DEST,StringoutPut);
	}
	fclose(INI_SOURCE);
	fclose(INI_DEST);

	sprintf(String2Put, "atfile:%s,%s",MyIP,INI_TO_PATCH);

	for (i = 0 ; i < sizeofelf ; i++)
	{
		j = 0;
		while (String2Find[j] == pElf[j])
		{
			if (String2Find[j] == 0) 
			{
				j = 0;
				while (String2Put[j])
				{
					*(pElf++) = String2Put[j++];
				}
				*(pElf++) = 0;
				return 0;
			}
				
			j++;
		}
		pElf++;
	}
	return -1;
	
}


int GivemeMyIP(char *IPADRESS)
{
	char ac[80];
	if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
		cout << "Error " << WSAGetLastError() <<
			" when getting local host name." << endl;
		return 1;
	}
	struct hostent *phe = gethostbyname(ac);
	if (phe == 0) {
		cout << "Yow! Bad host lookup." << endl;
		return 1;
	}

	for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		strcpy(IPADRESS , inet_ntoa(addr));
	}
	return 0;
}

HANDLE MyStdOut; 
char PingCommand[256];
int Timer;
void CoutIP(char *IP)
{

	Timer = timeGetTime(); 
	sprintf(PingCommand , "ping %s -n 1 -w 1000 >> C:/Clakos.txt",IP);
	system(PingCommand); 
	Timer = timeGetTime() - Timer ; 
	if (Timer >= 900)
	{
		SetConsoleTextAttribute(MyStdOut,0xc);
		cout << " " << IP <<" (not respond) ";
	}
	else
	{
		SetConsoleTextAttribute(MyStdOut,0xa);
		cout << " " << IP <<" ";
	}
	
	SetConsoleTextAttribute(MyStdOut,0x7);
}

int _tmain(int argc, _TCHAR* argv[])
{
	WSAData wsaData;
	FILE *Prout ;
	char MyIP[256];
	char DebugIP[256] = "0.0.0.0";
	int ulSizeOfElf,Valid;
	char *pAllTheElf;

	unsigned char buffer[4096];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	STARTUPINFO si2;
	PROCESS_INFORMATION pi2;

	MyStdOut = GetStdHandle(STD_OUTPUT_HANDLE); 

	SetConsoleTextAttribute(MyStdOut,0x1f);
	cout << " PS2 DEBUG LAUNCHER V3 " << endl;
	cout << "" << endl;
	SetConsoleTextAttribute(MyStdOut,0x7);
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) 
	{
		cout << "Could not retreive IP of this PC !!" << endl;
		goto END;
	}
	if (GivemeMyIP(MyIP)) 
	{
		cout << "Could not retreive IP of this PC !!" << endl;
		goto END;
	} 
	WSACleanup();

	Prout  = fopen(DEBUGIPFILE,"rt" );
	DebugIP [0] = 0;
	if (Prout)
	{
		fgets( DebugIP , 256, Prout);
		fclose(Prout);
	} 

	cout << "\n";
	Valid = 0;

	while (!Valid)
	{
		char input ;
		CoutIP(DebugIP);
		cout << "(press ENTER if OK else type new number) : ";
		SetConsoleTextAttribute(MyStdOut,0x0f);
		input = getchar();
		if (input != 0x0a)
		{
			int Counter;
			cin >> DebugIP ;
			getchar();
			Counter = 255;
			while(Counter--) DebugIP[Counter + 1] = DebugIP[Counter];
			DebugIP[0] = input;
		}
		SetConsoleTextAttribute(MyStdOut,0x07);
		Timer = timeGetTime(); 
		sprintf(PingCommand , "ping %s -n 1 -w 1000 >> C:/Clakos.txt",DebugIP);
		system(PingCommand); 
		Timer = timeGetTime() - Timer ; 
		if (Timer >= 900)
			Valid = 0;
		else
			Valid = 1;
	}

	
	Prout	= fopen(DEBUGIPFILE,"wt");
	if (Prout)
	{
		fprintf(Prout,DebugIP);
		fclose(Prout);
	}

	SetConsoleTextAttribute(MyStdOut,0x0f);
	
	cout << "\nPATCH ELF .";

	Prout  = fopen(ELF_SOURCE,"rb" );
	if (!Prout)
	{
		cout << ELF_SOURCE << " is not here !" << endl;
		goto END;
	}
	ulSizeOfElf = 0;
	/* Compute size of elf */
	while( !feof( Prout ) )
	{
		ulSizeOfElf += (int)fread( buffer, sizeof( char ), 4096 , Prout );
		if ((ulSizeOfElf & 0xfffff) == 0) cout << ".";
	}
	fclose(Prout);
	cout << "\n";

	Prout  = fopen(ELF_SOURCE,"rb" );
	pAllTheElf = (char *)malloc(ulSizeOfElf);
	fread( pAllTheElf , 1 , ulSizeOfElf , Prout );
	fclose(Prout);


	if (Patch(pAllTheElf , ulSizeOfElf , MyIP))
	{
		cout << "\nthis is not the good " << ELF_SOURCE << "!!" << endl;
		goto END;
	}

	Prout  = fopen("x:/j2.elf","wb" );
	fwrite( pAllTheElf , 1 , ulSizeOfElf , Prout );
	fclose(Prout);


	free(pAllTheElf);

	/* Create default.txt */

	Prout  = fopen(DEFAULTFILE,"wt" );
	fprintf(Prout ,"%s\n", MyIP);
	fprintf(Prout ,"%s\n", DebugIP );
	fclose(Prout);

	cout << "LAUNCH ELF" << endl;
	SetConsoleTextAttribute(MyStdOut,0x07);
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	// Start the child process. 
	if( !CreateProcess( NULL, // No module name (use command line). 
		"rfsv", // Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		0,                // No creation flags. 
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi )             // Pointer to PROCESS_INFORMATION structure.
	) 
	{
		cout << "* CreateProcess rfsv failed ... *" << endl;
		goto END;
	}

	
	ZeroMemory( &si2, sizeof(si) );
	si2.cb = sizeof(si);
	ZeroMemory( &pi2, sizeof(pi) );

	// Start the child process. 
	if( !CreateProcess( NULL, // No module name (use command line). 
		"\"mcpclient.exe\" -xx:/j2.elf", // Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		0,                // No creation flags. 
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&si2,              // Pointer to STARTUPINFO structure.
		&pi2 )             // Pointer to PROCESS_INFORMATION structure.
	) 
	{
		cout << "* CreateProcess mcpclient failed ... *" << endl;
		goto END;
	}

END:
	while(1){};
	return 0;
}

