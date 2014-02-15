#ifndef GCC_ECO32_H
#define GCC_ECO32_H

#define is_ECO32_GENERAL_REG(REGNO) ( ((REGNO) >1 && (REGNO) < 30 ) )
#define is_ECO32_REG(REGNO) ( (REGNO) >= 0 && (REGNO) < 32 )

#define ECO32_REG_SIZE 4

#define ECO32_MAX_PARM_REGS 4  

#undef  STARTFILE_SPEC
#define STARTFILE_SPEC "crt0%O%s" 

#undef  ENDFILE_SPEC
#define ENDFILE_SPEC ""

#undef LIB_SPEC
#define LIB_SPEC "%{!g:-lc} %{g:-lg} -leco32"

/* Layout of Source Language Data Types */

#define INT_TYPE_SIZE 32
#define SHORT_TYPE_SIZE 16
#define LONG_TYPE_SIZE 32

#define DEFAULT_SIGNED_CHAR 1

#define eco32_AP    27
#define eco32_FP    28
#define eco32_SP    29
#define eco32_RA	31
#define eco32_R0    2
#define eco32_AUX   1

#define ECO32_FIRST_ARG_REGNO	4
#define ECO32_LAST_ARG_REGNO	7
#define ECO32_NUM_ARG_REGS		4

enum reg_class
{
  NO_REGS, GENERAL_REGS, ALL_REGS, LIM_REG_CLASSES
};

#define REG_CLASS_CONTENTS  \
{  \
{0x00000000},   \
{0xBFFFFFFC},   \
{0xFFFFFFFF}    \
}

#define N_REG_CLASSES LIM_REG_CLASSES

#define REG_CLASS_NAMES {\
    "NO_REGS", \
    "GENERAL_REGS", \
    "ALL_REGS" }

#define FIRST_PSEUDO_REGISTER 32

#define FIXED_REGISTERS {\
1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1}

#define CALL_USED_REGISTERS {\
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1}

#define REGISTER_NAMES {\
"$0", "$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9", "$10",\
"$11", "$12", "$13", "$14", "$15", "$16","$17", "$18", "$19", "$20",\
"$21", "$22", "$23", "$24", "$25", "$26", "?ap", "$28", "$29", "$30",\
"$31"}

/* A C expression that is nonzero if it is permissible to store a
 value of mode MODE in hard register number REGNO (or in several
 registers starting with that one).  All gstore registers are 
 equivalent, so we can set this to 1.  */
#define HARD_REGNO_MODE_OK(R,M) 1

/* A C expression whose value is a register class containing hard
 register REGNO.  */
#define REGNO_REG_CLASS(R) \
eco32_reg_class(R)

/* A C expression for the number of consecutive hard registers,
 starting at register number REGNO, required to hold a value of mode
 MODE.  */
#define HARD_REGNO_NREGS(REGNO, MODE)			   \
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1)		   \
   / UNITS_PER_WORD)

/* A C expression that is nonzero if a value of mode MODE1 is
 accessible in mode MODE2 without copying.  */
#define MODES_TIEABLE_P(MODE1, MODE2) 1

/* no nested function support */
#define TRAMPOLINE_SIZE 0

/* The Overall Framework of an Assembler File */

#undef  ASM_SPEC
#define ASM_COMMENT_START "#"
#define ASM_APP_ON ""
#define ASM_APP_OFF ""

#define FILE_ASM_OP     "\t.file\n"

/* Switch to the text or data segment.  */
#define TEXT_SECTION_ASM_OP  "\t.text"
#define DATA_SECTION_ASM_OP  "\t.data"

/* Output and Generation of Labels */
#define GLOBAL_ASM_OP "\t.global\t"

/* Assembler Commands for Alignment */

#define ASM_OUTPUT_ALIGN(STREAM,POWER) \
	fprintf (STREAM, "\t.p2align %d\n", POWER);

/* A C compound statement to output to stdio stream STREAM the
 assembler syntax for an instruction operand X.  */
#define PRINT_OPERAND(STREAM, X, CODE) \
	eco32_print_operand (STREAM, X, CODE)

#define PRINT_OPERAND_ADDRESS(STREAM ,X) \
	eco32_print_operand_address (STREAM, X)

