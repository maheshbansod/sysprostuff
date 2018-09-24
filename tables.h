
#define SYMN 100
#define MNEMN 17
#define ERRN 100

struct symtab {
	char symbol[100];
	char defined;
	char used;
	int addr;
};


struct errtab {
	int id;
	int lno;
};

char mnem[][10] = {"STOP", "ADD", "SUB", "MULT",
	"MOVER", "MOVEM", "COMP", "BC",
	"DIV", "READ", "PRINT", "DS",
	"DC", "START", "END", "LTORG",
	"ORIGIN"};
char errmsg[][100] = {"Invalid operand. Expected integer.",
					"Invalid operand. Expected quoted integer.",
					"Not an instruction"};


