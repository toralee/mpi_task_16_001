#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include <io.h>
#include <string>
#include <vector>

#include <fstream>

#include "proj.h"

using namespace std;


static int callPluginExe(const char* cmd, const char* params)
{
	int nRet = -1;
	BOOL bRet = FALSE;
	DWORD dw;
	SHELLEXECUTEINFO shExecInfo = {0};
	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	
	shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shExecInfo.hwnd = NULL;
	shExecInfo.lpVerb = NULL;
	shExecInfo.lpFile = _T(cmd);
	shExecInfo.lpParameters = _T(params);
	shExecInfo.lpDirectory = NULL;
	shExecInfo.nShow = SW_SHOW;
	shExecInfo.hInstApp = NULL;
	
	bRet = ShellExecuteEx(&shExecInfo);
	
	printf("__DEBUG__:cmd %s\n", cmd);
	printf("__DEBUG__:lpParameters %s\n", params);
	
	if(!bRet)
	{
		dw = GetLastError();
		return nRet;
	}
	//TerminateProcess(shExecInfo.hProcess, NULL);
	//阻塞等待处理完，发送消息
	dw = WaitForSingleObject(shExecInfo.hProcess, INFINITE);
	switch(dw)
	{
		case WAIT_OBJECT_0:
			printf("complete the call of callPluginExe\n");
			nRet = 0;
			break;
		case WAIT_TIMEOUT:
			break;
		case WAIT_FAILED:
			printf("Fail to call\n");
			break;
	}
	return nRet;
}

int callPluginByMPI(string input, string output, string cmd, string params)
{
	int nRet = -1;
	string szParamsCmd;
	szParamsCmd.clear();
	
	szParamsCmd = input + " " + output + " " + params;
	
	printf("call exe %s %s\n", cmd.c_str(), szParamsCmd.c_str());
	nRet = callPluginExe(cmd.c_str(), szParamsCmd.c_str());

	szParamsCmd.clear();
	return nRet;
}
/*
*遍历参数目录下文件 
*/
int TransDir(vector<string>* vec, const char* dir)
{
	int nRet = -1;
	long handle;
	//char szDir[256];
	_finddata_t fileinfo;

	if(dir == NULL)
	{
		return nRet;
	}
	if(vec == NULL)
	{
		return nRet;
	}
	//memset(szDir, 0x00, sizeof(szDir));
	//strncpy(szDir, dir, 256);
	//if(szDir[strlen(szDir) - 1] == '/')
	//{
	//	szDir[strlen(szDir) - 1] = '\0';
	//}
	//handle = _findfirst(szDir, &fileinfo); 
	handle = _findfirst(dir, &fileinfo); 
	if(-1 == handle)
	{
		return nRet;
	}
	
	if(!(fileinfo.attrib & _A_SUBDIR) )
	{
		vec->push_back(fileinfo.name);
		DEBUG_MPI("%s \n", fileinfo.name);
	}
	while(!_findnext(handle, &fileinfo))
	{
		if(!(fileinfo.attrib & _A_SUBDIR) )
		{
			vec->push_back(fileinfo.name);
			DEBUG_MPI("%s \n", fileinfo.name);
		}
	}
	nRet = 0;
	
	_findclose(handle);
	return nRet;
}

static int readConfig(string* szParams, const char* path)
{
	ifstream in(path);
	string line;
	int nRet = -1;
	int i = 0;
	if(in)
	{
		while(getline(in, line))
		{
			szParams[i++] = line;
		}
		nRet = 0;
		in.close();
	}
	if(i < NUM_LINE_CONFIG)
	{
		nRet = -1;
	}
	return nRet;
}

int getConfig(CONFIG_STRUCTURE* cfg)
{
	int nRet = -1;
	
	if(cfg == NULL)
	{
		return nRet;
	}
	
	string szconfig[NUM_LINE_CONFIG];

	//读配置 
	readConfig(szconfig, CONFIG_PATH);
	for(int i = 0; i<NUM_LINE_CONFIG; i ++)
	{
		DEBUG_MPI("%s\n", szconfig[i].c_str());
	}
	string szInputPath = szconfig[INPUT_PATH_LINE];
	string szOutputPath = szconfig[OUTPUT_PATH_LINE];
	string szCmd = szconfig[COMMAND_LINE];
	string szCmdParams = szconfig[COMMAND_PARAMETERS_LINE];
	if(szInputPath.length() > 0)
	{
		if(szInputPath[szInputPath.length() - 1] != '/')
		{
			szInputPath += "/";
		}
	}
	else
	{
		
		return nRet;
	}
	if(szOutputPath.length() > 0)
	{
		if(szOutputPath[szOutputPath.length() -1] !='/')
		{
			szOutputPath += "/";
		}
	}
	else
	{
		return nRet;
	}
	cfg->szInputPath = szInputPath;
	cfg->szOutputPath = szOutputPath;
	cfg->szCmd = szCmd;
	cfg->szCmdParams = szCmdParams;
	nRet = 0;
	return nRet;
}

