#include "proj.h"

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

using namespace std;
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

#if defined(TEST_MPI)

int main(int argc, char** argv) {
	
	int myrank;
	char szMsg[20];
	MPI_Status status;
	memset(szMsg, 0x00, sizeof(szMsg));
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
	int nRet = -1;
	//读出配置 
	string szconfig[NUM_LINE_CONFIG];
	printf("Read the Config:\n");
	nRet = readConfig(szconfig, CONFIG_PATH); 
	if(-1 == nRet)
	{
		return nRet;
	}
	for(int i = 0; i<NUM_LINE_CONFIG; i ++)
	{
		printf("%s\n", szconfig[i].c_str());
	}
	
	string szInputPath = szconfig[INPUT_PATH_LINE];
	string szOutputPath = szconfig[OUTPUT_PATH_LINE];
	string szCmd = szconfig[COMMAND_LINE];
	string szCmdParams = szconfig[COMMAND_PARAMETERS_LINE];
	string szNumNode = szconfig[NUM_NODE_LINE];
	
	//遍历目录
	vector<string> vec;
	printf("TransDir: \n");
	
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
	
	TransDir(&vec, szTransPath.c_str());
	szTransPath.clear(); 
	
	//分发任务
	 if(myrank == 0)
	 {
	 	for(vector<string>::iterator iter = vec.begin(); iter != vec.end(); iter++)
	 	{
	 		
		}
	 }
	 else
	 {
	 	
	 }
	
#ifdef RUN_USE_MAINMODE_PARALLEL
	//read config
	
	//

#else 
	if(myrank == 0)
	{
		strcpy(szMsg, "hello");
		//MPI_Send(szMsg, strlen(szMsg), MPI_CHAR, 1, 99, MPI_COMM_WORLD);
		fprintf(stdout, "Process %d has sent %s \n", myrank, szMsg);
		taskWorker(myrank);
	}
	else /*if(myrank == 1)*/
	{
		//MPI_Recv(szMsg, 20, MPI_CHAR, 0, 99, MPI_COMM_WORLD, &status);
		fprintf(stdout, "Process %d has received %s \n", myrank, szMsg);
		taskWorker(myrank);
	}
#endif
	MPI_Finalize();
	return 0;
}
#endif
