#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tables.h"

int errn=0, symn = 0, icn = 0, litn = 0, pooln = 0;
int literalCounter = 0;

struct ictab ic[ICN];

struct littab lit[LITN];

struct pooltab pool[POOLN];

struct symtab sym[SYMN];

struct errtab err[ERRN];

void addError(int id, int lno) {
	err[errn].id = id;
	err[errn++].lno = lno;
}

struct littab *searchLiteral(int val) {
	int i;
	for(i=0;i<litn;i++) {
		if(lit[i].value==val) return &lit[i];
	}
	return NULL;
}

struct symtab *searchSymbol(char name[100]) {
	int i;
	for(i = 0;i < symn;i++) {
		if(strcmp(sym[i].symbol,name)==0)
			return &sym[i];
	}
	return NULL;
}

int getLiteralIndex(int val) {
	int i;
	for(i=0;i<literalCounter;i++) if(lit[litn-i-1].value == val) return litn-i-1;
	return -1;
}

int getSymbolIndex(char name[100]) {
	int i;
	for(i = 0;i<symn;i++)
		if(strcmp(sym[i].symbol, name)==0)
			return i;
	return -1;
}

void unquoteNumber(char *s) {
	/*It's already assumed that number is in quotes.
	* also handles literals i.e. strings of the form
	* "='something'". */
	int i=1;
	s[0]=s[1];

	while(s[i] != '\0') {
		s[i]=s[i+1];
		i++;
	}
	s[i]='\0';
	if(s[0]=='\'')
		unquoteNumber(s); //To handle literals. Change this in the future?
}

int setPool(int litsn, int addr) {
	int i, j;
	if(litsn <= 0) return 0;
	if(pooln==0) {
		i = 1;
	}
	else {
		i = pool[pooln-1].size+pool[pooln-1].index;
	}

	for(j=0 ; j<litsn; j++ ) {
		lit[i-1+j].address = addr+j;
	}

	pool[pooln].index = i;
	pool[pooln].size = litsn;
	pooln++;
	return addr+litsn;
}

