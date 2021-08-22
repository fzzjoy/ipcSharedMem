/*
 * read from the shm every 1 second
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
    int *share;
    int key = 0x123456;

    int write_key = 0x111;
    int read_key = 0x222;
    int write_sem_id;
    int read_sem_id;

    // 创建并打开信号量
    write_sem_id = semget(write_key, 1, IPC_CREAT | 0666);
    read_sem_id = semget(read_key, 1, IPC_CREAT | 0666);

    // 设置信号量初始值为0
    semctl(write_sem_id, 0, SETVAL, 0);
    semctl(read_sem_id, 0, SETVAL, 0);

    shm_id = shmget (key, getpagesize(), IPC_CREAT | 0666);
    if(shm_id == -1)
    {
        perror("shmget()");
        return -1;
    }
    share = (int *)shmat(shm_id, 0, 0);

    int cnt = 5;
    struct sembuf sops;
    while(cnt--)
    {
        // P（wait()）等待生产者完成生产
        cout << "等待生产者完成生产" << endl;
        sops.sem_num = 0;
        sops.sem_op = -1; // -1
        semop(write_sem_id, &sops, 1);
        cout << "生产者已完成生产" << endl;

        printf("%d\n", *share);

        // V（signal()）消费者完成消费 
        sops.sem_num = 0;
        sops.sem_op = 1; // +1
        semop(read_sem_id, &sops, 1);

        sleep(2);
    }

    // 删除对共享内存的映射 detach
    if(0 != shmdt(share))
    {
        cout << "detach share mem fail!" << endl;
    }

    // 删除共享内存
    if(0 != shmctl(shm_id, IPC_RMID, nullptr))
    {
        perror("shmctl RMID fail!");
    }

    // 删除信号量
    if(0 != semctl(write_sem_id, 0, IPC_RMID))
    {
        perror("semctl rm write_sem_id fail!");
    }

    if(0 != semctl(read_sem_id, 0, IPC_RMID))
    {
        perror("semctl rm read_sem_id fail!");
    }
    return 0;
}