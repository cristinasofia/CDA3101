// Cristina S. Alonso
// Project 2: Simulator

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXLINELENGTH 1000
#define NUMMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 8 /* number of machine registers */

#define ADD 0
#define NAND 1
#define LW 2
#define SW 3
#define BEQ 4
#define CMOV 5
#define HALT 6
#define NOOP 7

#define NOOPINSTRUCTION 0x1c00000

typedef struct IFIDStruct {
    int instr;
    int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
    int instr;
    int pcPlus1;
    int readRegA;
    int readRegB;
    int offset;
} IDEXType;

typedef struct EXMEMStruct {
    int instr;
    int branchTarget;
    int aluResult;
    int readRegB;
} EXMEMType;

typedef struct MEMWBStruct {
    int instr;
    int writeData;
} MEMWBType;

typedef struct WBENDStruct {
    int instr;
    int writeData;
} WBENDType;

typedef struct stateStruct {
    int pc;
    int instrMem[NUMMEMORY];
    int dataMem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
    IFIDType IFID;
    IDEXType IDEX;
    EXMEMType EXMEM;
    MEMWBType MEMWB;
    WBENDType WBEND;
    int cycles; /* number of cycles run so far */
} stateType;

int field0(int instruction)
{
	return( (instruction>>19) & 0x7);
}

int field1(int instruction)
{
	return( (instruction>>16) & 0x7);
}

int field2(int instruction)
{
	return(instruction & 0xFFFF);
}

int opcode(int instruction)
{
	return(instruction>>22);
}