void setLiteral(char val[20]) {
	int value;
	struct littab *t;
	unquoteNumber(val);
	value = atoi(val);
	t = searchLiteral(value);
	if(t == NULL) { // create literal
		t = &lit[litn];
		t->value = value;
		litn++;
	}
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

int getMnemonicIndex(char *s) { //prolly throwaway function
	int i;
	for(i=0;i<MNEMN; i++) {
		if(strcmp(s,mnem[i])==0)
			return i;
	}
	return -1;
}

int getRegIndex(char *s) {
	int i;
	for(i=0;i<REGN;i++)
		if(strcmp(s, reg[i])==0)
			return i;
	return -1;
}

int getCCIndex(char *s) {
	int i;
	for(i=0;i<CCN;i++)
		if(strcmp(s, cc[i])==0)
			return i;
	return -1;
}

int getRegOpVal(char *s) {
	int regi, cci;
	if(s[0]=='0') return 0;
	if((regi = getRegIndex(s))!=-1) {
		return regi+1;
	}
	if((cci = getCCIndex(s))!=-1) {
		return cci+1;
	}
	return 0;
}

void addEntry(int addr, char *s1, char *s2, char *s3) {
	int x,d;
	ic[icn].addr = addr;
	x=getMnemonicIndex(s1);
	if(x>=ADSTART) {
		ic[icn].opclass = 'a';
		d=ADSTART-1;
	}
	else if(x >= DLSTART) {
		ic[icn].opclass = 'd';
		d=DLSTART-1;
	} else {
		ic[icn].opclass = 'i';
		d=ISSTART;
	}
	ic[icn].opcode = x-d;
	ic[icn].regop = getRegOpVal(s2);
	if(s3[0]!='=') { //not a literal
		x = atoi(s3);
		if(x==0 && strcmp(s3, "0")!=0) { //it's not a constant -> it's a symbol
			ic[icn].optype='s';
			ic[icn].opvalue=getSymbolIndex(s3)+1;
		} else { //it's a constant
			ic[icn].optype='c';
			ic[icn].opvalue = x;
		}
	}
	else { //it's a literal
		ic[icn].optype = 'l';
		unquoteNumber(s3);
		ic[icn].opvalue = getLiteralIndex(atoi(s3))+1;
	}
	icn++;
}

int processInstruction(int n,char * s1,char * s2,char * s3,char * s4, int lc, int lno) {
	char strtmp[20];
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
			addEntry(lc, s2, "0", s1); //add opcode and symbol
			addEntry(lc, s2, "0", s3); // add constant val
			lc+=(x-1);
		} else if(strcmp(s2,"DC")==0) {
			setSymbol(s1, -1, 1, lc);
			unquoteNumber(s3);
			x = atoi(s3);
			if(x==0 && strcmp("0",s3)!=0) {
				addError(1,lno);
			}
			addEntry(lc, s2, "0", s1); //add opcode and symbol
			addEntry(lc, s2, "0", s3); //add constant val
		}
		else if(getMnemonicIndex(s1)==-1) { //islabel
			setSymbol(s1, -1, 1, lc);
			processInstruction(2, s2, s3, s4, s1, lc, lno);
		} else {
			if(s3[0]!='=') setSymbol(s3, 1, -1, lc);
			else {
				strcpy(strtmp, s3);
				setLiteral(strtmp);
				literalCounter++;
			}

			addEntry(lc, s1, s2, s3);
		}
	} else if(n == 2) {
		if(strcmp(s1,"START")==0) {
			x = atoi(s2);
			if(x == 0 && strcmp("0", s2) != 0) {
				addError(0, lno);
			}
			addEntry(-1, s1, "0", s2);
			return x;
		} else
		if(getMnemonicIndex(s1)==-1) { //label
			setSymbol(s1, -1, 1, lc);
			processInstruction(1, s2, s3, s4, s1, lc, lno);
		} else { //operation
			if(s2[0]!='=')setSymbol(s2, 1, -1, lc); //used a symbol
			else {
				strcpy(strtmp, s2);
				setLiteral(strtmp);
				literalCounter++;
			}
			addEntry(lc, s1, "0", s2);
		}
	} else if(n == 1) {
		if(getMnemonicIndex(s1)==-1) {
			addError(2, lno);
		} else {
			if(strcmp(s1,"LTORG") == 0 || strcmp(s1, "END")==0) {
				if(literalCounter > 0) {
					addEntry(lc, s1, "0", "0");
					lc = setPool(literalCounter, lc)-1;
					literalCounter = 0;
				}
			} else
			addEntry(lc, s1, "0", "0");
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

void passtwo() { //TODO: error handling
	FILE *outfile = fopen("out.sm","w");
	int i, lc=0, memop=0,pn=0,j=0;
	if(outfile == NULL) {
		//adderror
		return;
	}
	for(i=0;i<icn;i++) {
		if(ic[i].opclass=='a') { //assembler directive
			switch(ic[i].opcode) { //prolly don't handle anything.. as we'll handle it all in pass one.
			case 1:
				//START
				if(ic[i].optype != 'c')
					//add error.. required constant after start
					break;
//				lc = ic[i].opvalue;
				break;
			case 2: //END
			case 5: //LTORG
				lc = ic[i].addr;
				for(j=0;j<pool[pn].size;j++)
					fprintf(outfile, "%03d %d\n", lc++, lit[pool[pn].index-1+j].value);
					pn++;
				break;
			}
		} else if(ic[i].opclass=='i') { //imperative
			if(ic[i].optype=='s')
				memop = sym[ic[i].opvalue-1].addr;
			else if(ic[i].optype='l') {
				memop = lit[ic[i].opvalue-1].address;
			}
			else memop = 0;
			fprintf(outfile, "%03d %02d%d%03d\n", ic[i].addr,ic[i].opcode, ic[i].regop, memop);
		} else if(ic[i].opclass=='d') { //declarative
			if(ic[i].opcode == 1) { //DC
				i++;
				fprintf(outfile, "%03d %d\n",ic[i].addr, ic[i].opvalue);
			} else if(ic[i].opcode == 2) {
				i++;
				fprintf(outfile, "%03d 0\n",ic[i].addr);
			}
		}
	}
	fclose(outfile);
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

void printLiteralTable() {
	int i;
	printf("| Address | Value |\n");
	printf("+---------+-------+\n");
	for(i=0;i<litn;i++)
		printf("|%9d|%7d|\n", lit[i].address, lit[i].value);
	printf("+-----------------+\n");
}

void printICTable() {
	int i;
	printf("+-----------------------------------------------------+\n");
	printf("| Address | Opcode | Opclass | RegOp | Optype | Opval |\n");
	printf("+---------+--------+---------+-------+--------+-------+\n");
	for(i=0;i<icn;i++) {
		printf("|%9d|%8d|%9c|%7d|%8c|%7d|\n",ic[i].addr, ic[i].opcode, ic[i].opclass, ic[i].regop, ic[i].optype, ic[i].opvalue);
	};

	printf("+-----------------------------------------------------+\n");
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
	printLiteralTable();
	printICTable();

	if(errn > 0) {
		showErrors();
		return -1;
	}

	return 0;
}
