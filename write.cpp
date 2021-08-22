/*
 * write a random number between 0 and 999 to the shm every 1 second
*/
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <error.h>
#include <sys/sem.h>

using namespace std;

int main()
{
    int shm_id;
    int write_sem_id;
    int read_sem_id;
    int *share = nullptr;
    int num;
    int key = 0x123456;
    int write_key = 0x111;
    int read_key = 0x222;

    srand(time(nullptr)); // 初始化随机数的发生器

    // 创建并打开信号量
    write_sem_id = semget(write_key, 1, IPC_CREAT | 0666);
    read_sem_id = semget(read_key, 1, IPC_CREAT | 0666);

    // 设置信号量初始值为0
    semctl(write_sem_id, 0, SETVAL, 0);
    semctl(read_sem_id, 0, SETVAL, 0);

    // 创建共享内存 需要指定读写权限
    cout << "page size: " << getpagesize() << endl;
    shm_id = shmget (key, getpagesize(), IPC_CREAT | 0666);
    if(shm_id == -1)
    {
        perror("shmget error!");
        return -1;
    }

    // 添加对共享内存的映射 attch
    share = (int *)shmat(shm_id, nullptr, 0);
    int cnt = 5;
    struct sembuf v = {0, 1, SEM_UNDO};
    struct sembuf p = {0, -1, SEM_UNDO};
    while(cnt--)
    {
        num = random() % 1000;

        *share = num;
        printf("write a random number %d\n", num);

        // V（signal()）生产者完成生产 
        semop(write_sem_id, &v, 1);

        // P（wait()）等待消费者消费
        cout << "等待消费者完成消费" << endl;
        auto ret = semop(read_sem_id, &p, 1);
        if(ret != 0)
        {
            perror("semop read_sem_id error!");
        }
        cout << "消费者已完成消费" << endl;
    }

    // 删除对共享内存的映射 detach
    if(0 != shmdt(share))
    {
        cout << "detach share mem fail!" << endl;
    }
    return 0;
}
