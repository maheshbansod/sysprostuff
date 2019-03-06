
#define LITN 100
#define POOLN 100
#define SYMN 100
#define MNEMN 17
#define ERRN 100
#define ICN 700
#define REGN 4
#define CCN 6

struct littab {
	int value;
	int address;
};

struct pooltab {
	int index; //index of first literal of pool
	int size; //number of literals in the pool
};

struct symtab {
	char symbol[100];
	char defined; //bool
	char used; //bool
	int addr;
};


struct errtab {
	int id;
	int lno;
};


struct ictab {
	int addr;
	char opclass; //i,a,d
	int opcode;
	int regop;
	char optype; //memop type: c, s, l
	int opvalue;
};

char mnem[][10] = {"STOP", "ADD", "SUB", "MULT",
	"MOVER", "MOVEM", "COMP", "BC",
	"DIV", "READ", "PRINT", "DC",
	"DS", "START", "END", "ORIGIN",
	"EQU", "LTORG"};

const int ADSTART=13,DLSTART=11, ISSTART=0;

char errmsg[][100] = {"Invalid operand. Expected integer.",
					"Invalid operand. Expected quoted integer.",
					"Not an instruction"};

char reg[][5] = {"AREG","BREG","CREG","DREG"};

char cc[][4] = {"LT","LE","EQ","GT","GE","ANY"};