int taskWorker(int id)
{
	int nRet = -1;
#if 0
	string szconfig[NUM_LINE_CONFIG];
#endif
	vector<string> vec;
	vec.clear();
	
	CONFIG_STRUCTURE cfg;
	memset(&cfg, 0x00, sizeof(cfg));
	
	//读配置 
	printf("Read the Config:\n");
	getConfig(&cfg);
#if 0
	readConfig(szconfig, CONFIG_PATH);
	for(int i = 0; i<NUM_LINE_CONFIG; i ++)
	{
		printf("%s\n", szconfig[i].c_str());
	}
	string szInputPath = szconfig[INPUT_PATH_LINE];
	string szOutputPath = szconfig[OUTPUT_PATH_LINE];
	string szCmd = szconfig[COMMAND_LINE];
	string szCmdParams = szconfig[COMMAND_PARAMETERS_LINE];
	
	if(szInputPath.length() > 0)
	{
		if(szInputPath[szInputPath.length() - 1] != '/')
		{
			szInputPath += "/";
		}
	}
	else
	{
		
		return -1;
	}
	if(szOutputPath.length() > 0)
	{
		if(szOutputPath[szOutputPath.length() -1] !='/')
		{
			szOutputPath += "/";
		}
	}
	else
	{
		return -1;
	}
#endif
	//遍历输入目录 
	string szInputPath = cfg.szInputPath;
	string szOutputPath = cfg.szOutputPath;
	string szCmd = cfg.szCmd;
	string szCmdParams = cfg.szCmdParams;
	string szTransPath;
	szTransPath.clear();
	szTransPath = szInputPath + FILE_SUFFIX;
	printf("TransDir: \n");
	TransDir(&vec, szTransPath.c_str());
	szTransPath.clear();
	
	//每个进程读一次配置文件、遍历输入目录。每个节点读一次进程
	//让每个节点有一个管理进程，不太现实。输入目录变量不好共享到其他进程。
	// 
	printf("Iterator the path:\n");
	string szParamsInput;
	string szParamsOutput;
	string szParamsOth;
	string szParamsCmd;
	szParamsInput.clear();
	szParamsOutput.clear();
	szParamsOth.clear();
	szParamsCmd.clear();
#ifdef RUN_USE_LOCAL_PARALLEL
	int nFileTransIndex = 0;
#endif
	for(vector<string>::iterator iter=vec.begin(); iter != vec.end(); iter++)
	{
#ifdef RUN_USE_LOCAL_PARALLEL
		if(nFileTransIndex % NUM_NODE == id)	//T.B.D. NUM_NODE最好能从配置文件里读 
		{
			printf("%s\n", (*iter).c_str());
		
			szParamsInput = szInputPath + (*iter);
			szParamsOutput = szOutputPath + (*iter);
			
			szParamsCmd = szParamsInput + " " + szParamsOutput + " " + szCmdParams;
			
			printf("call exe %s %s\n", szCmd.c_str(), szParamsCmd.c_str());
			callPluginExe(szCmd.c_str(), szParamsCmd.c_str());
			
			szParamsInput.clear();
			szParamsOutput.clear();
			szParamsOth.clear();
			szParamsCmd.clear();	
		}
#else
		printf("%s\n", (*iter).c_str());
		
		szParamsInput = szInputPath + (*iter);
		szParamsOutput = szOutputPath + (*iter);
		
		szParamsCmd = szParamsInput + " " + szParamsOutput + " " + szCmdParams;
		
		printf("call exe %s %s\n", szCmd.c_str(), szParamsCmd.c_str());
		callPluginExe(szCmd.c_str(), szParamsCmd.c_str());
		
		szParamsInput.clear();
		szParamsOutput.clear();
		szParamsOth.clear();
		szParamsCmd.clear();
#endif
		nFileTransIndex++;
	}
	nRet = 0;
	return nRet;
}

