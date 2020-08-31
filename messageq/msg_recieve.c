#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#include "defs.h"

struct msgbuf {
  long mtype;
  char mtext[MESSAGE_SIZE];
};

int main() {
  int msgid;
  key_t key = ftok(FILE_NAME, PROJECT_ID);
  struct msgbuf msg;
  msg.mtype = MESSAGE_TYPE;

  msgid = msgget(key, IPC_CREAT | 0666);

  do {
    printf("Waiting for sender...\n");
    msgrcv(msgid, &msg, sizeof(msg), msg.mtype, 0);
    printf(">> %s\n", msg.mtext);
  }while(strcmp("END\n",msg.mtext) != 0);

  msgctl(msgid, IPC_RMID, NULL);

  return 0;

}
