#include "proj.h"

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

using namespace std;

#if defined(TEST_ASYN_MSG)

int main(int argc, char** argv)
{
	int nRet = -1;
	//������Ŀ
	int nProcessNum = -1;
	int nSlaveNum = -1;
	
	//����������Ŀ
	int nDataTaskNum = -1;
	int nCurSlaveTaskNum = -1;
	vector<MPI_Request> vec_request;
	MPI_Request* pReq = NULL;
	string szCurSlaveTaskFilename;
	szCurSlaveTaskFilename.clear();
	vec_request.clear();
	
	//��ǰ����id
	int nProcess = -1; 
	
	//��ʼ��MPI����
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &nProcess);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcessNum);
	nSlaveNum = nProcessNum - 1;
	DEBUG_MPI("Init %d of %d\n", nProcess, nProcessNum);
	//�������� 
	CONFIG_STRUCTURE cfg;

	DEBUG_MPI("Read the Config:\n");
	//fprintf(stdout, "Read the Config:\n");
	nRet = getConfig(&cfg);
	if(nRet == -1)
	{
		fprintf(stderr, "config read err.\n");
		return nRet;
	}
	
	//
	vector<string> vec_sz;
	vec_sz.clear();
	
	//nTag[0]=0 �����ӽڵ��������� 
	//nTag[1]=1 �����ӽڵ������� 
	//nTag[2]=2 �ӽڵ�������ɱ�ʶ 
	//nTag[3]=100 �жϻ�����ʼ���Ƿ�ɹ� 
	int nTag[4] = {0, 1, 2, 100};
	//���ڵ���� 
	DEBUG_MPI("Init complete\n");
	if(nProcess == 0)
	{
		DEBUG_MPI("Main node process id:%d\n", nProcess);
		//��������slave 
		nRet = TransDir(&vec_sz, (cfg.szInputPath+FILE_SUFFIX).c_str());
		int nBufRet[1]; 
		nBufRet[0] = nRet;
		INFO_MPI("MPI Envi Init result: %d", nRet);
		for(int i = 1; i <= nSlaveNum; i++)
		{
			MPI_Send(nBufRet, 1, MPI_INT, i, nTag[3], MPI_COMM_WORLD);
		}
		if(nRet == -1)
		{
			//T.B.D. ���ʱ���˳�����Ҫ֪ͨ���ɹ���ʧ�ܶ�Ҫ֪ͨ��
			DEBUG_MPI("Tag %d: MPI send: %d", nTag[3], nBufRet[0]);
			return nRet; 
		}
		DEBUG_MPI("Tag %d: MPI send: %d", nTag[3], nBufRet[0]);
		
		nDataTaskNum = vec_sz.size();
		//������������-�������� 
		int nMod = -1;
		int nDivide = -1;
		int nBuf[1];
		nDivide = nDataTaskNum / nSlaveNum;
		nMod = nDataTaskNum % nSlaveNum;
		nBuf[0] = nDataTaskNum / nSlaveNum;
		DEBUG_MPI("%d\n", nSlaveNum);
		DEBUG_MPI("%d\n", nMod);
		
		int i = -1;
		nBuf[0] = nDivide + 1;
		for(i = 1; i <= nMod; i++)
		{
			MPI_Send(nBuf, 1, MPI_INT, i, nTag[0], MPI_COMM_WORLD);
			fprintf(stdout, "Tag %d:%d -> %d Despatch %d Tasks\n", nTag[0], nProcess, i, nBuf[0]);
		}
		nBuf[0] = nDivide;
		for(i = nMod + 1; i <= nSlaveNum; i++)
		{
			MPI_Send(nBuf, 1, MPI_INT, i, nTag[0], MPI_COMM_WORLD);
			fprintf(stdout, "Tag %d:%d -> %d Despatch %d Tasks\n", nTag[0], nProcess, i, nBuf[0]);
		}
#if 0
		for(i = 1; i < nSlaveNum; i++)
		{
			MPI_Send(nBuf, 1, MPI_INT, i, nTag[0], MPI_COMM_WORLD);
			fprintf(stdout, "Tag %d:%d -> %d Despatch %d Tasks\n", nTag[0], nProcess, i, nBuf[0]);
		}
		if(nMod == 0)
		{
			MPI_Send(nBuf, 1, MPI_INT, i, nTag[0], MPI_COMM_WORLD);
			fprintf(stdout, "Tag %d:%d -> %d Despatch %d Tasks\n", nTag[0], nProcess, i, nBuf[0]);
		}
		else
		{
			nBuf[0] = nMod;
			MPI_Send(nBuf, 1, MPI_INT, i, nTag[0], MPI_COMM_WORLD);
			fprintf(stdout, "Tag %d:%d -> %d Despatch %d Tasks\n", nTag[0], nProcess, i, nBuf[0]);
		}
#endif		
		//���ʹ����������-���������� 
		int nModi = -1;
		char szMsg[1024] = {0};
		MPI_Request req;
		vec_request.clear();
		pReq = (MPI_Request*)malloc(nDataTaskNum * sizeof(MPI_Request));
		memset(pReq, 0x00, nDataTaskNum * sizeof(MPI_Request));
		for(i = 1; i <= nDataTaskNum; i++)
		{
			memset(szMsg, 0x00, sizeof(szMsg));
			strncpy(szMsg, vec_sz[i-1].c_str(), 1024);
			nModi = i % nSlaveNum;
			if(nModi == 0)
			{//T.B.D.
				//MPI_Send(szMsg, strlen(szMsg), MPI_CHAR, nSlaveNum, nTag[1], MPI_COMM_WORLD);
				//memset(&req, 0x00, sizeof(MPI_Request));
				//MPI_Isend(szMsg, strlen(szMsg), MPI_CHAR, nSlaveNum, nTag[1], MPI_COMM_WORLD, &req);
				//vec_request.push_back(req);
				MPI_Isend(szMsg, strlen(szMsg), MPI_CHAR, nSlaveNum, nTag[1], MPI_COMM_WORLD, (pReq + i -1));
				fprintf(stdout, "Tag %d:%d -> %d send taskinfo %s\n", nTag[1], nProcess, nSlaveNum, szMsg);
			}
			else
			{//T.B.D.
				//MPI_Send(szMsg, strlen(szMsg), MPI_CHAR, nModi, nTag[1], MPI_COMM_WORLD);
				//memset(&req, 0x00, sizeof(MPI_Request));
				//MPI_Isend(szMsg, strlen(szMsg), MPI_CHAR, nModi, nTag[1], MPI_COMM_WORLD, &req);
				//vec_request.push_back(req);
				MPI_Isend(szMsg, strlen(szMsg), MPI_CHAR, nModi, nTag[1], MPI_COMM_WORLD, (pReq + i -1));
				fprintf(stdout, "Tag %d:%d -> %d send taskinfo %s\n", nTag[1], nProcess, nModi, szMsg);
			}
		} 
		
		//�ȴ����н��̽���		 
		//wait
		
		//while(true)
		//{
			//T.B.D.
			//ʹ��MPI_Waitsomeִ�ж�Σ��ڲ����ݽṹ��θ��� 
			//MPI_Waitsome(nDataTaskNum, )
		//}
		
		
		int nAckIndex = 0;
		int nAckBuf[1] = {0};
		int nCorrectIndex = 0;	//������ȷ������� 
		int nIncorrectIndex = 0;	//������ȷ������� 
		while(true)
		{
			memset(nAckBuf, 0x00, sizeof(nAckBuf));
			memset(&status, 0x00, sizeof(MPI_Status));
			MPI_Recv(nAckBuf, 1, MPI_INT, MPI_ANY_SOURCE, nTag[2], MPI_COMM_WORLD, &status);
			fprintf(stdout, "Tag %d:%d <- %d Recv ACK %d\n", nTag[2], nProcess, status.MPI_SOURCE, nAckBuf[0]);
			nAckIndex++;
			if(nAckBuf[0] == 1)
			{
				nCorrectIndex++;
			}
			if(nAckBuf[0] == 0)
			{
				nIncorrectIndex++;
			}
			//nCorrectIndex+=nAckBuf[0];
			if(nAckIndex == nDataTaskNum)
			{
				break;
			}
		} 
		
	}
	//�ӽڵ���� 
	else
	{
		DEBUG_MPI("Slave node process id:%d\n", nProcess);
		//������������ 
		int nBuf[1];
		int nBufRet[1];
		MPI_Recv(nBufRet, 1, MPI_INT, 0, nTag[3], MPI_COMM_WORLD, &status);
		INFO_MPI("%d:%d <- %d Recv nBufRet %d\n", nTag[3], nProcess, 0, nBufRet[0]);
		if(nBufRet[0] != 0)
		{
			nRet = -1;
			DEBUG_MPI();
			return nRet;
		}
		MPI_Recv(nBuf, 1, MPI_INT, 0, nTag[0], MPI_COMM_WORLD, &status);
		fprintf(stdout, "%d:%d <- %d Recv Tasks %d\n", nTag[0], nProcess, 0, nBuf[0]);
		//�������� 
		nCurSlaveTaskNum = nBuf[0];
		char szMsg[1024] = {0};
		
		int nAckBuf[1] = {0};
		
		for(int i = 0; i < nCurSlaveTaskNum; i++)
		{
			memset(szMsg, 0x00, sizeof(szMsg));
			memset(&status, 0x00, sizeof(MPI_Status));
			MPI_Recv(szMsg, sizeof(szMsg), MPI_CHAR, 0, nTag[1], MPI_COMM_WORLD, &status);
			fprintf(stdout, "%d:%d <- %d Recv taskinfo %s\n", nTag[1], nProcess, 0, szMsg);
			szCurSlaveTaskFilename = string(szMsg);
			//���ò������
			DEBUG_MPI("callPluginByMPI(%s+%s, %s+%s, %s, %s)", 
					cfg.szInputPath.c_str(), 
					szCurSlaveTaskFilename.c_str(), 
					cfg.szOutputPath.c_str(), 
					szCurSlaveTaskFilename.c_str(), 
					cfg.szCmd.c_str(), 
					cfg.szCmdParams.c_str());
#if 1
			nRet = callPluginByMPI(cfg.szInputPath + szCurSlaveTaskFilename, cfg.szOutputPath + szCurSlaveTaskFilename, cfg.szCmd, cfg.szCmdParams);
#endif
			//ִ���귢�ͷ��� 
			if(nRet != -1)
			{
				//�����������Ack 
				nAckBuf[0] = 1;
				DEBUG_MPI("nAckBuf %d\n", nAckBuf[0]);
			}
			else
			{
				nAckBuf[0] = 0;
				DEBUG_MPI("nAckBuf %d\n", nAckBuf[0]);
			}
			MPI_Send(nAckBuf, 1, MPI_INT, 0, nTag[2], MPI_COMM_WORLD);
			DEBUG_MPI("MPI_Send nAckBuf:%d\n", nAckBuf[0]);
		}
				 
	}
	MPI_Finalize();
}

#endif