void printInstruction(int instr)
{
	char opcodeString[10];
	if (opcode(instr) == ADD) {
	strcpy(opcodeString, "add");
	} else if (opcode(instr) == NAND) {
	strcpy(opcodeString, "nand");
	} else if (opcode(instr) == LW) {
	strcpy(opcodeString, "lw");
	} else if (opcode(instr) == SW) {
	strcpy(opcodeString, "sw");
	} else if (opcode(instr) == BEQ) {
	strcpy(opcodeString, "beq");
	} else if (opcode(instr) == CMOV) {
	strcpy(opcodeString, "cmov");
	} else if (opcode(instr) == HALT) {
	strcpy(opcodeString, "halt");
	} else if (opcode(instr) == NOOP) {
	strcpy(opcodeString, "noop");
	} else {
	strcpy(opcodeString, "data");
	}

	printf(	"%s %d %d %d\n", opcodeString, field0(instr), field1(instr),
		field2(instr));
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate before cycle %d starts\n", statePtr->cycles);
    printf("\tpc %d\n", statePtr->pc);

    printf("\tdata memory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tdataMem[ %d ] %d\n", i, statePtr->dataMem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("\tIFID:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IFID.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IFID.pcPlus1);
    printf("\tIDEX:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IDEX.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IDEX.pcPlus1);
	printf("\t\treadRegA %d\n", statePtr->IDEX.readRegA);
	printf("\t\treadRegB %d\n", statePtr->IDEX.readRegB);
	printf("\t\toffset %d\n", statePtr->IDEX.offset);
    printf("\tEXMEM:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->EXMEM.instr);
	printf("\t\tbranchTarget %d\n", statePtr->EXMEM.branchTarget);
	printf("\t\taluResult %d\n", statePtr->EXMEM.aluResult);
	printf("\t\treadRegB %d\n", statePtr->EXMEM.readRegB);
    printf("\tMEMWB:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->MEMWB.instr);
	printf("\t\twriteData %d\n", statePtr->MEMWB.writeData);
    printf("\tWBEND:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->WBEND.instr);
	printf("\t\twriteData %d\n", statePtr->WBEND.writeData);
}

int main(int argc, const char *argv[])
{
	// At the start of the program, initialize the pc and all registers to zero.
	// Initialize the instruction field in all pipeline registers to the
	// noop instruction (0x1c00000).
	stateType state, newState;
	state.pc = 0;
	state.cycles = 0;
	for( int i = 0; i < NUMREGS; i++ ) {
		state.reg[i] = 0;
	}

	state.IFID.instr = NOOPINSTRUCTION;
	state.IDEX.instr = NOOPINSTRUCTION;
	state.EXMEM.instr = NOOPINSTRUCTION;
	state.MEMWB.instr = NOOPINSTRUCTION;
	state.WBEND.instr = NOOPINSTRUCTION;

	// Open file
	FILE *infile = fopen(argv[1],"r");
	char line[MAXLINELENGTH];

	if( infile == NULL ) {
		fprintf(stderr, "Error: Cannot open file %s\n", argv[1]);
		exit(1);
	}

	state.numMemory = 0;
	while(fgets(line, MAXLINELENGTH, infile))
	{
		if(sscanf( line, "%d", &state.instrMem[state.numMemory]))
		{
			state.dataMem[state.numMemory] = state.instrMem[state.numMemory];
			printf( "memory[%d]=%d\n", state.numMemory, state.instrMem[state.numMemory] );
			state.numMemory++;
		}
	}

	// Print instruction
	printf( "%d memory words\n", state.numMemory );
	printf( "\tinstruction memory:\n" );
	for(int i = 0; i < state.numMemory; i++)
	{
		printf( "\t\tinstrMem[ %d ] ", i);
		printInstruction(state.instrMem[i]);
	}

	// Begin cycles
	while(1)
	{
		printState(&state);

		// check for halt
		if(opcode(state.MEMWB.instr) == HALT) {
			printf("machine halted\n");
			printf("total of %d cycles executed\n", state.cycles);
			exit(0);
		}

		newState = state;
		newState.cycles++;

		// Each test case may execute at most 100 cycles
		if(state.cycles > 100)
		{
			exit(0);
		}

		/* --------------------- IF stage --------------------- */

		newState.IFID.instr = state.instrMem[state.pc];
		newState.IFID.pcPlus1 = state.pc + 1;
		newState.pc = state.pc + 1;

		/* --------------------- ID stage --------------------- */

		newState.IDEX.instr = state.IFID.instr;
		newState.IDEX.pcPlus1 = state.IFID.pcPlus1;

		// Load registers
		newState.IDEX.readRegA = state.reg[field0(state.IFID.instr)];
		newState.IDEX.readRegB = state.reg[field1(state.IFID.instr)];
		newState.IDEX.offset = field2(state.IFID.instr);

		// Convert 16-bit number
		if(newState.IDEX.offset & (1 << 15))
		{
			newState.IDEX.offset -= (1 << 16);
		}

		// You will need to stall for one type of data hazard: a lw followed by an
		// instruction that uses the register being loaded.  Implement
		// this stall as shown in the textbook.

		// STALL REQUIRED FOR LW
		if( opcode(state.IDEX.instr) == LW &&
			( (field1(state.IDEX.instr) == field0(state.IFID.instr)) ||
			(field1(state.IDEX.instr) == field1(state.IFID.instr)) ) )
			{
				newState.IFID = state.IFID; // delayed to next clock cycle
				newState.IDEX.instr = NOOPINSTRUCTION; // becomes NOOP
				newState.pc = state.pc;
			}


		/* --------------------- EX stage --------------------- */

		// Use data forwarding to resolve most data hazards.  I.e. the ALU should
		// be able to take its inputs from any pipeline register (instead of just
		// the IDEX register).  There is no need for forwarding within the
		// register file.  For this case of forwarding, you'll instead forward
		// data from the WBEND pipeline register.
		// Remember to take the most recent data (e.g. data in the EXMEM register
		// gets priority over data in the MEMWB register).
		// ONLY FORWARD DATA TO THE EX STAGE.

		int writeA = 0;
    int writeB = 0;
		int dataA = state.IDEX.readRegA;
		int dataB = state.IDEX.readRegB;

		// FORWARDING: EX HAZARD
		if( opcode(state.EXMEM.instr) == ADD ||
				opcode(state.EXMEM.instr) == NAND ||
				opcode(state.EXMEM.instr) == CMOV )
		{
			// 1a Data Hazard
			if( field2(state.EXMEM.instr) != 0 && writeA != 1
					&& field2(state.EXMEM.instr) == field0(state.IDEX.instr) )
			{
				dataA = state.EXMEM.aluResult;
        writeA = 1;
			}
			// 1b Data Hazard
			if( field2(state.EXMEM.instr) != 0 && writeB != 1
					&& field2(state.EXMEM.instr) == field1(state.IDEX.instr) )
			{
				dataB = state.EXMEM.aluResult;
        writeB = 1;
			}
		}

		// FORWARDING: MEM HAZARD
		if( opcode(state.MEMWB.instr) == ADD ||
				opcode(state.MEMWB.instr) == NAND ||
				opcode(state.MEMWB.instr) == CMOV )
		{
			// 2a Data Hazard
			if( field2(state.MEMWB.instr) != 0  && writeA != 1
				&& field2(state.MEMWB.instr) == field0(state.IDEX.instr) )
			{
				dataA = state.MEMWB.writeData;
        writeA = 1;
			}
			// 2b Data Hazard
			if( field2(state.MEMWB.instr) != 0 && writeB != 1
				&& field2(state.MEMWB.instr) == field1(state.IDEX.instr) )
			{
				dataB = state.MEMWB.writeData;
        writeB = 1;
			}
		}
    else if(opcode(state.MEMWB.instr) == LW)
    {
      // When 1 instruction inbetween LW and register in need of value
      if( field1(state.MEMWB.instr) != 0 && writeA != 1
        && field1(state.MEMWB.instr) == field0(state.IDEX.instr) )
      {
        dataA = state.MEMWB.writeData;
        writeA = 1;
      }
      if( field1(state.MEMWB.instr) != 0 && writeB != 1
        && field1(state.MEMWB.instr) == field1(state.IDEX.instr) )
      {
        dataB = state.MEMWB.writeData;
        writeB = 1;
      }
    }

    // FORWARDING: WB HAZARD
		if( opcode(state.WBEND.instr) == ADD ||
				opcode(state.WBEND.instr) == NAND ||
				opcode(state.WBEND.instr) == CMOV )
		{
			// 2a Data Hazard
			if( field2(state.WBEND.instr) != 0 && writeA != 1
				&& field2(state.WBEND.instr) == field0(state.IDEX.instr) )
			{
				dataA = state.WBEND.writeData;
        writeA = 1;
			}
			// 2b Data Hazard
			if( field2(state.WBEND.instr) != 0 && writeB != 1
				&& field2(state.WBEND.instr) == field1(state.IDEX.instr) )
			{
				dataB = state.WBEND.writeData;
        writeB = 1;
			}
		}
    else if(opcode(state.WBEND.instr) == LW)
    {
      // When 2 instructions inbetween LW and register in need of value
      if( field1(state.WBEND.instr) != 0 && writeA != 1
        && field1(state.WBEND.instr) == field0(state.IDEX.instr) )
      {
        dataA = state.WBEND.writeData;
        writeA = 1;
      }
      if( field1(state.WBEND.instr) != 0 && writeB != 1
        && field1(state.WBEND.instr) == field1(state.IDEX.instr) )
      {
        dataB = state.WBEND.writeData;
        writeB = 1;
      }
    }

		newState.EXMEM.instr = state.IDEX.instr;
		newState.EXMEM.readRegB = dataB;

		// Calculate branch target address
		newState.EXMEM.branchTarget = state.IDEX.pcPlus1 + state.IDEX.offset;

		switch(opcode(state.IDEX.instr))
		{
			case ADD:
				newState.EXMEM.aluResult = dataA + dataB;
				break;
			case NAND:
				newState.EXMEM.aluResult = ~(dataA & dataB);
				break;
			case CMOV:
				if( dataB != 0 )
				{
					// Condition for move
					newState.EXMEM.aluResult = dataA;
				}
				else
				{
					// If condition isn't met, then nothing happens
					newState.EXMEM.instr = NOOPINSTRUCTION;
				}
				break;
			case LW:
				newState.EXMEM.aluResult = dataA + state.IDEX.offset;
				break;
			case SW:
				newState.EXMEM.aluResult = dataA + state.IDEX.offset;
				break;
			case BEQ:
				newState.EXMEM.aluResult = (dataA == dataB);
				break;
			case HALT:
				// do nothing
				break;
			case NOOP:
				// do nothing
				break;
		}

		/* --------------------- MEM stage --------------------- */

		newState.MEMWB.instr = state.EXMEM.instr;

		switch(opcode(state.EXMEM.instr))
		{
			case ADD:
				newState.MEMWB.writeData = state.EXMEM.aluResult;
				break;
			case NAND:
				newState.MEMWB.writeData = state.EXMEM.aluResult;
				break;
			case CMOV:
				newState.MEMWB.writeData = state.EXMEM.aluResult;
				break;
			case LW:
				newState.MEMWB.writeData = state.dataMem[state.EXMEM.aluResult];
				break;
			case SW:
				newState.dataMem[state.EXMEM.aluResult] = state.EXMEM.readRegB;
				break;
			case BEQ:
				// does not go through MEM stage: do nothing
				break;
			case HALT:
				// do nothing
				break;
			case NOOP:
				// do nothing
				break;
		}

		// Use branch-not-taken to resolve most branch hazards, and decide whether or not
		// to branch in the MEM stage.  This requires you to discard instructions if it
		// turns out that the branch really was taken.  To discard instructions, change
		// the relevant instructions in the pipeline to the noop instruction (0x1c00000).

		// CONTROL HAZARD
		if(opcode(state.EXMEM.instr) == BEQ && state.EXMEM.aluResult == 1)
			{
				newState.IFID.instr = NOOPINSTRUCTION; // flush
				newState.IDEX.instr = NOOPINSTRUCTION; // flush
				newState.EXMEM.instr = NOOPINSTRUCTION; // flush
				newState.pc = state.EXMEM.branchTarget;
			}

		/* --------------------- WB stage --------------------- */

		newState.WBEND.instr = state.MEMWB.instr;
		newState.WBEND.writeData = state.MEMWB.writeData;

		switch(opcode(state.MEMWB.instr))
		{
			case ADD:
			{
				newState.reg[field2(state.MEMWB.instr)] = state.MEMWB.writeData;
				break;
			}
			case NAND:
			{
				newState.reg[field2(state.MEMWB.instr)] = state.MEMWB.writeData;
				break;
			}
			case CMOV:
			{
				newState.reg[field2(state.MEMWB.instr)] = state.MEMWB.writeData;
				break;
			}
			case LW:
			{
				newState.reg[field1(state.MEMWB.instr)] = state.MEMWB.writeData;
				break;
			}
			case SW:
				// does not go through WB stage: do nothing
				break;
			case BEQ:
				// does not go through WB stage: do nothing
				break;
			case HALT:
				// do nothing
				break;
			case NOOP:
				// do nothing
				break;
		}


		state = newState; /* this is the last statement before end of the loop.
				    It marks the end of the cycle and updates the
				    current state with the values calculated in this
				    cycle */
	}
}
