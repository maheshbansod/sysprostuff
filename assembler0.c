#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tables.h"

#define SYMN 100
#define MNEMN 17
#define ERRN 100

char mnem[][10] = {"STOP", "ADD", "SUB", "MULT",
	"MOVER", "MOVEM", "COMP", "BC",
	"DIV", "READ", "PRINT", "DS",
	"DC", "START", "END", "LTORG",
	"ORIGIN"};
char errmsg[][100] = {"Invalid operand. Expected integer.",
					"Invalid operand. Expected quoted integer.",
					"Not an instruction"};

int errn=0, symn = 0;

struct symtab sym[SYMN];

struct errtab err[ERRN];

void addError(int id, int lno) {
	err[errn].id = id;
	err[errn++].lno = lno;
}

struct symtab *searchSymbol(char name[100]) {
	int i;
	for(i = 0;i < symn;i++) {
		if(strcmp(sym[i].symbol,name)==0)
			return &sym[i];
	}
	return NULL;
}

void setSymbol(char name[100], char used, char defined, int addr) {
	struct symtab *s;
	s = searchSymbol(name);
	if(s == NULL) { //create new symbol
		s = &sym[symn];
		strcpy(sym[symn].symbol, name );
		s->used = s->defined = s->addr = -1;
		symn++;
	}
	if(used != -1) s->used = used;
	if(defined != -1) {
		s->defined = defined;
		s->addr = addr;
	}
}

int hasUndefinedSymbols() {
	int i, exit = 0;
	for(i=0;i<symn;i++) {
		if(sym[i].used == 1 && sym[i].defined == -1) {
			printf("Error: symbol '%s' used but not defined.\n", sym[i].symbol);
			exit = 1;
		} else if(sym[i].defined == 1 && sym[i].used == -1) {
			printf("Warning: Symbol '%s' defined but not used.\n", sym[i].symbol);
		}
	}
	return exit;
}

int isMnemonic(char *s) { //prolly throwaway function
	int i;
	for(i=0;i<MNEMN; i++) {
		if(strcmp(s,mnem[i])==0)
			return 1;
	}
	return 0;
}

void unquoteNumber(char *s) {
	/*It's already assumed that number is in quotes*/
	int i=1;
	s[0]=s[1];
	while(s[i] != '\0' && s[i] != '\'') {
		s[i]=s[i+1];
		i++;
	}
	s[i]='\0';
}

int processInstruction(int n,char * s1,char * s2,char * s3,char * s4, int lc, int lno) {
	int x;
	if(n==4) {
		setSymbol(s1, -1, 1, lc); //first one is label
		processInstruction(3, s2, s3, s4, s1, lc, lno);
	} else if(n == 3) {
		if(strcmp(s2,"DS")==0) {
			setSymbol(s1,-1, 1, lc);
			x = atoi(s3);
			if(x==0 && strcmp(s4, "0") != 0) {
				addError(0, lno);
			}
			lc+=(x-1);
		} else if(strcmp(s2,"DC")==0) {
			setSymbol(s1, -1, 1, lc);
			unquoteNumber(s3);
			x = atoi(s3); //set this constant at the mem
			if(x==0 && strcmp("0",s3)!=0) {
				addError(1,lno);
			}
		}
		else if(!isMnemonic(s1)) { //islabel
			setSymbol(s1, -1, 1, lc);
			processInstruction(2, s2, s3, s4, s1, lc, lno);
		} else {
			setSymbol(s3, 1, -1, lc); //used a symbol
		}
	} else if(n == 2) {
		if(strcmp(s1,"START")==0) {
			x = atoi(s2); //handle this exception
			if(x == 0 && strcmp("0", s3) != 0) {
				addError(0, lno);
			}
			return x;
		} else
		if(!isMnemonic(s1)) {
			setSymbol(s1, -1, 1, lc);
			processInstruction(1, s2, s3, s4, s1, lc, lno);
		} else {
			setSymbol(s2, 1, -1, lc); //used a symbol
		}
	} else if(n == 1) {
		if(!isMnemonic(s1)) {
			addError(2, lno);
		} else {
			;//handle mnemonic
		}
	}
	return lc+1;
}

void passone(FILE *fp) {
	
	char s1[30], s2[30], s3[30], s4[30], str[120];
	int lc, n, lno;

	fgets(str, 120, fp);
	n = sscanf(str, "%s %s %s %s\n", s1, s2, s3, s4);
	lc=lno=1;

	while(!feof(fp)) {
		lc = processInstruction(n, s1, s2, s3, s4, lc, lno);
		fgets(str, 120, fp);
		n = sscanf(str, "%s %s %s %s", s1, s2, s3, s4);
		lno++;
	}
}

void passtwo() {
}

void showErrors() {	
	int i;
	for(i=0;i < errn;i++) {
		printf("Line %3d: %s\n",err[i].lno,errmsg[err[i].id]);
	}
}

void printSymbolTable() {
	int i;
	printf("| Address | Symbol | Used | Defined |\n");
	printf("+---------+--------+------+---------+\n");
	for(i=0;i<symn;i++) {
		printf("|%9d|%8s|%6d|%9d|\n",sym[i].addr,sym[i].symbol,sym[i].used,sym[i].defined);
	}
	printf("-------------------------------------\n");
}

int main( int argc, char **argv) {
	FILE *fp;

	if(argc < 2) {
		printf("Specify a .asm file as a command line argument.\n");
		return -2;
	}

	fp = fopen(argv[1], "r");
	if(fp == NULL) {
		printf("Invalid file provided.\n");
		return -3;
	}

	passone(fp);
	if(hasUndefinedSymbols()) {
		return -1;
	}

	if(errn > 0) {
		showErrors();
		return -1;
	} else passtwo();
	printSymbolTable();

	if(errn > 0) {
		showErrors();
		return -1;
	}

	return 0;
}