/* Passing Arguments in Registers */

#define FUNCTION_VALUE_REGNO_P(REGNO) ((REGNO) == eco32_R0)

/* Define how to find the value returned by a function.
   VALTYPE is the data type of the value (as a tree).
   If the precise function being called is known, FUNC is its FUNCTION_DECL;
   otherwise, FUNC is 0.  */
#define FUNCTION_VALUE(VALTYPE, FUNC) gen_rtx_REG (TYPE_MODE (VALTYPE), eco32_R0)

/* Define how to find the value returned by a library function
   assuming the value has mode MODE.  */
#define LIBCALL_VALUE(MODE) gen_rtx_REG (MODE, eco32_R0)

/* A C type for declaring a variable that is used as the first
 argument of `FUNCTION_ARG' and other related values.  */
#define CUMULATIVE_ARGS int

/* If defined, the maximum amount of space required 
 for outgoing arguments will be computed and placed into the variable
 `current_function_outgoing_args_size'.  No space will be pushed
 onto the stack for each call; instead, the function prologue should
 increase the stack frame size by this amount.  */
#define ACCUMULATE_OUTGOING_ARGS 1

/* Return the maximum number of consecutive registers
   needed to represent mode MODE in a register of class CLASS.  */
#define CLASS_MAX_NREGS(CLASS, MODE) \
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)
  
/* Given an rtx X being reloaded into a reg required to be
   in class CLASS, return the class of reg to actually use.
   In general this is just CLASS; but on some machines
   in some cases it is preferable to use a more restrictive class.  */
#define PREFERRED_RELOAD_CLASS(X,CLASS) (CLASS)


/* A C statement (sans semicolon) for initializing the variable CUM
 for the state at the beginning of the argument list.  
 For eco32, the first arg is passed in register $4 .  */
#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,FNDECL,N_NAMED_ARGS) \
  (CUM = 0)

/* STACK AND CALLING */

/* Define this macro if pushing a word onto the stack moves the stack
 pointer to a smaller address.  */
#define STACK_GROWS_DOWNWARD 1

/* Offset from the frame pointer to the first local variable slot to
 be allocated.  */
#define STARTING_FRAME_OFFSET 0

/* Offset from the SP to the first outgoing argument*/
#define STACK_POINTER_OFFSET 0

/* Define this if the above stack space is to be considered part of the
 space allocated by the caller.  */
#define OUTGOING_REG_PARM_STACK_SPACE(FNTYPE) 1
/*#define STACK_PARMS_IN_REG_PARM_AREA*/

/* Define this if it is the responsibility of the caller to allocate
 the area reserved for arguments passed in registers.  */
#define REG_PARM_STACK_SPACE(FNDECL) (4 * UNITS_PER_WORD)

/* Offset from the argument pointer register to the first argument's
 address.  On some machines it may depend on the data type of the
 function.  */
#define FIRST_PARM_OFFSET(F) 0

/* Define this macro to nonzero value if the addresses of local 
   variable slots are at negative offsets from the frame pointer.  */
#define FRAME_GROWS_DOWNWARD 0

/* Define this macro as a C expression that is nonzero for registers
  that are used by the epilogue or the return pattern.  
  The stack and frame pointer registers are already assumed to be used
  as needed.  */
#define EPILOGUE_USES(R) (R == RETURN_ADDRESS_REGNUM )

/* A C expression whose value is RTL representing the location of the
 incoming return address at the beginning of any function, before
 the prologue. */
#define INCOMING_RETURN_ADDR_RTX gen_rtx_REG (SImode, eco32_RA)

/* A C expression whose value is RTL representing the value of the
   return address for the frame COUNT steps up from the current
   frame, after the prologue.  FRAMEADDR is the frame pointer of the
   COUNT frame, or the frame pointer of the COUNT - 1 frame if
   `RETURN_ADDR_IN_PREVIOUS_FRAME' is defined.

   The value of the expression must always be the correct address when
   COUNT is zero, but may be `NULL_RTX' if there is not way to
   determine the return address of other frames. */
#define RETURN_ADDR_RTX eco32_return_addr_rtx

/* Describe how we implement __builtin_eh_return.
#define EH_RETURN_DATA_REGNO(N)	((N) < 4 ? (N+2) : INVALID_REGNUM)  */

