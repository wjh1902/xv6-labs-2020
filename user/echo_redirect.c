//by wjh：将echo输出重定向到echo_redirect.txt文件

#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

int main(int argc, char** argv) {
    if (fork() == 0) {  // 子进程
        close(1);  // 关闭标准输出
        open("echo_redirect.txt", O_RDONLY|O_WRONLY|O_CREATE);  // 打开文件，并将标准输入重定向到该文件
        char *record[argc];
        record[0]="echo";
        for(int i=1;i<argc;i++){
            record[i]=argv[i];
        }
        record[argc]=0;
        exec("echo", record);  // 执行 cat 命令
    }
    wait(0);  // 父进程等待子进程结束
    exit(0);  // 父进程退出
}
