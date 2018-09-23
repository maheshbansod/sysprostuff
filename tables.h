
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
