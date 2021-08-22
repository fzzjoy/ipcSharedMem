/*
 * read from the shm every 1 second
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
    int *share;
    shm_id = shmget (0x123456, getpagesize(), IPC_CREAT | 0666);
    if(shm_id == -1)
    {
        perror("shmget()");
        return -1;
    }
    share = (int *)shmat(shm_id, 0, 0);

    int cnt = 10;
    while(cnt--)
    {
        sleep(1);
        printf("%d\n", *share);
    }

    // 删除对共享内存的映射 detach
    if(0 != shmdt(share))
    {
        cout << "detach share mem fail!" << endl;
    }
    return 0;
}