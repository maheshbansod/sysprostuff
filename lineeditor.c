#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LSIZE 500

typedef struct editbuff {
	char *line; //character string
	struct editbuff *next;
} List;
List *head, *last;

void readBuffer(FILE *f);
int countLines();
void eprint(int, int);
void einsert(int);
void edelete(int, int);
void ecopy(int, int, int);
void emove(int, int,int );
void efind(int, int, char*);
int countSpaces(char*);
List* copyList(int, int);
void pasteList(int, List*);

main(int argc, char **argv) {
	char cmdline[30],str[500],cmd, *filename;
	char alive = 1, ch;
	int n1, n2, n3, n;
	FILE *file, *f2;
	List *t;
	
	if(argc < 2) {
		printf("No filename specified.\n");
		return -1;
	}

	filename = argv[1];
	file = fopen(filename, "r");

	readBuffer(file);
	
	printf("%d lines\n", countLines());
	
	while(alive) {
		printf("? ");
		fgets(cmdline, 30, stdin);

		n1 = n2 = 0;

		sscanf(cmdline, "%c %d %d %d", &cmd, &n1, &n2, &n3);
		n = countSpaces(cmdline);

		switch(cmd) {
		case 'e':
			//Exit
			printf("Exit\n");
			alive = 0;
			break;
		case 'p':
			//print
			if(n < 2) {
				n2 = 1;
				if(n < 1)
					n1 = 1;
			}
			eprint(n1, n2);
			break;
		case 'i':
			if(n < 1) n1 = 1;
			einsert(n1);
			break;
		case 's':
			if((fopen(filename,"r"))!=NULL) { //file exist
				printf("Overwrite existing file(y/n)? ");
				fgets(str, 500, stdin);
				if(strcmp("n",str)==0) {
					printf("Enter new filename: ");
					fgets(filename,20 , stdin);
				}
			}
			f2 = fopen(filename,"w");

			t = head->next;	
			while(t != NULL) {
				fputs(t->line, f2);
				fputc('\n',f2);
				t = t->next;
			}
			fclose(f2);
			break;
		case 'd':
			if(n < 2) {
				n2 = 1;
				if(n < 1)
					n1 = 1;
			}
			edelete(n1,n2);
			break;
		case 'c':
			if(n<3) {
				n3=1;
				if(n<2) {
					n2 = 1;
					if(n<1)
						n1=1;
				}
			}
			ecopy(n1, n2, n3);
			break;
		case 'm':
			if(n<3) {
				n3=1;
				if(n<2) {
					n2 = 1;
					if(n<1)
						n1=1;
				}
			}
			emove(n1,n2,n3);
			break;
		case 'f':
			if(n<2) {
				n2 = 1;
				if(n<1)
					n1=1;
			}
			printf("Enter pattern: ");
			fgets(str, 500, stdin);
			efind(n1, n2, str);
			break;
		default:
			printf("invalid operation.\n");
			break;
		}
	}
}

void efind(int n1, int n2, char *str) {
	List *t=head->next;
	int n=1,c=0;

	//navigate to line n1
	while(t != NULL) {
		t=t->next;
		if(n==n1) break;
		n++;
	}

	printf("Result:\n");

	while( t!= NULL) {
		if(strstr(t->line,str)) {
			printf("%d: %s\n",n+1,t->line);
			c++;
		}
		n++;
		if(n>=n2) break;
		t=t->next;
	}
	
	if(c == 0) printf("No line contains the given pattern");
}

void emove(int n1, int n2, int n3) {
	List *t = copyList(n1,n2);
	edelete(n1,n2);
	pasteList(n3,t);
}

void edelete(int n1, int n2) {
	int n=1;
	List *t = head->next, *p=head, *todel;
	while(t != NULL) {
		if(n1 <= n) {
			todel = t;
			p->next = t->next;
			free(t->line);
			free(todel);
			t = p;
			if(n2 <= n) return;
		}
		n++;
		p = t;
		t = t->next;
	}
}

List* copyList(int n1, int n2) {
	List *tn = (List*)malloc(sizeof(List)), *t = head, *tnh;
	int n = 0;
	tn->next = NULL;
	tnh = tn;

	while(n< n1 && t->next != NULL) {
		t=t->next;
		n++;
	}

	while( t != NULL) {
		tn->next = (List*) malloc(sizeof(List));
		tn=tn->next;
		tn->line = (char*)malloc(sizeof(char)*(strlen(t->line)+1));
		strcpy(tn->line,t->line);
		tn->next = NULL;
		if(n2<=n) break;
		n++;
		t=t->next;
	}

	return tnh;
}

void pasteList(int n1, List *l) {
	int n=0;
	List *t = head, *ln;
	//navigate to line (n1-1)
	while(t != NULL) {
		n++;
		if(n==n1) break;
		t=t->next;
	}
	
	//insert list
	l = l->next;
	while(l != NULL) {
		ln = l->next;
		l->next = t->next;
		t->next = l;
		l=ln;
		t = t->next;
	}	
}

void ecopy(int n1, int n2, int n3) {
	List *t;
	t = copyList(n1,n2);
	pasteList(n3,t);
}

void einsert(int n1) {
	int n=1;
	char str[500];
	List *t = head, *newnode;

	//navigate to line n1
	while(n < n1 && t->next != NULL) {
		t = t->next;
		n++;
	}

	printf("Enter text:\n");
	while(1) {
		fgets(str, 500, stdin);
		if(strcmp(str, ".")==0) return;
		newnode = (List*)malloc(sizeof(List));
		newnode->next = t->next;
		newnode->line = (char*)malloc(sizeof(char)*(strlen(str)+1));
		strcpy(newnode->line, str);
		t->next = newnode;
		t = newnode;
	}
}

void eprint(int n1, int n2) {
	int n = 1;
	List *t = head->next;
	while(t != NULL) {
		if(n1 <= n) {
			printf("%d: %s\n",n, t->line);
			if(n2 <= n) return;
		}
		t = t->next;
		n++;
	}
}

int countSpaces(char *str) {
	int i=0, n=0;
	while(str[i] != '\0') {
		if(str[i]==' ') n++;
		i++;
	}
	return n;
}

void readBuffer(FILE *f) {
	char ch, linel[LSIZE], i=0;
	List *t = NULL, *next;
	head = (List*) malloc(sizeof(List));
	head->next = NULL;
	next = head;
	if(f == NULL) return;
	while((ch = fgetc(f))!=EOF) {
		if(ch == '\n') {
			//store to ll
			linel[i]='\0';
			t = (List *) malloc(sizeof(List));
			next->next = t;
			t->next = NULL;
			t->line = (char *)malloc((strlen(linel)+1)*sizeof(char));
			strcpy(t->line, linel);
			next = next->next;
			i=0;
		} else {
			linel[i++]=ch;
		}
	}

	if(t!=NULL) last = t;
}

int countLines() {
	int n=0;
	List *t = head;
	while(t->next != NULL) {
		n++;
		t = t->next;
	}
	return n;
}
