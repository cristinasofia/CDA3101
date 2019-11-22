/* Assembler code fragment for LC3101 in C */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <map> // for symbol table
#include <utility> // make_pair
#include <iostream>
using std::string;

#define MAXLINELENGTH 1000
#define MAXINT 32767

// global
int32_t address;
std::map<string, std::pair<int32_t,string> > symtab; // label, (address,content)

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int secondPass(FILE *, char *, char *, char *, char *, char *, FILE *);

int isNumber(char *);
bool isComment(char *);

int isUndefined(char *);
void isDuplicate(char *);
void isOverflow(int);
void isUnrecognized(char *);
int32_t setOpcode(char *);
void isValid(char *, char *, char *, char *);

int main(int argc, char *argv[])
{
  char *inFileString, *outFileString;
  FILE *inFilePtr, *outFilePtr;
  char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
          arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

  if (argc != 3)
  {
    printf("error: usage: %s <assembly-code-file> <machine-code-file>\n", argv[0]);
    exit(1);
  }

  inFileString = argv[1];
  outFileString = argv[2];

  inFilePtr = fopen(inFileString, "r");
  if (inFilePtr == NULL)
  {
      printf("error in opening %s\n", inFileString);
      exit(1);
  }
  outFilePtr = fopen(outFileString, "w");
  if (outFilePtr == NULL)
  {
      printf("error in opening %s\n", outFileString);
      exit(1);
  }

  address = 0;
  /* use readAndParse to read a line from inFilePtr */
  while(1)
  {
    if(! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2))
    {
      /* reached end of file */
      break;
    }
    address++;
  }

  /* this is how to rewind the file ptr so that you start reading from the
      beginning of the file */
  rewind(inFilePtr);

  address = 0;
  /* use second pass */
  while(1)
  {
    if(! secondPass(inFilePtr, label, opcode, arg0, arg1, arg2, outFilePtr))
    {
      /* reached end of file */
      break;
    }
    address++;
  }

  return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
  char line[MAXLINELENGTH];
  char *ptr = line;

  /* delete prior values */
  label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

  /* read the line from the assembly-language file */
  if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL)
  {
    /* reached end of file */
    return(0);
  }

  /* check for line too long (by looking for a \n) */
  if (strchr(line, '\n') == NULL)
  {
    /* line too long */
    printf("error: line too long\n");
    exit(1);
  }

  /* is there a label? */
  ptr = line;
  if (sscanf(ptr, "%[^\t\n ]", label))
  {
    /* successfully read label; advance pointer over the label */
      ptr += strlen(label);

    /* check if label is already in symbol table */
      isDuplicate(label);

    /* insert label as key into symbol table */
      symtab[label];
  }

  /*
   * Parse the rest of the line.  Would be nice to have real regular
   * expressions, but scanf will suffice.
   */
  sscanf(ptr, "%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]",
      opcode, arg0, arg1, arg2);

  /* syntax check: if format of type is correct */
  isValid(opcode, arg0, arg1, arg2);

  /* if label AND .fill, enter values for symtab map */
  if(strcmp(opcode,".fill") == 0)
  {
    /* else add to symbol table */
    symtab[label] = std::make_pair(address,arg0);
  }
  else
  {
    /* store address only */
    symtab[label].first = address;

    /* check if opcode is unrecognized */
    isUnrecognized(opcode);
  }

  return(1);
}

int secondPass(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2, FILE *outFilePtr)
{
  char line[MAXLINELENGTH];
  char *ptr = line;

  /* delete prior values */
  label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

  /* read the line from the assembly-language file */
  if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL)
  {
    /* reached end of file */
    return(0);
  }

  /* check for line too long (by looking for a \n) */
  if (strchr(line, '\n') == NULL)
  {
    /* line too long */
    printf("error: line too long\n");
    exit(1);
  }

  /* is there a label? */
  ptr = line;
  if (sscanf(ptr, "%[^\t\n ]", label))
  {
    /* successfully read label; advance pointer over the label */
      ptr += strlen(label);
  }

  /*
   * Parse the rest of the line.  Would be nice to have real regular
   * expressions, but scanf will suffice.
   */
  sscanf(ptr, "%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]",
      opcode, arg0, arg1, arg2);

  int32_t instruction = 0;
  int32_t op, regA, regB, offset;
  int32_t offsetField = 0x0000FFFF;

  /* check if .fill, then final output is only content */
  if(strcmp(opcode,".fill") == 0)
  {
    if(isNumber(arg0) == 1)
    {
      instruction = atoi(arg0);
    }
    else
    {
      /* check if arg0 is undefined */
      instruction = isUndefined(arg0);
    }
  }
  else
  {
    op = setOpcode(opcode);
    regA = atoi(arg0);
    regB = atoi(arg1);

    instruction &= ~(7 << 22);
    instruction |= (op << 22);

    instruction &= ~(7 << 19);
    instruction |= (regA << 19);

    instruction &= ~(7 << 16);
    instruction |= (regB << 16);

    // Determine offset
    if(op == 6 || op == 7) // if halt or noop
    {
      offset = 0;
    }
    else if(isNumber(arg2) == 1) // if numeric value
    {
      offset = atoi(arg2);
    }
    else // if symbolic addressss
    {
      //offset = symtab[arg2].first;
      offset = isUndefined(arg2);
      if(op == 4) // BEQ instruction
      {
        offset -= address;
        offset--;
      }
    }

    // detect if offset is range
    isOverflow(offset);

    // determine offsetField
    if(op == 0 || op == 1 || op == 5) // if R-type, bits 0 - 2
    {
      offsetField = (7 << 0);
    }

    offset &= offsetField;
    instruction |= offset;

  }
  /* write to file */
  fprintf(outFilePtr, "%d\n", instruction);

  return(1);
}

