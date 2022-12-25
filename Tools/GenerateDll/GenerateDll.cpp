// GenerateDll.cpp : Defines the entry point for the console application.
// In order to link correctly, the AIdllXXX.cpp files need #includes 
// for the generated functions they use.
// This exe parses the AIdllXXX.cpp files and the generated functions files
// in order to add the #includes needed.

#include "stdafx.h"
#include <io.h>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

////////////////////////////////////////////
// STRUCTS
////////////////////////////////////////////

// This struct defines a function and its dependencies.
struct Function
{
    // File in which the function is included
	std::string sFileName;

    // The name of the function
	std::string sFunctionName;

    // The possible functions called by this function (temp strings)
    std::vector<std::string> asCalledFunctions;

    // The functions called by this function (after we found where they are defined)
    std::vector<Function *> apCalledFunctions;
};

// All functions defined by all files.
std::vector<Function> apDefinedFunctions;


// This struct contains the file name of a model and functions it uses and defines.
/*struct ModelFunctions
{
	char sModelFileName[260];
	std::vector<std::string> asUsedFunctions;
	std::vector<ModelFunction> asDefinedFunctions;
};*/




FILE *logFile = NULL;

////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////

void AddIfNotPresent(std::vector<std::string> &_asStrings,const char *_sNewString)
{
	int i;
	for (i=0 ;i<_asStrings.size(); i++)
	{
		if (!strcmp(_asStrings[i].c_str(),_sNewString))
			return;
	}
	_asStrings.push_back(std::string(_sNewString));
}

void AddIfNotPresent(std::vector<std::string> &_asStringsToCheck,std::vector<std::string> &_asWhereToAdd,const char *_sNewString)
{
	int i;
	for (i=0 ;i<_asStringsToCheck.size(); i++)
	{
		if (!strcmp(_asStringsToCheck[i].c_str(),_sNewString))
			return;
	}
	_asWhereToAdd.push_back(std::string(_sNewString));
}

void CatPathFile(const char *_sPath,const char *_sFile,char *_sResult)
{
	int length = strlen(_sPath);
	if (_sPath[length-1] == '/' || _sPath[length-1] == '\\')
		sprintf(_sResult,"%s%s",_sPath,_sFile);
	else
		sprintf(_sResult,"%s/%s",_sPath,_sFile);
}


/*void AddFunctionsToLists(ModelFunctions *_pModel,std::vector<std::string> &_rasAIdllDefinedFunctions,std::vector<std::string> &_rasAIdllUsedFunctions)
{
	int i;
	for (i=0; i<_pModel->asDefinedFunctions.size(); i++)
		AddIfNotPresent(_rasAIdllDefinedFunctions,_pModel->asDefinedFunctions[i].c_str());

	for (i=0; i<_pModel->asUsedFunctions.size(); i++)
		AddIfNotPresent(_rasAIdllUsedFunctions,_pModel->asUsedFunctions[i].c_str());
}*/

char *ReadBufferFromFile(const char *_sFileName)
{
	int code;
	FILE * file = fopen(_sFileName,"r");
	char c = fgetc(file);
	int fileSize = 0;
	while (c != EOF)
	{
		fileSize++;
		c = fgetc(file);
	}

	/*code = fseek( file,0,SEEK_END);
	int fileSize = ftell(file);*/
	rewind(file);
	
	char *buffer = new char [fileSize+1];
	code = fread(buffer,fileSize,1,file);
	buffer[fileSize] ='\0';
	if (code == 0 && ferror(file))
	{
		printf("Error : problem during reading of %s\n",_sFileName);
		exit(-4);
	}

	fclose(file);
	return buffer;
}

bool bIsSpace(char c)
{
	return (c==' ') || (c=='\n') || (c=='\t') || (c=='\r');
}

bool bIsValidChar(char c)
{
	return ((c>='a') && (c<='z')) ||
		((c>='A') && (c<='Z')) ||
		((c>='0') && (c<='9')) ||
		(c == '_');
}



