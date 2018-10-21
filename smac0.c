#include <stdio.h>
#include <string.h>

/*takes .sm file(machine code) and runs the program*/

#define REGNUM 4
#define MEMSIZE 1000

#define TEMP_FILE_NAME "smac0.stdin_source.sm.tmp"

void inputSFixer() {
	char c;
	while( (c = getchar()) != '\n' && c != EOF);
}

int reg[REGNUM]={0}, mem[MEMSIZE]={0};
char cc[6]={0,0,0,0,0,1};

int loadFile(char *fname) {
	int addr, instr, l=1;
	FILE *fp;
	fp = fopen(fname, "r");
	if(fp == NULL) {
		printf("Couldn't open file.\n");
		return -2;
	}

	fscanf(fp, "%d %d", &addr, &instr);
	while(!feof(fp)) {
		
		if(addr < MEMSIZE) mem[addr] = instr; //set LC
		else {
			printf("Exception at line %d. Address out of range.\n", l);
			fclose(fp);
			return -1;
		}

		fscanf(fp, "%d %d", &addr, &instr);
		l++;
	}
	fclose(fp);

	return 0;
}

void run(int st, int debug) {
	int opcode, memop, regop, i;
	
	printf("**********Execution start**********\n");

	while(mem[st] != 0 && st < MEMSIZE) {
		opcode = mem[st]/10000;
		regop = (mem[st]/1000)%10 - 1;
		memop = mem[st]%1000;

		if(regop < -1 || regop >= (REGNUM>6?REGNUM:6)) {
			printf("Address %03d: Register operand out of range.\n", st);
			break;
		}

		switch(opcode) {
		case 10:
			printf("=> %d\n",mem[memop]);
			break;
		case 9:
			printf("in: ");
			scanf("%d", &mem[memop]);
			break;
		case 8:
			if(mem[memop]==0) {
				printf("Address %03d: Division by 0 exception.\n",st);
				st = MEMSIZE;
				break;
			}
			reg[regop] /= mem[memop];
			break;
		case 7:
			if(cc[regop]==1) st=memop-1;
			break;
		case 6:
			if(reg[regop] < mem[memop]) { cc[0]=cc[1]=1; cc[2]=cc[3]=cc[4]=0; }
			else if(reg[regop] > mem[memop]) { cc[3]=cc[4]=1; cc[0]=cc[1]=cc[2]=0; }
			else if(reg[regop] == mem[memop]) {cc[2]=cc[1]=cc[4]=1; cc[0]=cc[3]=0; }
			break;
		case 5:
			mem[memop] = reg[regop];
			break;
		case 4:
			reg[regop] = mem[memop];
			break;
		case 3:
			reg[regop] *= mem[memop];
			break;
		case 2:
			reg[regop] -= mem[memop];
			break;
		case 1:
			reg[regop] += mem[memop];
			break;
		case 0: break;
		default:
			printf("Address %03d: Invalid opcode.\n",st);
			st = MEMSIZE;
			break;
		}
		if(debug) {
			printf("instr: %d\nmemval: %d\tregid: %d\n",mem[st],mem[memop],regop);
			printf("registers = {");
			for(i = 0;i<REGNUM;i++) {
				printf(" <%d> ", reg[i]);
			}
			printf("}\nPress any key to continue.\n");
			getchar();
		}

		st++;
	}
	printf("**********Execution   end**********\n");
}

int main(int argc, char *argv[]) {
	char alive = 1;
	int ch = 1,st,end, i;
	char fname[1024];
	int n;
	FILE *fp;
	//char cmd[1024];

	if(argc >= 2) {
		if(loadFile(argv[1])==0) printf("Done loading '%s'.\n", argv[1]);
		else printf("'%s' was not loaded.\n", argv[1]);
	}

	while(alive) {
		printf("1. Load\n2. Run\n3. Trace\n4. Print\n5. Load from stdin\n0. Exit");
		printf("\n> ");
		scanf("%d", &ch);
		//fgets(cmd, 1023, stdin);
	//	scanf("%*[^\n]%*c");
		switch(ch) {
		case 1:

			printf("Enter file name: ");

			inputSFixer();
			fgets(fname, 1023, stdin);

			n = strlen(fname);
			if(fname[n-1]=='\n') fname[n-1]='\0';

	 		if(loadFile(fname)==0) {
				printf("Done.\n");
			} else
				printf("Error while loading.\n");

			alive = 1; 
			break;
		case 2: //run
			printf("Enter address: ");
			scanf("%d", &st);
			run(st, 0);
			break;
		case 3:
			printf("Enter address: ");
			scanf("%d", &st);
			run(st, 1);
			break;
		case 4:
			printf("Enter start and end address: ");
			scanf("%d%d", &st, &end);
			for(i=st;i <= end;i++) {
				printf("%03d %06d\n",i,mem[i]);
			}
			break;
		case 5:
			inputSFixer();
			printf("Enter source:\n");
			fname[0]='\0';
			fp = fopen(TEMP_FILE_NAME,"w");
			fgets(fname,1023,stdin);
			while(strcmp(fname,"\n")!=0) {
				fputs(fname, fp);
				fgets(fname,1023,stdin);
			}
			fclose(fp);
			loadFile(TEMP_FILE_NAME);
			break;
		case 0:
			printf("Exit\n");
			alive = 0;
			break;
		default:
			printf("Invalid command.\n");
			break;
		}
	}
}
