//#include <mpi.h>   
//#include <stdlib.h>   
//#include <stdio.h>     
//#include <string.h>
//
//  
//#pragma comment (lib, "mpi.lib")    
//
//int main(int argc, char* argv[])  
//{  
//    int myid,numprocs;  
//    int namelen;  
//    char processor_name[MPI_MAX_PROCESSOR_NAME];  
//	int len=0;
//	char * content;
//	FILE * file;
//	char * filePath;
//	int pathLen;
//	
//      
//    MPI_Init(&argc, &argv);  
//  
//    //用MPI_Comm_rank 获得进程的rank，该rank值为0到p-1间的整数,相当于进程的ID   
//    MPI_Comm_rank(MPI_COMM_WORLD, &myid);  
//    //用MPI_Comm_size 获得进程个数  int MPI_Comm_size(MPI_Comm comm, int *size);   
//    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);  
//  
//    MPI_Get_processor_name(processor_name, &namelen);  
//	filePath="e:/mpi/config.txt";
//	if(myid == 0){
//		pathLen = strlen(filePath);
//		file=fopen(filePath,"rb");
//		fseek(file,0,SEEK_END);
//		len = ftell(file);
//		content= new char[len+1];
//		fseek(file,0,SEEK_SET);
//		fread(content,1,len,file);
//		content[len] = 0;
//		fclose(file);
//		
//	}
//	MPI_Bcast(&len, 1,MPI_INT,0,MPI_COMM_WORLD);
//	if(myid!=0){
//		printf("len is %d from process %d \n",len,myid);
//		fflush(stdout);
//		content = new char[len+1];
//	}
//	MPI_Bcast(content,len,MPI_CHAR,0,MPI_COMM_WORLD);
//	if(myid!=0){
//		printf("filepath is %s from process %d \n",filePath,myid);
//		fflush(stdout);
//		file = fopen(filePath,"wb");
//		fwrite(content, len, 1, file);
//		//printf("content is %s from process %d \n",content,myid);
//		//fflush(stdout);
//		fclose(file);
//	}
//	delete content;
//    MPI_Finalize();  
//	//system("pause");
//    return 0; 
//}  