void ParseModel(const char *_sPath, const char *_sFileName)
{
	char sFullName[260];
	CatPathFile(_sPath,_sFileName,sFullName);

	// Put file in buffer (easier manipulation).
	char *buffer = ReadBufferFromFile(sFullName);

	// We search for '{', '}' and '('
	// Functions are defined outside of {...}
	// They are used inside of {...}
	// They are written before '('

	int bracesNb = 0; // nb of '{' minus nb of '}' encountered during parsing of file
	char *currentBuffer = buffer + strcspn(buffer,"{}("); // place of first char in "{}(" (or 0 if end of buffer)

	char bracket = *currentBuffer;
	while (bracket == '{' || bracket == '}' || bracket == '(')
	{
		if (bracket == '(')
		{
			// search before the '(' for a potentially valid procedure name.
			char *current = currentBuffer-1;

			// Eat spaces
			while (current >= buffer && bIsSpace(*current))
				current--;

			// Build name.
			char *endOfName = current+1;
			while (current >= buffer && bIsValidChar(*current))
				current--;

			current ++;
			if (current < endOfName)
			{
				// Potential name
				char sName[260];
				int i = 0;
				while (current != endOfName)
				{
					sName[i] = *current;
					current++;
					i++;
				}
				sName[i] = '\0';

				if (strcmp(sName,"for") && 
					strcmp(sName,"if") &&
					strcmp(sName,"while") &&
					strcmp(sName,"switch") &&
					strcmp(sName,"return") &&
					strcmp(sName,"RETURN") &&
					strcmp(sName,"sizeof") &&
					strcmp(sName,"case") &&
					strcmp(sName,"const") &&
					strcmp(sName,"delete") &&
					strcmp(sName,"new") &&
					strcmp(sName,"catch"))
				{
					// Potential function. It is defined if it is outside of {} 
					// and if after the name we find a '{' before a ';'.
					bool bIsDefinition;
					bool bIsUsefulFunction;

					// What do we find first ? ';' or '{' ?
					char *sFindSemiColon = strchr(currentBuffer,';');
					char *sFindOpeningBracket = strchr(currentBuffer,'{');
					bool bIsBracketFirst = (sFindSemiColon == NULL) || 
							((sFindOpeningBracket != NULL) && (sFindOpeningBracket < sFindSemiColon));

					if (bracesNb)
					{
						// inside {}. 
                        // The function may be used if :
                        // 1) we find a ';' before a '{'. 
                        // 2) we don't find the term "procedure" in the same line
						bIsDefinition = false;
                        if (!bIsBracketFirst)
                        {
                            // Search line for "procedure" between begining of line and end of name.
                            current = endOfName;
                            while ((current >= buffer) && (*current != '\n'))
                                current--;
                            
                            current = strstr(current,"procedure");
                            bIsUsefulFunction = (current == NULL) || (current>endOfName);
                        }
                        else
    						bIsUsefulFunction = false;
					}
					else
					{
						// outside {}. The function is only declared (not defined) if ';' is first.
						bIsDefinition = true;
						bIsUsefulFunction = bIsBracketFirst;
					}

					if (bIsUsefulFunction)
					{
						if (bIsDefinition)
                        {
                            Function myFunction;
                            myFunction.sFunctionName = sName;
                            myFunction.sFileName = _sFileName;

                            // Add function to list of defined functions
                            apDefinedFunctions.push_back(myFunction);
                        }
						else
                        {
                            // Add function to dependencies of last defined function.
                            AddIfNotPresent(apDefinedFunctions.back().asCalledFunctions,sName);
                        }
					}
				}
			}
		}
		else if (bracket == '{')
			bracesNb++;
		else // bracket == '}'
			bracesNb--;

		currentBuffer += 1+strcspn(currentBuffer+1,"{}("); // Search for next bracket.
		bracket = *currentBuffer;
	}
	
	delete[] buffer;
}

void ResolveFunctionLinks()
{
    int i,j,k;
    for (i=0; i<apDefinedFunctions.size(); i++)
    {
        for (j=0; j<apDefinedFunctions.size(); j++)
        {
            if (i == j)
                continue;

            for (k=0; k<apDefinedFunctions[j].asCalledFunctions.size(); k++)
            {
                // Is the function defined in apDefinedFunctions[i] called by apDefinedFunctions[j] ?
                if (apDefinedFunctions[j].asCalledFunctions[k] == apDefinedFunctions[i].sFunctionName)
                {
                    // remove string from asCalledFunctions and add Function to apCalledFunctions.
                    apDefinedFunctions[j].apCalledFunctions.push_back(&(apDefinedFunctions[i]));
                    apDefinedFunctions[j].asCalledFunctions[k] = apDefinedFunctions[j].asCalledFunctions.back();
                    apDefinedFunctions[j].asCalledFunctions.pop_back();
                }
            }
        }
    }
}

