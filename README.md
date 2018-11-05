# sysprostuff
<p>This is where I practice coding system programs like simulator and assembler for that simulator. Hopefully this repository will also contain debuggers and compilers in the future.</p>

# Table of contents
[smac0](#smac0) <- simulator<br/>
[assembler0](#assembler0) <- assembler<br/>
[macropreprocessor0](#macropreprocessor) <- macro preprocessor (development not yet started)<br/>

# <a name='assembler0'></a>Assembler
<p>Assembly code is given to the assembler as input, it outputs a file 'out.sm' which can be fed to smac0 to get output of the assembly code.<br>
See the mnemonics in the section below. It allows literals using the syntax ='value'.<br>
Take a look at the sample '.asm' files to understand the language better.<br>
</p>

# <a name='smac0'></a>Simulator
<p>The simulator has a memory of 1000 bytes with address ranging from 000 to 999. A code stored in smac0 can be executed using the run option. A smac0 instruction consists of three parts: opcode, register operand and memory operand.<br>
The registers A,B,C,D are specified by 1,2,3,4 respectively.
The memory operand can be any address of memory.
E.g. the instruction to add the content at address 106 to the A register: 011106
</p>
<table>
  <tr>
    <th><span style="font-weight:bold">Mnemonic</span><br></th>
    <th><span style="font-weight:bold">Opcode</span><br></th>
    <th><span style="font-weight:bold">Explaination</span></th>
  </tr>
  <tr>
    <td>STOP</td>
    <td>00</td>
    <td>Stops execution of program</td>
  </tr>
  <tr>
    <td>ADD<br></td>
    <td>01</td>
    <td>Add the content given by memory operand to the register operand<br></td>
  </tr>
  <tr>
    <td>SUB</td>
    <td>02</td>
    <td>Subtract the content of register operand by the content given by memory operand</td>
  </tr>
  <tr>
    <td>MULT<br></td>
    <td>03</td>
    <td>Multiply the content of register operand by the content given by memory operand</td>
  </tr>
  <tr>
    <td>MOVER</td>
    <td>04</td>
    <td>Copy content given by memory operand to the register operand<br></td>
  </tr>
  <tr>
    <td>MOVEM</td>
    <td>05</td>
    <td>Copy the content at register operand at the address given by memory operand</td>
  </tr>
  <tr>
    <td>COMP<br></td>
    <td>06</td>
    <td>Compare the register operand with the content given by memory operand</td>
  </tr>
  <tr>
    <td>BC</td>
    <td>07</td>
    <td>Conditional/unconditional jump. See code examples for more details.<br></td>
  </tr>
  <tr>
    <td>DIV<br></td>
    <td>08</td>
    <td>Divide the content at register operand by the content given by memory operand</td>
  </tr>
  <tr>
    <td>READ</td>
    <td>09</td>
    <td>Read the user input and store in the address given by memory operand</td>
  </tr>
  <tr>
    <td>PRINT</td>
    <td>10</td>
    <td>Print the value at the address given by memory operand</td>
  </tr>
  <tr>
    <td>DC<br></td>
    <td>-<br></td>
    <td>[*]Used to store a constant value at an address.<br></td>
  </tr>
  <tr>
    <td>DS</td>
    <td>-</td>
    <td>[*]Used to set a memory address recognized by a given symbol.<br></td>
  </tr>
  <tr>
    <td>START<br></td>
    <td>-</td>
    <td>[*]Sets the start address to store the code</td>
  </tr>
  <tr>
    <td>END</td>
    <td>-<br></td>
    <td>[*]End of the code</td>
  </tr>
  <tr>
    <td>ORIGIN</td>
    <td>-</td>
    <td>[*]Sets the address of the next instruction to the given operand&lt;not yet implemented&gt;</td>
  </tr>
  <tr>
    <td>EQU</td>
    <td>-</td>
    <td>[*]Used to set a label<br></td>
  </tr>
  <tr>
    <td>LTORG<br></td>
    <td>-<br></td>
    <td>[*]Used to make a literal pool</td>
  </tr>
</table>
[*] : used only in assembly language programs.<br>
BC does not take a register operand, instead of a register operand it takes a condition : LT, LE, EQ, GT, GE, ANY specified by 1,2,3,4,5,6 respectively.<br>
PRINT and READ do not take a register operand, a 0 is specified instead of one. E.g.: print the content at address 307: 100307

# <a name='macropreprocessor0'></a>Macro preprocessor
This allows the programmer to create macros in the assembly language program. See the example programs to understand the syntax for declaring and using macros.
