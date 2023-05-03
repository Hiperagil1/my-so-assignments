#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include "a2_helper.h"
#include <semaphore.h>
#include <fcntl.h>


sem_t sem3;
sem_t sem5;

typedef struct {
    int no_process;
    int no_thread;
} TH_STRUCT;

void *threads(void *param) {
    TH_STRUCT *s = (TH_STRUCT *)param;
    

    if (s->no_thread == 3) {
        // Firul de execuție 3 începe imediat
        sem_wait(&sem5);
        info(BEGIN, s->no_process, s->no_thread);
        info(END, s->no_process, s->no_thread);
        sem_post(&sem3);
    }  else if (s->no_thread == 5) { 
        // Firul de execuție 5 așteaptă semnalul semaforului 3 înainte de a începe execuția
        info(BEGIN, s->no_process, s->no_thread);
        sem_post(&sem5);
        sem_wait(&sem3);
        info(END, s->no_process, s->no_thread);
    } else if (s->no_thread != 3 && s->no_thread != 5){
        info(BEGIN, s->no_process, s->no_thread);
        info(END, s->no_process, s->no_thread);
    }

    return NULL;
}

int main() {
    init();
    info(BEGIN, 1, 0);

    // Process 2
    if (fork() == 0) {
        info(BEGIN, 2, 0);
        info(END, 2, 0);
    } else {
        wait(NULL);

        // Process 3
        if (fork() == 0) {
            info(BEGIN, 3, 0);

            // Process 4
            if (fork() == 0) {
                info(BEGIN, 4, 0);
                info(END, 4, 0);
            } else {
                wait(NULL);
                info(END, 3, 0);
            }
        } else {
            wait(NULL);

            // Process 5
            if (fork() == 0) {
                info(BEGIN, 5, 0);

                // Process 6
                if (fork() == 0) {
                    info(BEGIN, 6, 0);
                    info(END, 6, 0);
                } else {
                    wait(NULL);
                    info(END, 5, 0);
                }
            } else {
                wait(NULL);

                // Process 7
                if (fork() == 0) {
                    info(BEGIN, 7, 0);

                    TH_STRUCT params[5];
                    pthread_t tid[5];
                    
                    if(sem_init(&sem3, 0, 0) != 0){
                        perror("Could not init the semaphore logSem.");
                        return -1;
                    }
                    
                    if(sem_init(&sem5, 0, 0) != 0){
                        perror("Could not init the semaphore logSem.");
                        return -1;
                    }
                    
        
                    // Thread 1
                    params[0].no_process = 7;
                    params[0].no_thread = 1;
                    pthread_create(&tid[0], NULL, threads, &params[0]);

                    // Thread 2
                    params[1].no_process = 7;
                    params[1].no_thread = 2;
                    pthread_create(&tid[1], NULL, threads, &params[1]);

                    // Thread 5
                    params[4].no_process = 7;
                    params[4].no_thread = 5;
                    pthread_create(&tid[4], NULL, threads, &params[4]);
    
                    // Thread 3
                    params[2].no_process = 7;
                    params[2].no_thread = 3;
                    pthread_create(&tid[2], NULL, threads, &params[2]);
                
                    // Thread 4
                    params[3].no_process = 7;
                    params[3].no_thread = 4;
                    pthread_create(&tid[3], NULL, threads, &params[3]);
                    
                    
                    pthread_join(tid[3], NULL);
                    pthread_join(tid[2], NULL);     
                    pthread_join(tid[0], NULL);
                    pthread_join(tid[1], NULL);
                    pthread_join(tid[4], NULL);
                        
                    sem_destroy(&sem3);
                    sem_destroy(&sem5);
                    info(END, 7, 0);
                } else {
                    wait(NULL);
                    info(END, 1, 0);
                }
            }
        }
    }

    return 0;
}
