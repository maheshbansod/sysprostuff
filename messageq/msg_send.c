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
    printf("Enter message: ");
    fgets(msg.mtext, MESSAGE_SIZE, stdin);
    msgsnd(msgid, &msg, sizeof(msg),  0);
    printf("Sent %s\n", msg.mtext);
  }while(strcmp("END\n",msg.mtext) != 0);

  return 0;

}
