#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct line {
	int lno;
	char *data;
	struct line* next;
} Line;

int main(int argc, char **argv) {
	char str[1000],exit=0;
	int i=0, n= 0;
	FILE *fp;
	Line* head = (Line*)malloc(sizeof(Line));
	Line *temp = head;
	Line *temp2;
	head->next = NULL;

	if(argc < 2) {
		printf("Enter filename as argument.\n");
		return -1;
	}
	if((fp = fopen(argv[1], "r"))==NULL) {
		printf("Couldn't open the file.\n");
		return -1;
	}

	printf("Loading file...\n");

	while(!feof(fp)) {
		fgets(str, 999, fp);
		temp->next = (Line*)malloc(sizeof(Line));
		temp = temp->next;
		temp->data = (char*)malloc(strlen(str)+1);
		strcpy(temp->data, str);
		temp->next = NULL;
	}
	fclose(fp);
	fp = NULL;
	printf("Done.\n");

	do {
		printf("$ ");
		fgets(str, 999, stdin);
		switch(str[0]) {
		case 'a':
			if(str[1]!='\n') {
				printf("Invalid command or wrong syntax for command 'a'\n");
				break;
			}
			printf("Enter the data to append(enter -1 on a new line to stop):\n");
			str[0]='\0';
			temp = head;
			while(temp->next != NULL) temp=temp->next;

			while(1) {
				fgets(str, 999, stdin);
				if(atoi(str) == -1 && strlen(str)==3)
					break;
				temp->next = (Line*)malloc(sizeof(Line));
				temp = temp->next;
				temp->data = (char*)malloc(strlen(str)+1);
				strcpy(temp->data, str);
				temp->next = NULL;
			}

			break;
		case 'p':
			if(str[1] != '\n') {
				printf("Invalid command or wrong syntax for 'p'\n");
				break;
			}
			temp = head->next;
			i=1;
			while(temp != NULL) {
				printf("%d: %s",i++, temp->data);
				temp=temp->next;
			}
			break;

		case 'd':
			i=sscanf(str,"%c %d", &i, &n);
			printf("'%s', n'%d'\n",str,n);
			if(i!=2) {
				printf("Invalid command or invalid syntax for 'd'\n");
				break;
			}
			temp = head;
			i=1;
			while(temp->next != NULL && i < n) {
				temp = temp->next;
				i++;
			}
			if(temp->next == NULL) {
				printf("Line number out of  range\n");
			} else if(i==n) {
				temp2 = temp->next;
				temp->next = temp2->next;
				free(temp2->data);
				temp2->data = NULL;
				free(temp2);
			}
			break;
		case 'e':
			printf("Exit\n");
			exit = 1;
			break;
		default:
			printf("Invalid command.\n");
			break;
		}
	}while(!exit);
}
