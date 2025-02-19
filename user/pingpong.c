#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

#define MAX_BUF 1024  // 定义最大缓冲区大小

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: pingpong <string>\n");
        exit(1);
    }

    // 定义两个管道
    int p1[2], p2[2];
    pipe(p1);
    pipe(p2);

    char buf[MAX_BUF];
    int len = strlen(argv[1]);

    // 打开文件记录交换过程
    close(1);
    open("record.txt", O_WRONLY | O_CREATE);

    // 父进程
    if (fork() != 0) {
        // 写入命令行传入的字符串到管道1
        write(p1[1], argv[1], len);
        close(p1[1]);

        // 从管道2读取子进程返回的内容
        read(p2[0], buf, len);
        buf[len] = '\0';  // 确保字符串结尾
        printf("%d: received pong: %s\n", getpid(), buf);

        // 记录交换过程到文件
        write(1, "Parent sent: ", 13);
        write(1, argv[1], len);
        write(1, "\n", 1);

        write(1, "Parent received: ", 17);
        write(1, buf, len);
        write(1, "\n", 1);

        // 等待子进程结束
        wait(0);
    }

    // 子进程
    else {
        // 从管道1读取父进程传来的字符串
        read(p1[0], buf, len);
        buf[len] = '\0';  // 确保字符串结尾
        printf("%d: received ping: %s\n", getpid(), buf);

        // 写入相同的字符串到管道2（发送给父进程）
        write(p2[1], buf, len);
        close(p2[1]);

        // 记录交换过程到文件
        write(1, "Child received: ", 16);
        write(1, buf, len);
        write(1, "\n", 1);

        write(1, "Child sent: ", 12);
        write(1, buf, len);
        write(1, "\n", 1);
    }

    close(p1[0]);
    close(p2[0]);
    exit(0);
}
