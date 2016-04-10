#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>
 
#define BUF_LEN 256
#define N 20  // count of threads : Writers  1 , Readers  (N - 1)  ;
#define FILE_NAME_LEN 20
 
const char quit[] = "quit";
 
pthread_rwlock_t rwlock;
pthread_rwlock_t flock;
 
int msglen;
char msgbuf[BUF_LEN];
char fname[FILE_NAME_LEN];
FILE *f;
 
 
void * writer(void *argp)
{
 while(1) {
 
  pthread_rwlock_wrlock(&rwlock);
 
  if (!msglen) {
 
   scanf("%s",msgbuf);   
   if(strcmp(msgbuf,quit) == 0) {
     break;
   }
   msglen = strlen (msgbuf) + 1;
  }
 
  pthread_rwlock_unlock(&rwlock);
  sched_yield();
 
 }
 
 printf ("Thread %i exits\n", (int)pthread_self());
 pthread_rwlock_unlock(&rwlock);
 return 0;
}
 
void * reader(void * argp)
{
 while(1){
  pthread_rwlock_rdlock(&rwlock);
 
  if (strcmp(msgbuf,quit) == 0)
  {
    printf ("Thread %i exits\n", (int)pthread_self());
    pthread_rwlock_unlock(&rwlock);
   return 0;
  }
 
  pthread_rwlock_wrlock(&flock);
 
  if (msglen) {
   printf ("Thread %i readed. \n",(int)pthread_self());
 
   f = fopen(fname,"a+");
   fprintf(f,"%s\n",msgbuf);
   fflush(f);
   fclose(f);
 
   msglen = 0;
  }
 
  pthread_rwlock_unlock(&flock);
  pthread_rwlock_unlock(&rwlock);
  sched_yield();
 }
}
 
int main(int argc,char **argv)
{
  if(argc != 2) {
    printf("Usage: %s <FILE_NAME> \n",argv[0]);
    return 1;
  } else {
    strcpy(fname,argv[1]);
  }

  pthread_t thread[N];

  if (pthread_rwlock_init(&rwlock, NULL)) {}
  if (pthread_rwlock_init(&flock, NULL)) {}

  if (pthread_create(&thread[0], NULL, writer, NULL)) {}

  int i;
  for(i = 0;i < N;i++)
  if (pthread_create(&thread[i], NULL, reader, NULL)) {}

  for(i = 0;i < N;i++)  {
    pthread_join(thread[i], NULL);
  }
  
  if (pthread_rwlock_destroy(&rwlock)) { }
  if (pthread_rwlock_destroy(&flock)) { }
  
  return 0;
}