int callPluginWorker(int id, const char* filename)
{
	int nRet = -1;
	CONFIG_STRUCTURE cfg;
	memset(&cfg, 0x00, sizeof(cfg));
	
	//读配置 
	printf("Read the Config:\n");
	getConfig(&cfg);
#if 0
	string szconfig[NUM_LINE_CONFIG];
	readConfig(szconfig, CONFIG_PATH);

	printf("Read the Config:\n");
	for(int i = 0; i<NUM_LINE_CONFIG; i ++)
	{
		printf("%s\n", szconfig[i].c_str());
	}
	
	string szInputPath = szconfig[INPUT_PATH_LINE];
	string szOutputPath = szconfig[OUTPUT_PATH_LINE];
	string szCmd = szconfig[COMMAND_LINE];
	string szCmdParams = szconfig[COMMAND_PARAMETERS_LINE];
	string szTransPath;
	szTransPath.clear();
	if(szInputPath.length() > 0)
	{
		if(szInputPath[szInputPath.length() - 1] != '/')
		{
			szInputPath += "/";
		}
	}
	else
	{
		
		return -1;
	}
	
	if(szOutputPath.length() > 0)
	{
		if(szOutputPath[szOutputPath.length() -1] !='/')
		{
			szOutputPath += "/";
		}
	}
	else
	{
		return -1;
	}
#endif
	string szInputPath = cfg.szInputPath;
	string szOutputPath = cfg.szOutputPath;
	string szCmd = cfg.szCmd;
	string szCmdParams = cfg.szCmdParams;

	string szParamsInput;
	string szParamsOutput;
	string szParamsOth;
	string szParamsCmd;
	szParamsInput.clear();
	szParamsOutput.clear();
	szParamsOth.clear();
	szParamsCmd.clear();
	
	
	szParamsInput = szInputPath + filename;
	szParamsOutput = szOutputPath + filename;
	
	szParamsCmd = szParamsInput + " " + szParamsOutput + " " + szCmdParams;
	
	printf("call exe %s %s\n", szCmd.c_str(), szParamsCmd.c_str());
	callPluginExe(szCmd.c_str(), szParamsCmd.c_str());
	
	szParamsInput.clear();
	szParamsOutput.clear();
	szParamsOth.clear();
	szParamsCmd.clear();
	
	return nRet;
}

#if defined(TEST_readConfig)
int main(int argc, char** argv)
{
	string szconfig[NUM_LINE_CONFIG];
	//const string FILE_SUFFIX = "*.*";
	readConfig(szconfig, CONFIG_PATH);
	
	printf("Read the Config:\n");
	
	for(int i = 0; i<NUM_CONFIG_LINE; i ++)
	{
		printf("%s\n", szconfig[i].c_str());
	}
	
	vector<string> vec;
	printf("TransDir: \n");
	
	string szInputPath = szconfig[INPUT_PATH_LINE];
	string szOutputPath = szconfig[OUTPUT_PATH_LINE];
	string szCmd = szconfig[COMMAND_LINE];
	string szCmdParams = szconfig[COMMAND_PARAMETERS_LINE];
	string szTransPath;
	szTransPath.clear();
	if(szInputPath.length() > 0)
	{
		if(szInputPath[szInputPath.length() - 1] == '/')
		{
			//szInputPath += "*.*";
			szTransPath = szInputPath + FILE_SUFFIX;
		}
		else
		{
			
			//szInputPath += "/*.*";
			szInputPath += "/";
			szTransPath = szInputPath + FILE_SUFFIX;
		}
	}
	else
	{
		
		return -1;
	}
	
	if(szOutputPath.length() > 0)
	{
		if(szOutputPath[szOutputPath.length() -1] =='/')
		{
			//
		}
		else
		{
			szOutputPath += "/";
		}
	}
	else
	{
		return -1;
	}	
		
	TransDir(&vec, szTransPath.c_str());
	szTransPath.clear();
	
	printf("Iterator the path:\n");
	string szParamsInput;
	string szParamsOutput;
	string szParamsOth;
	string szParamsCmd;
	szParamsInput.clear();
	szParamsOutput.clear();
	szParamsOth.clear();
	szParamsCmd.clear();
	for(vector<string>::iterator iter=vec.begin(); iter != vec.end(); iter++)
	{
		printf("%s\n", (*iter).c_str());
		
		szParamsInput = szInputPath + (*iter);
		szParamsOutput = szOutputPath + (*iter);
		
		szParamsCmd = szParamsInput + " " + szParamsOutput + " " + szCmdParams;
		
		printf("call exe %s %s\n", szCmd.c_str(), szParamsCmd.c_str());
		callPluginExe(szCmd.c_str(), szParamsCmd.c_str());
		
		szParamsInput.clear();
		szParamsOutput.clear();
		szParamsOth.clear();
		szParamsCmd.clear();
	}
	//callPluginExe(szCmd.c_str(), szCmdParams.c_str());
	return 0;
}
#endif

#if defined(TEST_TransDir)
int main(int argc, char** argv)
{
	vector<string> vec;
	printf("TransDir \n");
	TransDir(&vec);
	
	printf("iterator the path:\n");
	for(vector<string>::iterator iter=vec.begin(); iter != vec.end(); iter++)
	{
		printf("%s\n", (*iter).c_str());
	}
	return 0;
}
#endif

#if defined(TEST_CALLPLUGIN)
int main(int argc, char** argv)
{
	printf("callPluginExe \n");
	callPluginExe();
	return 0;
}
#endif
