#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <malloc.h>
#include "log.h"
#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "credui.lib")


typedef enum {
	setRunKey,
	delRunKey,
}actions;

actions processArguments(char* arg);
FILE* setupLogging();
void setupRunKeyPersistance(char* exePath, char* keyName);
void deleteRunKeyPersistance(char *keyName);


void main(int argc, char* argv[])
{
	if (argc == 2) {
		printf("The argument supplied is %s\n", argv[1]);
	}
	else if (argc > 2) {
		printf("Too many arguments supplied.\n");
		exit(0);
	}
	else {
		printf("One argument expected.\n");
		exit(0);
	}

	actions opt = processArguments(argv[1]);

	FILE* logFptr = setupLogging();

	log_trace("Current Function: %s\n", __func__);

	log_trace("Starting Persistance. \n");

	log_trace("Setting up registry persistance.\n");

	char szFile[MAX_PATH] = { 0 };

	DWORD p_id = GetCurrentProcessId();
	GetModuleFileName(NULL, szFile, MAX_PATH);
	
	log_trace("Process ID: %d.\n", p_id);
	log_trace("GetModuleFileName: %s\n", szFile);

	switch (opt)
	{
	case setRunKey:
		setupRunKeyPersistance(szFile, "askRunPersistance");
		break;
	case delRunKey:
		deleteRunKeyPersistance("askRunPersistance");
		break;
	default:
		break;
	}

	fclose(logFptr);
	
	char c = getchar();
}


actions processArguments(char* arg)
{
	log_trace("Current Function: %s\n", __func__);

	actions retValue;

	if (strcmp(arg, "SetRunKeyPersistance") == 0)
		retValue = setRunKey;
	else if (strcmp(arg, "DeleteRunKeyPersistance") == 0)
		retValue = delRunKey;
	else
		log_trace("No Match Found");

	return retValue;
}



FILE* setupLogging()
{
	char logPath[MAX_PATH];

	GetTempPath(sizeof(logPath), logPath);

	char* logFileName = "PersistanceLog.log";

	if (logPath[lstrlen(logPath) - 1] != '\\') lstrcat(logPath, "\\");

	lstrcat(logPath, logFileName);

	FILE* logFptr;

	logFptr = fopen(logPath, "a+");

	log_add_fp(logFptr, LOG_TRACE);

	return logFptr;
}


void setupRunKeyPersistance(char *exePath, char *keyName)
{
	log_trace("Current Function: %s\n", __func__);
	
	HKEY hKey;
	char *regPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,regPath,0, KEY_WRITE,&hKey) != 0)
		if (RegOpenKeyEx(HKEY_CURRENT_USER, regPath, 0, KEY_WRITE, &hKey) != 0)
		{
			log_trace("Could not open registry run keys for editing.");
			return;
		}

	RegSetValueEx(hKey, keyName, 0, REG_SZ, exePath, strlen(exePath));
	RegCloseKey(hKey);

}

void deleteRunKeyPersistance(char *keyName)
{
	log_trace("Current Function: %s\n", __func__);

	HKEY hKey;
	char* regPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, regPath, 0, KEY_WRITE, &hKey) != 0)
		if (RegOpenKeyEx(HKEY_CURRENT_USER, regPath, 0, KEY_WRITE, &hKey) != 0)
		{
			log_trace("Could not open registry run keys for editing.");
			return;
		}

	RegDeleteValue(hKey, keyName);
	RegCloseKey(hKey);
}