/* Store the return handler into the call frame.
#define EH_RETURN_HANDLER_RTX						\
  gen_frame_mem (Pmode,							\
		 plus_constant (frame_pointer_rtx, UNITS_PER_WORD))  */

/* Storage Layout */
#define BITS_PER_UNIT 8
#define BITS_BIG_ENDIAN 0
#define BYTES_BIG_ENDIAN 1
#define WORDS_BIG_ENDIAN 1

/* Alignment required for a function entry point, in bits.  */
#define FUNCTION_BOUNDARY 32

/* Define this macro as a C expression which is nonzero if accessing
 less than a word of memory (i.e. a `char' or a `short') is no
 faster than accessing a word of memory.  */
#define SLOW_BYTE_ACCESS 1

/* Number of storage units in a word; normally the size of a
 general-purpose register, a power of two from 1 or 8.  */
#define UNITS_PER_WORD 4

/* Define this macro to the minimum alignment enforced by hardware
 for the stack pointer on this machine.  The definition is a C
 expression for the desired alignment (measured in bits).  */
#define STACK_BOUNDARY 32

/* Normal alignment required for function parameters on the stack, in
 bits.  All stack parameters receive at least this much alignment
 regardless of data type.  */
#define PARM_BOUNDARY 32

/* Alignment of field after `int : 0' in a structure.  */
#define EMPTY_FIELD_BOUNDARY  32

/* No data type wants to be aligned rounder than this.  */
#define BIGGEST_ALIGNMENT 32

/* The best alignment to use in cases where we have a choice.  */
#define FASTEST_ALIGNMENT 32

/* Every structures size must be a multiple of 8 bits.  */
#define STRUCTURE_SIZE_BOUNDARY 8

/* Look at the fundamental type that is used for a bit-field and use 
 that to impose alignment on the enclosing structure.
 struct s {int a:8}; should have same alignment as "int", not "char".*/
#define	PCC_BITFIELD_TYPE_MATTERS	1

/* Largest integer machine mode for structures.  
 If undefined, the default is GET_MODE_SIZE(DImode).  */
#define MAX_FIXED_MODE_SIZE 32

/* Make strings word-aligned so strcpy from constants will be faster.*/
#define CONSTANT_ALIGNMENT(EXP, ALIGN)  \
  ((TREE_CODE (EXP) == STRING_CST	\
    && (ALIGN) < FASTEST_ALIGNMENT)	\
   ? FASTEST_ALIGNMENT : (ALIGN))

/* Make arrays of chars word-aligned for the same reasons.  */
#define DATA_ALIGNMENT(TYPE, ALIGN)		\
  (TREE_CODE (TYPE) == ARRAY_TYPE		\
   && TYPE_MODE (TREE_TYPE (TYPE)) == QImode	\
   && (ALIGN) < FASTEST_ALIGNMENT ? FASTEST_ALIGNMENT : (ALIGN))

/* Set this nonzero if move instructions will actually fail to work
 when given unaligned data.  */
#define STRICT_ALIGNMENT 1

/* Generating Code for Profiling */
#define FUNCTION_PROFILER(FILE,LABELNO) (abort (), 0)

/* An alias for the machine mode for pointers.  */
#define Pmode         SImode

/* An alias for the machine mode used for memory references to
 functions being called, in `call' RTL expressions.  */
#define FUNCTION_MODE SImode

/* The register number of the stack pointer register, which must also
 be a fixed register according to `FIXED_REGISTERS'.  */
#define STACK_POINTER_REGNUM eco32_SP

/* The register number of the frame pointer register, which is used to
 access automatic variables in the stack frame.  */
#define FRAME_POINTER_REGNUM eco32_FP
/*#define HARD_FRAME_POINTER_REGNUM eco32_RFP*/

#define INITIAL_FRAME_POINTER_OFFSET(DEPTH) (DEPTH) = 0

/* The register number of the arg pointer register, which is used to
 access the function's argument list.  */
#define ARG_POINTER_REGNUM eco32_AP

/* The register number of the return pointer register, which is used to
 access the function's return point. */
#define RETURN_ADDRESS_REGNUM eco32_RA

