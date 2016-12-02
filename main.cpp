#include <mpi.h>   
#include <stdlib.h>   
#include <stdio.h>   
#include <conio.h>   
#include <string.h>

  
#pragma comment (lib, "mpi.lib")    

int main(int argc, char* argv[])  
{  
    int myid,numprocs;  
    int namelen;  
    char processor_name[MPI_MAX_PROCESSOR_NAME];  
	int len=0;
	int i;
	char * content;
	FILE * file;
	char * filePath;
	int pathLen;
	
      
    MPI_Init(&argc, &argv);  
  
    //用MPI_Comm_rank 获得进程的rank，该rank值为0到p-1间的整数,相当于进程的ID   
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);  
    //用MPI_Comm_size 获得进程个数  int MPI_Comm_size(MPI_Comm comm, int *size);   
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);  
  
    MPI_Get_processor_name(processor_name, &namelen);  
	filePath="e:/mpi/config.txt";
	if(myid == 0){
		
		pathLen = strlen(filePath);
		file=fopen(filePath,"rb");
		fseek(file,0,SEEK_END);
		len = ftell(file);
		content= new char[len+1];
		fseek(file,0,SEEK_SET);
		fread(content,1,len,file);
		content[len] = 0;
		
		for(i = 1;i < numprocs;i++){
			//向子节点发送配置文件路径长度及名称
			MPI_Send(&pathLen,1,MPI_INT,i,98,MPI_COMM_WORLD);
			//printf("file path is %d from process 99 \n",pathLen);
			MPI_Send(filePath,pathLen,MPI_CHAR,i,98,MPI_COMM_WORLD);

			//向子节点发送配置文件的内容
			MPI_Send(&len,1,MPI_INT,i,99,MPI_COMM_WORLD);
			MPI_Send(content,len,MPI_CHAR,i,99,MPI_COMM_WORLD);
		}
		fclose(file);
		delete content;
	}
	else{
		MPI_Status status;
		//从主节点接收配置文件路径及名称
		MPI_Recv(&pathLen,1,MPI_INT,0,98,MPI_COMM_WORLD,&status);
		filePath = new char[pathLen+1];
		MPI_Recv(filePath,pathLen,MPI_CHAR,0,98,MPI_COMM_WORLD,&status);
		filePath[pathLen] = 0;
		printf("file path is %d from process %d \n",pathLen,myid);
		fflush(stdout);
		
		//从主节点接收配置文件的内容
		MPI_Recv(&len,1,MPI_INT,0,99,MPI_COMM_WORLD,&status);
		content = new char[len+1];
		MPI_Recv(content,len,MPI_CHAR,0,99,MPI_COMM_WORLD,&status);
		content[len]= 0;
		//将配置写入子节点本地文件
		file = fopen(filePath,"wb");
		fwrite(content, len, 1, file);
		printf("content is %s from process %d \n",content,myid);
		fflush(stdout);
		fclose(file);
		delete content;
	}
	
    MPI_Finalize();  
	//system("pause");
   

    return 0; 
}  