void TreatAIdll(const char *_sSrcPath,const char *_sDestPath, const char *_sFileName)
{
	int i,j,k,l,m,code;

	// Functions used in the AIdll file.
	std::vector<Function *> apFunctionsUsed;

	// Functions whose dependencies must be treated.
	std::vector<Function *> apFunctionsToTreat;

	// The files that should be included.
	std::vector<std::string> asFilesToInclude;

	// Read source file
	char sFullName[260];
	CatPathFile(_sSrcPath,_sFileName,sFullName);
	char *srcBuffer = ReadBufferFromFile(sFullName);

	// Open dest file
	CatPathFile(_sDestPath,_sFileName,sFullName);
	code = _chmod(sFullName,_S_IREAD | _S_IWRITE ); 	// We don't want to be bugged by read only
	FILE *pFile = fopen(sFullName,"w");

	/////////////////////////////////////////////////////////////////////////
	// PARSE THE AIdll FILE
	/////////////////////////////////////////////////////////////////////////

	// Parse the #includes in order to know which models are included
	// Put file in buffer (easier manipulation).
	char *currentBuffer;

	// Search the entry point to write in AI2C_dllxxx_Open (just before '}')
	char *entryPointAI2C_dllxxx_Open = strstr(srcBuffer,"Open(void)\n{"); 
	if (!entryPointAI2C_dllxxx_Open)
	{
		// Copy src file in dest file and exit.
		fwrite(srcBuffer,strlen(srcBuffer),1,pFile);
		goto end;
	}
	entryPointAI2C_dllxxx_Open = strchr(entryPointAI2C_dllxxx_Open,'}')-1;

	// Search the entry point to write in AI2C_dllxxx_Open (just before '}')
	char *entryPointAI2C_dllxxx_Close = strstr(srcBuffer,"Close(void)\n{"); 
	entryPointAI2C_dllxxx_Close = strchr(entryPointAI2C_dllxxx_Close,'}')-1;

	// Search the begining of #includes
	char *entryPointStartInclude = strstr(srcBuffer,"#include \"AI2C_ofc/"); 

	// Search the end of #includes
    char *entryPointStopInclude = strstr(srcBuffer,"#endif /* GAMECUBE_USE_AI2C_DLL */"); 

    // Search functions
	currentBuffer = strstr(srcBuffer,"Close(void)\n{"); 
	currentBuffer = strstr(currentBuffer,"AI2C_pfi_");
	while (currentBuffer)
    {
		char sFuncName[260];

		// Find function name
		currentBuffer += strlen("AI2C_pfi_");

        for (i = 0; currentBuffer[i] != ' '; i++)
		{
			sFuncName[i] = currentBuffer[i];
		}

        sFuncName[i] = '\0';

        // Search for Function
        for (i=0; i<apDefinedFunctions.size(); i++)
        {
            if (apDefinedFunctions[i].sFunctionName == sFuncName)
            {
                apFunctionsUsed.push_back(&apDefinedFunctions[i]);
                apFunctionsToTreat.push_back(&apDefinedFunctions[i]);
                break;
            }
        }

	    currentBuffer = strstr(currentBuffer,"AI2C_pfi_");
    }

    // Compute function dependencies
    while (apFunctionsToTreat.size() != 0)
    {
        Function *pTreatedFunc = apFunctionsToTreat.back();
        apFunctionsToTreat.pop_back();
        for (i=0; i<pTreatedFunc->apCalledFunctions.size(); i++)
        {
            Function *pCalledFunc = pTreatedFunc->apCalledFunctions[i];
            bool bAddFuncToFuncToTreat = true;
            for (j=0; j<apFunctionsUsed.size(); j++)
            {
                if (apFunctionsUsed[j]->sFunctionName == pCalledFunc->sFunctionName)
                {
                    bAddFuncToFuncToTreat = false;
                    break;
                }
            }
            if (bAddFuncToFuncToTreat)
            {
                apFunctionsToTreat.push_back(pCalledFunc);
                apFunctionsUsed.push_back(pCalledFunc);
            }
        }
    }

    // Compute files to include
    for (i=0; i<apFunctionsUsed.size(); i++)
    {
        bool bAddFileToInclude = true;
        for (j=0; j<asFilesToInclude.size(); j++)
        {
            if (apFunctionsUsed[i]->sFileName == asFilesToInclude[j])
                bAddFileToInclude =false;
        }

        if (bAddFileToInclude)
            asFilesToInclude.push_back(apFunctionsUsed[i]->sFileName);
    }



	// Search #include
	/*currentBuffer = strstr(srcBuffer,"#include \"AI2C_ofc/");
	while (currentBuffer)
	{
		// Find file name
		currentBuffer += strlen("#include \"AI2C_ofc/");
		char sFileName[260];
		i=0;
		while (currentBuffer[i] != '\"')
		{
			sFileName[i] = currentBuffer[i];
			i++;
		}
		sFileName[i] = '\0';

        if (strstr(sFileName,".cpp"))
        {
            // Find model with file name.
            ModelFunctions *pModel = NULL;
            for (i=0; i<_rModels.size(); i++)
            {
                ModelFunctions *pTempModel = _rModels[i];

                if (!strcmp(pTempModel->sModelFileName,sFileName))
                {
                    pModel = pTempModel;
                    break;
                }
            }

            if (!pModel)
            {
                printf("Error : could not find model %s used by %s\n",sFileName,_sFileName);
                exit(-2);
            }

            aModelsIncluded.push_back(pModel);

            for (i=0; i<pModel->asDefinedFunctions.size(); i++)
                AddIfNotPresent(asAIdllDefinedFunctions,pModel->asDefinedFunctions[i].c_str());
        }
	
		// Next #include
		currentBuffer = strstr(currentBuffer,"#include \"AI2C_ofc/");
	}*/

	/////////////////////////////////////////////////////////////////////////
	// COMPUTE THE FUNCTIONS AND FILES THAT MUST BE INCLUDED
	/////////////////////////////////////////////////////////////////////////

	// For each of the functions that are used but not defined, 
	// include the file that defines the function.
	/*for (i=0; i<aModelsIncluded.size(); i++)
	{
		ModelFunctions *pModelIncluded = aModelsIncluded[i];
		for (j=0; j<pModelIncluded->asUsedFunctions.size(); j++)
		{
			// Is the function defined in the files already included ?
			bool bIsFunctionDefined = false;
			for (k=0; k< asAIdllDefinedFunctions.size(); k++)
			{
				if (pModelIncluded->asUsedFunctions[j] == asAIdllDefinedFunctions[k])
				{
					// Function is defined.
					bIsFunctionDefined = true;
					break;
				}
			}

			if (!bIsFunctionDefined)
			{
				// We search in all the models one that defines the function.
				for (k=0; k<_rModels.size(); k++)
				{
					ModelFunctions *pModel = _rModels[k];
					for (l=0; l<pModel->asDefinedFunctions.size(); l++)
					{
						if (pModelIncluded->asUsedFunctions[j] == pModel->asDefinedFunctions[l])
						{
							aModelsIncluded.push_back(pModel);
							aModelsToInclude.push_back(pModel);
							for (m=0; m<pModel->asDefinedFunctions.size(); m++)
								AddIfNotPresent(asAIdllDefinedFunctions,pModel->asDefinedFunctions[m].c_str());

							AddIfNotPresent(asFunctionsToDefine,pModelIncluded->asUsedFunctions[j].c_str());

							bIsFunctionDefined = true;
							goto found;
						}
					}
				}
found:
				if (!bIsFunctionDefined)
				{
					fprintf(logFile,"Function %s used in %s and never defined\n",pModelIncluded->asUsedFunctions[j].c_str(),pModelIncluded->sModelFileName);
				}
			}
		}
	}*/


	/////////////////////////////////////////////////////////////////////////
	// WRITE THE DEST FILE
	/////////////////////////////////////////////////////////////////////////

	// We have parsed the source file. 
	// Now we create the dest file with the additionnal includes and function definitions.

	// Copy src file to dest file up to first entry point.
	fwrite(srcBuffer,entryPointAI2C_dllxxx_Open-srcBuffer,1,pFile);

	// Additional functions in AI2C_dllxxx_Open
	/*fprintf(pFile,"\n\n\t// Functions added by GenerateDll.exe\n");
	for (i=0; i<asFunctionsToDefine.size(); i++)
	{
		fprintf(pFile,"\tAI2C_pfi_%s = %s;\n",asFunctionsToDefine[i].c_str(),asFunctionsToDefine[i].c_str());
	}*/

	// Copy src file to dest file between open and close functions.
	fwrite(entryPointAI2C_dllxxx_Open,entryPointAI2C_dllxxx_Close-entryPointAI2C_dllxxx_Open,1,pFile);

	// Additional functions in AI2C_dllxxx_Close
	/*fprintf(pFile,"\n\n\t// Functions added by GenerateDll.exe\n");
	for (i=0; i<asFunctionsToDefine.size(); i++)
	{
		fprintf(pFile,"\tAI2C_pfi_%s = NULL;\n",asFunctionsToDefine[i].c_str());
	}*/

	// Copy src file to dest file between close function and includes.
	//fwrite(entryPointAI2C_dllxxx_Close,entryPointStopInclude-entryPointAI2C_dllxxx_Close,1,pFile);
	fwrite(entryPointAI2C_dllxxx_Close,entryPointStartInclude-entryPointAI2C_dllxxx_Close,1,pFile);

	// Additional includes.
	fprintf(pFile,"\n// Includes added by GenerateDll.exe\n");

    // headers
	for (i=0; i<asFilesToInclude.size(); i++)
	{
        char sName[256];
        sprintf(sName,"%s",asFilesToInclude[i].c_str());
        int iLen = strlen(sName);
        sName[iLen-3] = 'h';
        sName[iLen-2] = '\0';

		fprintf(pFile,"#include \"AI2C_ofc/%s\"\n",sName);		
	}

    // src
	for (i=0; i<asFilesToInclude.size(); i++)
	{
		fprintf(pFile,"#include \"AI2C_ofc/%s\"\n",asFilesToInclude[i].c_str());		
	}
	fprintf(pFile,"\n\n");

	// Copy src file to dest file until end of src file.
	fwrite(entryPointStopInclude,strlen(entryPointStopInclude),1,pFile);

end:
	fclose(pFile);
	delete[] srcBuffer;
}

