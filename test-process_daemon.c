#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define ERR_EXIT(m) \
do\
{\
    perror(m);\
    exit(EXIT_FAILURE);\
}\
while (0);\

void creat_daemon(void);
int main(void)
{
    time_t t;
    int fd;
    //第一种通过自己建立:creat_daemon()实现;
	//第二种通过调用daemon函数实现
	if(daemon(0,0)==-1)
		ERR_EXIT("call daemon error");
    while(1){
        fd = open("daemon.log",O_WRONLY|O_CREAT|O_APPEND,0644);
        if(fd == -1)
            ERR_EXIT("open error");
        t = time(0);
        char *buf = asctime(localtime(&t));
        write(fd,buf,strlen(buf));
        close(fd);
        sleep(60);
            
    }
    return 0;
}
void creat_daemon(void)
{
    pid_t pid;
    pid = fork();
    if( pid == -1)
        ERR_EXIT("fork error");
    if(pid > 0 )
        exit(EXIT_SUCCESS);
    if(setsid() == -1)
        ERR_EXIT("SETSID ERROR");
    chdir("/");
    int i;
    for( i = 0; i < 3; ++i)
    {
        close(i);
        open("/dev/null", O_RDWR);
        dup(0);
        dup(0);
    }
    umask(0);
    return;
}

/*
二，创建守护进程步骤
首先我们要了解一些基本概念：

进程组 ：

每个进程也属于一个进程组
每个进程主都有一个进程组号，该号等于该进程组组长的PID号 .
一个进程只能为它自己或子进程设置进程组ID号
会话期：

会话期(session)是一个或多个进程组的集合。

setsid()函数可以建立一个对话期：

 如果，调用setsid的进程不是一个进程组的组长，此函数创建一个新的会话期。

(1)此进程变成该对话期的首进程

(2)此进程变成一个新进程组的组长进程。

(3)此进程没有控制终端，如果在调用setsid前，该进程有控制终端，那么与该终端的联系被解除。 如果该进程是一个进程组的组长，此函数返回错误。

(4)为了保证这一点，我们先调用fork()然后exit()，此时只有子进程在运行

现在我们来给出创建守护进程所需步骤：

编写守护进程的一般步骤步骤：

（1）在父进程中执行fork并exit推出；

（2）在子进程中调用setsid函数创建新的会话；

（3）在子进程中调用chdir函数，让根目录 ”/” 成为子进程的工作目录；

（4）在子进程中调用umask函数，设置进程的umask为0；

（5）在子进程中关闭任何不需要的文件描述符

说明：
  1. 在后台运行。 
为避免挂起控制终端将Daemon放入后台执行。方法是在进程中调用fork使父进程终止，让Daemon在子进程中后台执行。 
if(pid=fork()) 
exit(0);//是父进程，结束父进程，子进程继续 
2. 脱离控制终端，登录会话和进程组 
有必要先介绍一下Linux中的进程与控制终端，登录会话和进程组之间的关系：进程属于一个进程组，进程组号（GID）就是进程组长的进程号（PID）。登录会话可以包含多个进程组。这些进程组共享一个控制终端。这个控制终端通常是创建进程的登录终端。 
控制终端，登录会话和进程组通常是从父进程继承下来的。我们的目的就是要摆脱它们，使之不受它们的影响。方法是在第1点的基础上，调用setsid()使进程成为会话组长： 
setsid(); 
说明：当进程是会话组长时setsid()调用失败。但第一点已经保证进程不是会话组长。setsid()调用成功后，进程成为新的会话组长和新的进程组长，并与原来的登录会话和进程组脱离。由于会话过程对控制终端的独占性，进程同时与控制终端脱离。 
3. 禁止进程重新打开控制终端 
现在，进程已经成为无终端的会话组长。但它可以重新申请打开一个控制终端。可以通过使进程不再成为会话组长来禁止进程重新打开控制终端： 
if(pid=fork()) 
exit(0);//结束第一子进程，第二子进程继续（第二子进程不再是会话组长） 
4. 关闭打开的文件描述符 
进程从创建它的父进程那里继承了打开的文件描述符。如不关闭，将会浪费系统资源，造成进程所在的文件系统无法卸下以及引起无法预料的错误。按如下方法关闭它们： 
for(i=0;i 关闭打开的文件描述符close(i);> 
5. 改变当前工作目录 
进程活动时，其工作目录所在的文件系统不能卸下。一般需要将工作目录改变到根目录。对于需要转储核心，写运行日志的进程将工作目录改变到特定目录如/tmpchdir("/") 
6. 重设文件创建掩模 
进程从创建它的父进程那里继承了文件创建掩模。它可能修改守护进程所创建的文件的存取位。为防止这一点，将文件创建掩模清除：umask(0); 
7. 处理SIGCHLD信号 
处理SIGCHLD信号并不是必须的。但对于某些进程，特别是服务器进程往往在请求到来时生成子进程处理请求。如果父进程不等待子进程结束，子进程将成为僵尸进程（zombie）从而占用系统资源。如果父进程等待子进程结束，将增加父进程的负担，影响服务器进程的并发性能。在Linux下可以简单地将SIGCHLD信号的操作设为SIG_IGN。 
signal(SIGCHLD,SIG_IGN); 
这样，内核在子进程结束时不会产生僵尸进程。这一点与BSD4不同，BSD4下必须显式等待子进程结束才能释放僵尸进程
*/