int isNumber(char *string)
{
  /* return 1 if string is a number */
  int i;
  return( (sscanf(string, "%d", &i)) == 1);
}

bool isComment(char *string)
{
  char *c = strchr(string,'#');
  return((c - string + 1) >= 1 ? 1 : 0);
}

/* Error checking: Use of undefined labels */
int isUndefined(char *string)
{
  if(symtab.find(string) != symtab.end())
  {
    // found; label has address
    return symtab[string].first;
  }
  else
  {
    printf("error: undefined label\n");
    exit(1);
  }
}

/* Error checking: Use of duplicate labels */
void isDuplicate(char *string)
{
  if(symtab.find(string) != symtab.end())
  {
    printf("error: duplicate label\n");
    exit(1);
  }
}

/* Error checking: offsetFields that don't fit in 16 bits */
void isOverflow(int32_t val)
{
  if (val & ((unsigned)MAXINT + 1))
    val = ~val + 1;

  int bit_mask = 1 << 16; // check bit 16
  int range = val & bit_mask;

  if(range)
  {
    printf("error: offsetfield overflow\n");
    exit(1);
  }
}

/* Error checking: Use of unrecognized opcodes */
void isUnrecognized(char *string)
{
  bool flag;
  flag = ( (strcmp(string,"add") == 0 || strcmp(string,"nand") == 0
      || strcmp(string,"lw") == 0 || strcmp(string,"sw") == 0
      || strcmp(string,"beq") == 0 || strcmp(string,"cmov") == 0
      || strcmp(string,"halt") == 0 || strcmp(string,"noop") == 0) ? 1 : 0 );

  if(!flag)
  {
    printf("error: opcode is unrecognized\n");
    exit(1);
  }
}

/* finds opcode of mneumonic, else returns null */
int32_t setOpcode(char *string)
{
  if(strcmp(string,"add") == 0)
    return 0;
  else if(strcmp(string,"nand") == 0)
    return 1;
  else if(strcmp(string,"lw") == 0)
    return 2;
  else if(strcmp(string,"sw") == 0)
    return 3;
  else if(strcmp(string,"beq") == 0)
    return 4;
  else if(strcmp(string,"cmov") == 0)
    return 5;
  else if(strcmp(string,"halt") == 0)
    return 6;
  else if(strcmp(string,"noop") == 0)
    return 7;

  return 0;
}

/* Error checking: Valid entry */
void isValid(char *opstr, char *str0, char *str1, char *str2)
{
  if(strcmp(opstr,"add") == 0 || strcmp(opstr,"nand") == 0 || strcmp(opstr,"cmov") == 0)
  {
    if(isNumber(str0) != 1 || atoi(str0) < 0 || atoi(str0) > 7)
    {
      printf("error: invalid R-type field '%s'\n",str0);
      exit(1);
    }
    if(isNumber(str1) != 1 || atoi(str1) < 0 || atoi(str1) > 7)
    {
      printf("error: invalid R-type field '%s'\n",str1);
      exit(1);
    }
    if(isNumber(str2) != 1 || atoi(str2) < 0 || atoi(str2) > 7)
    {
      printf("error: invalid R-type field '%s'\n",str2);
      exit(1);
    }
  }
  else if(strcmp(opstr,"lw") == 0 || strcmp(opstr,"sw") == 0 || strcmp(opstr,"beq") == 0)
  {
    if(isNumber(str0) != 1 || atoi(str0) < 0 || atoi(str0) > 7)
    {
      printf("error: invalid I-type field '%s'\n",str0);
      exit(1);
    }
    if(isNumber(str1) != 1 || atoi(str1) < 0 || atoi(str1) > 7)
    {
      printf("error: invalid I-type field '%s'\n",str1);
      exit(1);
    }
  }
  else if(strcmp(opstr,"halt") == 0 || strcmp(opstr,"noop") == 0)
  {
    if(isComment(str0) || (str0 && !str0[0]))
    {
      // if comment or null, then syntax is correct
    }
    else
    {
      printf("error: invalid O-type field '%s'\n",str0);
      exit(1);
    }
  }
  else if(strcmp(opstr,".fill") == 0)
  {
    if(isComment(str0) || (str0 && !str0[0]))
    {
      // if comment or null, then syntax is incorrect
      printf("error: invalid .fill syntax '%s'\n",str0);
      exit(1);
    }
  }
}