/*
void WriteParseResult(std::vector<ModelFunctions *> &_Models)
{
	FILE *file = fopen("GenerateDllParseResult.log","w");
	int i,j;
	for (i=0; i<_Models.size(); i++)
	{
		ModelFunctions *pModel = _Models[i];

		fprintf(file,"##################################################\n");
		fprintf(file,"MODEL : %s\n",pModel->sModelFileName);
		fprintf(file,"##################################################\n");
		fprintf(file,"\n");
		fprintf(file,"FUNCTIONS DEFINED IN %s :\n",pModel->sModelFileName);
		for (j=0; j<pModel->asDefinedFunctions.size(); j++)
		{
			fprintf(file,"%s\n",pModel->asDefinedFunctions[j].c_str());	
		}
		fprintf(file,"\n");
		fprintf(file,"FUNCTIONS USED IN %s : \n",pModel->sModelFileName);
		for (j=0; j<pModel->asUsedFunctions.size(); j++)
		{
			fprintf(file,"%s\n",pModel->asUsedFunctions[j].c_str());	
		}
		fprintf(file,"\n");
	}

	fclose(file);
}

void AddModelToDependencies(
ModelFunctions *_pModel,
std::vector<ModelFunctions *> &_raDependencies,
const char *_sFunctionName,
std::vector<std::string> &_rasFunctionDependencies,
std::vector<std::string> &_rasDefinedFunctions,
std::vector<std::string> &_rasUndefinedFunctions
)
{
	int i;

	_raDependencies.push_back(_pModel);
	_rasFunctionDependencies.push_back(std::string(_sFunctionName));

	for (i=0; i<_pModel->asDefinedFunctions.size(); i++)
		_rasDefinedFunctions.push_back(_pModel->asDefinedFunctions[i]);

	for (i=0; i<_pModel->asUsedFunctions.size(); i++)	
		AddIfNotPresent(_rasDefinedFunctions,_rasUndefinedFunctions,_pModel->asUsedFunctions[i].c_str());
}


void AddModelToIsUsedBy(
ModelFunctions *_pModel,
std::vector<ModelFunctions *> &_raDependencies,
const char *_sFunctionName,
std::vector<std::string> &_rasFunctionDependencies,
std::vector<std::string> &_rasDefinedFunctions,
std::vector<std::string> &_rasUndefinedFunctions
)
{
	int i;

	_raDependencies.push_back(_pModel);
	_rasFunctionDependencies.push_back(std::string(_sFunctionName));

	for (i=0; i<_pModel->asDefinedFunctions.size(); i++)
		_rasDefinedFunctions.push_back(_pModel->asDefinedFunctions[i]);

	for (i=0; i<_pModel->asUsedFunctions.size(); i++)	
		AddIfNotPresent(_rasDefinedFunctions,_rasUndefinedFunctions,_pModel->asUsedFunctions[i].c_str());
}



ModelFunctions *pFindModelWhereDefined(std::string &_rsUndefinedFunction,std::vector<ModelFunctions *> &_Models)
{
	for (int i=0; i<_Models.size(); i++)
	{
		ModelFunctions *pModel = _Models[i];
		for (int j=0; j<pModel->asDefinedFunctions.size(); j++)
		{
			if (pModel->asDefinedFunctions[j] == _rsUndefinedFunction)
				return pModel;
		}
	}
	return NULL;
}

void WriteDependencies(std::vector<ModelFunctions *> &_Models,std::vector<std::vector<ModelFunctions *> > &_raaDependencies )
{
	_raaDependencies.resize(_Models.size());

	FILE *file = fopen("GenerateDllDependencies.log","w");
	int i,j;
	for (i=0; i<_Models.size(); i++)
	{
		ModelFunctions *pModel = _Models[i];

		std::vector<ModelFunctions *> &raDependencies = _raaDependencies[i];
		std::vector<std::string> asFunctionDependencies;
		std::vector<std::string> asUndefinedFunctions;
		std::vector<std::string> asDefinedFunctions;

		// Init vectors
		AddModelToDependencies(pModel,raDependencies,"none",asFunctionDependencies,asDefinedFunctions,asUndefinedFunctions);

		while (!asUndefinedFunctions.empty())
		{
			std::string sUndefinedFunction = asUndefinedFunctions.back();
			asUndefinedFunctions.pop_back();

			// Search for model that defines function, first in models already included
			if (!pFindModelWhereDefined(sUndefinedFunction,raDependencies))
			{
				ModelFunctions *pFoundModel	= pFindModelWhereDefined(sUndefinedFunction,_Models);
				if (pFoundModel)
					AddModelToDependencies(pFoundModel,raDependencies,sUndefinedFunction.c_str(),asFunctionDependencies,asDefinedFunctions,asUndefinedFunctions);
			}
		}


		fprintf(file,"##################################################\n");
		fprintf(file,"%s USES :\n",pModel->sModelFileName);

		// Print list of models dependencies.
		for (j=1; j<raDependencies.size(); j++)
		{
			fprintf(file,"%s (where %s is defined)\n",raDependencies[j]->sModelFileName,asFunctionDependencies[j].c_str());
		}

		fprintf(file,"##################################################\n");
		fprintf(file,"\n");

	}
	fclose(file);
}

void WriteIsUsedBy(std::vector<ModelFunctions *> &_Models,std::vector<std::vector<ModelFunctions *> > &_raaDependencies)
{
	FILE *file = fopen("GenerateDllIsUsedBy.log","w");
	int i,j,k;
	for (i=0; i<_Models.size(); i++)
	{
		ModelFunctions *pModel = _Models[i];
		std::vector<ModelFunctions *> aIsUsedBy;

		for (j=0; j<_raaDependencies.size(); j++)
		{
			ModelFunctions *pTempModel = _Models[j];
			std::vector<ModelFunctions *> &raTempDependencies = _raaDependencies[j];
			for (k=1; k<raTempDependencies.size(); k++)
			{
				if (raTempDependencies[k] == pModel)
					aIsUsedBy.push_back(pTempModel);
			}
		}

		fprintf(file,"##################################################\n");
		fprintf(file,"%s IS USED BY :\n",pModel->sModelFileName);

		// Print list of models dependencies.
		for (j=0; j<aIsUsedBy.size(); j++)
		{
			fprintf(file,"%s\n",aIsUsedBy[j]->sModelFileName);
		}

		fprintf(file,"##################################################\n");
		fprintf(file,"\n");
	}
	fclose(file);
}*/

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 4)
	{
		printf("usage : GenerateDll AIdll_src_path AIdll_dest_path function_path\n");
		exit(-1);
	}

	if (!strcmp(argv[1],argv[2]))
	{
		printf("Error : first and second arguments must be different.\n");
		exit(-10);
	}

	logFile = fopen("GenerateDll.log","w");

	struct _finddata_t fileinfo;
	intptr_t handle,code;
	char FileNames[260];

	// Open and parse the generated funcions files in order to get the functions defined and used.
	CatPathFile(argv[3],"*.cpp",FileNames);
	handle = _findfirst(FileNames,&fileinfo);
	code = handle;
	while (code != -1)
	{
		ParseModel(argv[3],fileinfo.name);
		code = _findnext(handle,&fileinfo);
	}
	code = _findclose(handle);

    ResolveFunctionLinks();

	// Write in log file the functions used and defined in the models .cpp files.
	//WriteParseResult(Models);

	// Write in log file the dependencies between .cpp files.
	//std::vector<std::vector<ModelFunctions *> > aaDependencies;
	//WriteDependencies(Models,aaDependencies);

	// Write in log file who need each .cpp file.
	//WriteIsUsedBy(Models,aaDependencies);

	// Open and parse the AIdllXXX.cpp files in order to get the functions used 
	// and to add the #includes needed for the link.
	CatPathFile(argv[1],"AIdll*.cpp",FileNames);
	handle = _findfirst(FileNames,&fileinfo);
	code = handle;
	while (code != -1)
	{
		TreatAIdll(argv[1],argv[2],fileinfo.name);
		code = _findnext(handle,&fileinfo);
	}
	code = _findclose(handle);

	fclose(logFile);

	// We don't delete everything, but who cares ?
	return 0;
}


