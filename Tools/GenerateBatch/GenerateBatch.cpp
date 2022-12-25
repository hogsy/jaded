#include "stdafx.h"
#include <io.h>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <direct.h>

////////////////////////////////////////////
// STRUCTS
////////////////////////////////////////////

void TreatDir(const char *_sDirName,FILE *logFile,int _iChangeListNb)
{
    struct _finddata_t fileinfo;
	intptr_t handle,code;
    struct _stat stFileStat;
    char sFileName[260];
    static int iWaitCounter = 0;
    static int iLineCounter = 0;

	// Open and parse the generated funcions files in order to get the functions defined and used.
    _chdir(_sDirName);
	handle = _findfirst("*",&fileinfo);
	code = handle;
	while (code != -1)
	{
        if (strcmp(".",fileinfo.name) && strcmp("..",fileinfo.name))
        {
            sprintf(sFileName,"%s\\%s",_sDirName,fileinfo.name);
            _stat(sFileName,&stFileStat);
            if (stFileStat.st_mode & _S_IFDIR)
            {
                TreatDir(sFileName,logFile,_iChangeListNb);
                _chdir(_sDirName);
            }
            else
            {
                if (!(iLineCounter--))
                {
                    iLineCounter = 100;
                    fprintf(logFile,"\np4 add -t binary+Fl -c %d",_iChangeListNb);
                }

                fprintf(logFile," %s",sFileName);

                if (!(iWaitCounter--))
                {
                    iWaitCounter = 1000;
                    printf("File added (1 in 1000): %s\n",sFileName);
                }
            }
        }
		code = _findnext(handle,&fileinfo);
	}
	code = _findclose(handle);
}


int _tmain(int argc, _TCHAR* argv[])
{
    int iChangeListNb;
	if (argc != 4 || !(iChangeListNb = atoi(argv[3])) )
	{
		printf("usage : GenerateBatch dst_batch path_keys changelist_number\n");
		exit(-1);
	}

	FILE *logFile = fopen(argv[1],"w");
    TreatDir(argv[2],logFile,iChangeListNb);
    fprintf(logFile,"\n\np4 submit -c %d\n",iChangeListNb);
    fprintf(logFile,"pause\n");
	fclose(logFile);

	return 0;
}


