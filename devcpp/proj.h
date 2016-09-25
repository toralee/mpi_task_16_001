#ifndef _HEADER_PROJ_H
#define _HEADER_PROJ_H
#include <string>
#include <vector>
#include <stdio.h>

using namespace std;
//================================================================================//
//#define TEST_CALLPLUGIN
//#define TEST_TransDir
//#define TEST_readConfig
//#define TEST_MPI
#define TEST_ASYN_MSG

#define RUN_USE_LOCAL_PARALLEL
#define RUN_USE_MAINMODE_PARALLEL

#define __DEBUG__MPI__

#ifdef __DEBUG__MPI__
#define DEBUG_MPI(format,...) printf("Debug,File: "__FILE__", Line: %05d: "format"\n", __LINE__, ##__VA_ARGS__)
#else
#define DEBUG_MPI(format,...)
#endif 


#define INFO_MPI(format,...) printf("Info,  File: "__FILE__", Line: %05d: "format"\n", __LINE__, ##__VA_ARGS__)

//================================================================================//



#define CONFIG_PATH ("E:\\mpi\\config.txt")

#define	INPUT_PATH_LINE	(0)
#define OUTPUT_PATH_LINE	(1)
#define COMMAND_LINE	(2)
#define COMMAND_PARAMETERS_LINE	(3)
#define NUM_NODE_LINE	(4)

#define NUM_LINE_CONFIG	(5)

#define NUM_NODE	(1)

#define FILE_SUFFIX "*.*"

/*
***********************************************
*DATA STRUCTURE                               *
***********************************************
*/ 
typedef struct _CONFIG_STRUCTURE{
	string szInputPath;
	string szOutputPath;
	string szCmd;
	string szCmdParams;
	
}CONFIG_STRUCTURE;

static int callPluginExe(const char* cmd, const char* params);
int callPluginByMPI(string input, string output, string cmd, string params);
int taskWorker(int id);
int callPluginWorker(int id, const char* filename);
//读出每行参数 
static int readConfig(string* szParams, const char* path);
int TransDir(vector<string>* vec, const char* dir);
int getConfig(CONFIG_STRUCTURE* cfg);
#endif