// Previous ParseModel

	/*

	// We search for the key word "procedure". Then we search for '{' and ';'. 
	// If we encounter ';', the procedure is declared (and used). 
	// If we encounter '{', the procedure is defined. 
	char *procedure = strstr(currentBuffer,"procedure");
	while (procedure)
	{
		// Which is first ? ';' or '{' ?
		char *sFindSemiColon = strchr(procedure,';');
		char *sFindOpeningBracket = strchr(procedure,'{');
		bool bIsDefinition = (sFindSemiColon == NULL) || 
			((sFindOpeningBracket != NULL) && (sFindOpeningBracket < sFindSemiColon));

		// Get procedure name
		char sProcedureString[260];
		char sReturnValueString[260];
		char sProcedureNameString[260];
		sscanf(procedure,"%s %s %s",sProcedureString,sReturnValueString,sProcedureNameString);

		// Clean procedure name
		char *sFindOpeningParenthesis = strchr(sProcedureNameString,'(');
		if (sFindOpeningParenthesis != NULL)
			*sFindOpeningParenthesis = '\0';

		// Add procedure name either to definitions or declarations (if not already present)
		if (bIsDefinition)
			AddIfNotPresent(pModel->asDefinedFunctions,sProcedureNameString);
		else
			AddIfNotPresent(pModel->asUsedFunctions,sProcedureNameString);

		// Next procedure
		currentBuffer = procedure+1;
		procedure = strstr(currentBuffer,"procedure");
	}

	// This is not enough. We have to take into acount all strings before '(', that could be procedures used.
	char *parenthesis = strchr(buffer,'(');
	while (parenthesis)
	{
		// search before the '(' for a potentially valid procedure name.
		char *current = parenthesis-1;

		// Eat spaces
		while (current >= buffer && bIsSpace(*current))
			current--;

		// Build name.
		char *endOfName = current+1;
		while (current >= buffer && bIsValidChar(*current))
			current--;

		current ++;
		if (current < endOfName)
		{
			// Potential name
			char sName[260];
			int i = 0;
			while (current != endOfName)
			{
				sName[i] = *current;
				current++;
				i++;
			}
			sName[i] = '\0';

			if (strcmp(sName,"for") && 
				strcmp(sName,"if") &&
				strcmp(sName,"while") &&
				strcmp(sName,"switch") &&
				strcmp(sName,"return") &&
				strcmp(sName,"RETURN") &&
				strcmp(sName,"sizeof") &&
				strcmp(sName,"case") &&
				strcmp(sName,"const") &&
				strcmp(sName,"delete") &&
				strcmp(sName,"new") &&
				strcmp(sName,"catch"))
			{
				AddIfNotPresent(pModel->asUsedFunctions,sName);
			}
		}
		
		parenthesis = strchr(parenthesis+1,'(');
	}
	*/

