#include <stdio.h>
#include <string.h>

struct mnttab {
	char name[30];
	int pp;
	int kp;
	int pntptr;
	int kpdptr;
	int mdtptr;
} mnt[100];

struct kpdtab {
	int kn;
	char value[100];
} kpd[100];

struct mdtab {
	char label[100];
	char str[4][50];
	int i[4];
} mdt[100];

int mntn = 0, pntn = 0, kpdn = 0, mdtn = 0;

char pnt[50][100];

void getMacroHeader(char *str) {
	int i,c=0,j=0;
	char rec=0, recv;

	//macro name
	for(i=0;str[i]!='\0';i++) {
		mnt[mntn].name[i]=str[i];
		if(str[i]==' ') break;
	}
	mnt[mntn].name[i]='\0';

	mnt[mntn].pntptr = pntn;
	//positional parameter
	for(i=0;str[i]!='\0';i++) {
		if(str[i] == '&') {
			rec=1;
			j=0;
		} else if(str[i]=='=') {
			rec=0;
			j=0;
		} else if(str[i]==' ' || str[i]=='\n') {
			rec = 0;
			if(j > 0) {
				pnt[pntn++][j]='\0';
				c++;
			}
			j=0;
		}

		if(rec) {
			pnt[pntn][j++]=str[i];
		}
	}
	mnt[mntn].pp = c;
	
	mnt[mntn].kpdptr = kpdn;
	rec=c=j=0;
	//formal parameter
	for(i=0;str[i]!='\0';i++) {
		if(str[i]=='&') {
			rec = 1;
			j = 0;
		} else if(str[i]=='=') {
			rec = 0;
			recv = 1;
			pnt[pntn++][j]='\0';
			c++;
			j = 0;
		} else if(str[i]==' ' || str[i]=='\n') {
			if(j > 0 && recv) {
				kpd[kpdn].kn = pntn-1;
				kpd[kpdn++].value[j]='\0';
			}
			rec = recv = 0;
			j=0;
		}
		if(rec) {
			pnt[pntn][j++] = str[i];
		} else if(recv) {
			if(str[i]!='=')
				kpd[kpdn].value[j++] = str[i];
		}
	}
	mnt[mntn].kp = c;
}

int searchParameter(char *str, int i) {
	for(i=mnt[i].pntptr ;i<pntn;i++) if(strcmp(pnt[i],str)==0) return i;
	return -1;
}

void addToMDT(char *str) {
	char s[4][50];
	int n, i;

	n = sscanf(str, "%s %s %s %s",s[0], s[1], s[2], s[3]);

	for(i=n;i<4;i++) {
		s[i][0]='\0';
	}
	for(i=0;i<4;i++) {
		if(s[i][0]=='&') {
			strcpy(mdt[mdtn].str[i],"p");
			mdt[mdtn].i[i] = searchParameter(s[i], mntn);
		} else {
			strcpy(mdt[mdtn].str[i],s[i]);
			mdt[mdtn].i[i] = -1;
		}
	}
	mdtn++;
}

void displayKPD() {
	int i;
	printf("| KN | Param | Value |\n");
	printf("+----+-------+-------+\n");
	for(i=0;i<kpdn;i++) {
		printf("|%4d|%7s|%7s|\n",kpd[i].kn,pnt[kpd[i].kn],kpd[i].value);
	}
	printf("+--------------------+\n");
}

void displayPNT() {
	int i;
	printf("| Param |\n");
	printf("+-------+\n");
	for(i = 0;i < pntn;i++) {
		printf("|%7s|\n",pnt[i]);
	}
	printf("+-------+\n");
}

void displayMNT() {
	int i;
	printf("| Name | PP | KP | PNTPTR | KPDPTR | MDTPTR |\n");
	printf("+------+----+----+--------+--------+--------+\n");
	for(i = 0;i<mntn;i++)
		printf("|%6s|%4d|%4d|%8d|%8d|%8d|\n", mnt[i].name, mnt[i].pp, mnt[i].kp, mnt[i].pntptr, mnt[i].kpdptr,mnt[i].mdtptr);
	printf("+-------------------------------------------+\n");
}

void displayMDT() {
	int i,j;
	for(i=0;i<mdtn;i++) {
		for(j=0;j<4;j++) {
			if(mdt[i].i[j]==-1) printf("%s ", mdt[i].str[j]);
			else printf("(p, %d) ", mdt[i].i[j]);
		}
		printf("\n");
	}
}

int main(int argc, char **argv) {
	FILE *fp;
	char str[100];

	if(argc < 2) {
		printf("Enter a .asm file as argument.\n");
		return -1;
	}
	fp = fopen(argv[1], "r");
	if(fp == NULL) {
		printf("Could not open '%s'.\n", argv[1]);
		return -1;
	}

	while(!feof(fp)) {
		fgets(str, 80, fp);
		if(strcmp(str, "MACRO\n")==0) {
			fgets(str, 80, fp);
			getMacroHeader(str);
			fgets(str,80, fp);
			mnt[mntn].mdtptr = mdtn;
			while(strcmp(str, "MEND\n")!=0) {
				addToMDT(str);
				fgets(str, 80, fp);
			}
			mntn++;
		}
	}

	displayMNT();
	displayKPD();
	displayPNT();
	displayMDT();
}
