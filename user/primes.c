// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user/user.h"

// void tracking(int p[2]){
//     //终止条件
//     int k;//接收的第一个数
//     read(p[0],&k,sizeof(k));
//     if(k==1)exit(0);
//     //接收到的不是1,那么该数一定为质数，将其打印
//     printf("prime %d\n");

//     //定义右管道
//     int p2[2];
//     pipe(p2);
//     //父进程
//     if(fork()!=0){
//         //关闭右通道读
//         close(p2[0]);
//         //继续读左通道的值,将不能整除k的值写入右通道直到读到1
//         int n;
        
//         while(read(p[0],&n,sizeof(n)) && n!=1){
//             if(n%k !=0 )
//             write(p2[1],&k,sizeof(k));
//         }
//         write(p2[1],&n,sizeof(n));
//         close(p[0]);
//     }
//     //子进程
//     else{
//         //关闭右通道的写
//         close(p[1]);
//         close(p2[1]);
//         tracking(p2);
//         //exit(0);
//     }
//     wait(0);
//     exit(0);
// }

// int main(int argc,char**argv){
//     int p[2];
//     pipe(p);
//     //父进程
//     if(fork()!=0){
//         //关闭读端
//         close(p[0]);
//         //遍历2-35，发送给管道
//         int i;
//         for(i=2;i<36;i++){
//             write(p[1],&i,sizeof(i));
//         }
//         i=1;
//         write(p[1],&i,sizeof(i));
//         //发完了就关闭写端
//         close(p[1]);
//     }

//     //子进程
//     else{
//         //关闭写端
//         close(p[1]);
//         tracking(p);
//         exit(0);
//     }
//     wait(0);
//     exit(0);
// }

// user/primes.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 筛选质数的函数，接收一个管道作为参数
void sieve(int pleft[2]) {
	// 从左邻居读取整数
	int p;
	read(pleft[0], &p, sizeof(p));
	if(p == -1) { 
		exit(0);    // 如果读取到-1，表示结束，退出进程
	}
	printf("prime %d\n", p);    // 此时接收到的数字肯定是质数

    // 创建一个新的管道
    int pright[2];
	pipe(pright); 

	if(fork() == 0) {       // 右邻居
		close(pright[1]);   // 右邻居用不到这个管道的写端，关闭
		close(pleft[0]);    // 右邻居用不到这个管道的读端，关闭
		sieve(pright);      // 递归调用筛选函数

	} else {
        close(pright[0]);       // 当前进程用不到这个管道的读端，关闭
        // 从左邻居接收数字
        int buf;
		while(read(pleft[0], &buf, sizeof(buf)) && buf != -1) { 
			if(buf % p != 0) {                              // 如果接收到的数字不是第一次接收到的数字的倍数
				write(pright[1], &buf, sizeof(buf));        // 才往管道中给右邻居写入这个数字
			}
        }

        // 此时接收到了左邻居传来的-1，要给右邻居也传-1，结束右邻居进程
        buf = -1;
		write(pright[1], &buf, sizeof(buf)); 
		wait(0); 
		exit(0);
	}
}

int main(int argc, char **argv) {
	// 创建初始管道
	int input_pipe[2];
	pipe(input_pipe); 

	if(fork() == 0) {				// 右邻居
		close(input_pipe[1]); 		// 右邻居用不到这个管道的写端，关闭右邻居的管道写文件描述符
		sieve(input_pipe);			// 调用筛选函数
		exit(0);
	} else {						// 父进程	
		close(input_pipe[0]); 		// 父进程只会向管道中给右邻居写数据，关闭父进程的管道读文件描述符
		int i;
		for(i=2;i<=35;i++){
			write(input_pipe[1], &i, sizeof(i));	// 向管道写入2~35的整数
		}
		// 写入结束标志
		i = -1;
		write(input_pipe[1], &i, sizeof(i)); 
	}
	wait(0); 	// 等待子进程结束
	// 注意：这里无法等待子进程的子进程，只能等待直接子进程，无法等待间接子进程。
	// 在 sieve() 中再各自执行 wait(0)，形成等待链
	exit(0);	// 退出进程
}