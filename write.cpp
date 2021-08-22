/*
 * write a random number between 0 and 999 to the shm every 1 second
*/
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <error.h>

using namespace std;

int main()
{
    int shm_id;
    int *share = nullptr;
    int num;

    srand(time(nullptr)); // 初始化随机数的发生器
    cout << "page size: " << getpagesize() << endl;

    // 创建共享内存 需要指定读写权限
    shm_id = shmget (0x123456, getpagesize(), IPC_CREAT | 0666);
    if(shm_id == -1)
    {
        perror("shmget error!");
        return -1;
    }

    // 添加对共享内存的映射 attch
    share = (int *)shmat(shm_id, nullptr, 0);
    int cnt = 5;
    while(cnt--)
    {
        num = random() % 1000;
        *share = num;
        printf("write a random number %d\n", num);
        sleep(1);
    }

    // 删除对共享内存的映射 detach
    if(0 != shmdt(share))
    {
        cout << "detach share mem fail!" << endl;
    }
    return 0;
}
