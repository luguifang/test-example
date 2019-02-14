/*************************************************************************
	> File Name: testclamav.c
	> Author: luguifang
	> Mail: lugf@chinabluedon.com.cn 
	> Created Time: Wed 13 Jun 2018 05:37:40 PM DST
 ************************************************************************/

#include<stdio.h>
#include<clamav.h>
#include<sys/time.h>
#define SIG_DB "/home/lgf/db/"

int main(){
	int ret;
	if(CL_SUCCESS != cl_init(CL_INIT_DEFAULT)){
		printf("----------------------exit process init failed!!!-----\n");
		exit(0);
	}
	struct cl_engine *pengine =NULL; pengine =  cl_engine_new();
	if(!pengine){
		printf("-----------------alloc mem failed to engine----------\n");
		exit(0);
	}
	//-----------load engine------
	unsigned int sig = 0;
	if(CL_SUCCESS != cl_load(SIG_DB,pengine,&sig,CL_DB_STDOPT)){
		printf("--------------------cl_load() failed---------\n");
		cl_engine_free(pengine);
		exit(0);
	}
	if(CL_SUCCESS!=(ret = cl_engine_compile(pengine))){
		printf("-------------------engine compile failed----\n");
		cl_engine_free(pengine);
		exit(0);
	}
	//-----scan file------
	struct timeval tval;
	gettimeofday(&tval,NULL);
	printf("----------------start time:%ld------\n",tval.tv_sec);
	const char *virname =NULL;
	if(CL_VIRUS == (ret = cl_scanfile("/tmp/123",&virname,NULL,pengine,CL_SCAN_STDOPT))){
		printf("-----------find virus:%s-----\n",virname);
	}else{
		printf("==================normal file====\n");
		if(ret!=CL_CLEAN){
			printf("--------------error----\n");
			exit(0);
		}
	}
	gettimeofday(&tval,NULL);
	printf("---------------end time:%ld----\n",tval.tv_sec);


	return 0;

}