#define ELIMINABLE_REGS				\
 {{ARG_POINTER_REGNUM, FRAME_POINTER_REGNUM},	\
  {FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM}}

 
/* This macro is similar to `INITIAL_FRAME_POINTER_OFFSET'.  It
 specifies the initial difference between the specified pair of
 registers.  This macro must be defined if `ELIMINABLE_REGS' is
 defined.  */
#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)			\
  do {									\
    (OFFSET) = eco32_initial_elimination_offset ((FROM), (TO));		\
  } while (0)

/* A C expression that is nonzero if REGNO is the number of a hard
 register in which function arguments are sometimes passed.  */
#define FUNCTION_ARG_REGNO_P(r) (r >= ECO32_FIRST_ARG_REGNO && \
r <= ECO32_LAST_ARG_REGNO)

/* A macro whose definition is the name of the class to which a valid
 base register must belong.  A base register is one used in an
 address which is the register value plus a displacement.  */
#define BASE_REG_CLASS GENERAL_REGS

#define INDEX_REG_CLASS NO_REGS

/* is register NUM save to be used for a base of an address */
#define HARD_REGNO_OK_FOR_BASE_P(NUM) \
  ((unsigned) (NUM) < FIRST_PSEUDO_REGISTER \
   && (REGNO_REG_CLASS(NUM) == GENERAL_REGS \
       || (NUM) == 0))

/* A C expression which is nonzero if register number NUM is suitable
 for use as a base register in operand addresses.  */
#ifdef REG_OK_STRICT
#define REGNO_OK_FOR_BASE_P(NUM)		 \
  (HARD_REGNO_OK_FOR_BASE_P(NUM) 		 \
   || HARD_REGNO_OK_FOR_BASE_P(reg_renumber[(NUM)]))
#else
#define REGNO_OK_FOR_BASE_P(NUM)		 \
  ((NUM) >= FIRST_PSEUDO_REGISTER || HARD_REGNO_OK_FOR_BASE_P(NUM))
#endif

/* A C expression which is nonzero if register number NUM is suitable
 for use as an index register in operand addresses.  */
#define REGNO_OK_FOR_INDEX_P(NUM) 0

/* The maximum number of bytes that a single instruction can move
 quickly between memory and registers or between two memory
 locations.  */
#define MOVE_MAX 4
/*A C expression which is nonzero if on this machine it is safe to "convert" 
an integer of inprec bits to one of outprec bits (where outprec is smaller 
than inprec) by merely operating on it as if it had only outprec bits. */
#define TRULY_NOOP_TRUNCATION(op,ip)1

/* All load operations sign extend.  */
#define LOAD_EXTEND_OP(MEM) SIGN_EXTEND

/* A number, the maximum number of registers that can appear in a
 valid memory address.  */
#define MAX_REGS_PER_ADDRESS 1

/* An alias for a machine mode name.  This is the machine mode that
 elements of a jump-table should have.  */
#define CASE_VECTOR_MODE SImode

/* A C compound statement with a conditional `goto LABEL;' executed
 if X (an RTX) is a legitimate memory address on the target machine
 for a memory operand of mode MODE.  */
#define GO_IF_LEGITIMATE_ADDRESS(MODE,X,LABEL)		\
  do {                                                  \
    if (GET_CODE(X) == PLUS)				\
      {							\
	rtx op1,op2;					\
	op1 = XEXP(X,0);				\
	op2 = XEXP(X,1);				\
	if (GET_CODE(op1) == REG			\
	    && CONSTANT_ADDRESS_P(op2)			\
	    && REGNO_OK_FOR_BASE_P(REGNO(op1)))		\
	  goto LABEL;					\
      }							\
    if (REG_P (X) && REGNO_OK_FOR_BASE_P (REGNO (X)))	\
      goto LABEL;					\
    if (GET_CODE (X) == SYMBOL_REF			\
	|| GET_CODE (X) == LABEL_REF			\
	|| GET_CODE (X) == CONST)			\
      goto LABEL;					\
  } while (0)

/* Run-time Target Specification */

#define TARGET_CPU_CPP_BUILTINS() \
  { \
    builtin_define ("__ECO32__");  \
  }

/*#define CC1_SPEC "-fno-inline"*/

#endif /* GCC_ECO32_H */
