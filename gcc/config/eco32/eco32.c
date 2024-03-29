#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
#include "rtl.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "real.h"
#include "insn-config.h"
#include "conditions.h"
#include "output.h"
#include "insn-attr.h"
#include "flags.h"
#include "expr.h"
#include "function.h"
#include "recog.h"
#include "diagnostic-core.h"
#include "toplev.h"
#include "ggc.h"
#include "df.h"
#include "tm_p.h"
#include "target.h"
#include "target-def.h"
#include "tm-constrs.h"

/* The Global `targetm' Variable.  */
static bool eco32_pass_by_reference (cumulative_args_t, enum machine_mode,
				     const_tree, bool);
static bool eco32_return_in_memory (const_tree, const_tree);
static bool eco32_frame_pointer_required(void);
static bool eco32_can_eliminate(int, int);
static bool eco32_must_pass_in_stack (enum machine_mode, const_tree);
static void eco32_setup_incoming_varargs (cumulative_args_t, enum machine_mode,
					  tree, int *, int);
static void eco32_option_override (void);
static rtx eco32_function_arg (cumulative_args_t, enum machine_mode, const_tree,
			       bool);
static void eco32_function_arg_advance (cumulative_args_t, enum machine_mode,
					const_tree, bool);
static bool eco32_legitimate_constant_p (enum machine_mode, rtx);
static int eco32_return_pops_args (tree, tree, int);
static int eco32_num_arg_regs (enum machine_mode, const_tree);
static int eco32_arg_partial_bytes (cumulative_args_t, enum machine_mode, tree,
				    bool);

/* Initialize the GCC target structure.  */

#undef  TARGET_PROMOTE_PROTOTYPES
#define TARGET_PROMOTE_PROTOTYPES	hook_bool_const_tree_true

#undef  TARGET_RETURN_IN_MEMORY
#define TARGET_RETURN_IN_MEMORY		eco32_return_in_memory

#undef  TARGET_MUST_PASS_IN_STACK
#define TARGET_MUST_PASS_IN_STACK	eco32_must_pass_in_stack

#undef  TARGET_PASS_BY_REFERENCE
#define TARGET_PASS_BY_REFERENCE        eco32_pass_by_reference

#undef TARGET_FRAME_POINTER_REQUIRED
#define TARGET_FRAME_POINTER_REQUIRED eco32_frame_pointer_required

#undef TARGET_CAN_ELIMINATE
#define TARGET_CAN_ELIMINATE eco32_can_eliminate

#undef TARGET_SETUP_INCOMING_VARARGS
#define TARGET_SETUP_INCOMING_VARARGS eco32_setup_incoming_varargs

#undef TARGET_OPTION_OVERRIDE
#define TARGET_OPTION_OVERRIDE eco32_option_override

#undef TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG eco32_function_arg

#undef TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE eco32_function_arg_advance

#undef TARGET_LEGITIMATE_CONSTANT_P
#define TARGET_LEGITIMATE_CONSTANT_P eco32_legitimate_constant_p

#undef TARGET_RETURN_POPS_ARGS
#define TARGET_RETURN_POPS_ARGS eco32_return_pops_args

#undef TARGET_ARG_PARTIAL_BYTES
#define TARGET_ARG_PARTIAL_BYTES eco32_arg_partial_bytes

struct gcc_target targetm = TARGET_INITIALIZER;

/*#include "gt-eco32.h"*/

enum reg_class
eco32_reg_class(int);

#define LOSE_AND_RETURN(msgid, x)		\
  do						\
    {						\
      eco32_operand_lossage (msgid, x);		\
      return;					\
    } while (0)

/* Per-function machine data.  */
struct GTY(()) machine_function
  {
	/* number of pretented arguments for varargs */  
	int pretend_size;
	
    /* Number of bytes saved on the stack for local variables.  */
    int local_vars_size;

    /* number of bytes saved on stack for register save area */
    int saved_reg_size;
    int save_ret;

    int sp_fp_offset;
    bool fp_needed;
    int size_for_adjusting_sp;

  };

/* Allocate a chunk of memory for per-function machine-dependent data.  */

static struct machine_function *
eco32_init_machine_status (void)
{
  return ggc_alloc_cleared_machine_function ();
}

/* The TARGET_OPTION_OVERRIDE worker.
 All this curently does is set init_machine_status.  */
void
eco32_option_override(void)
{
  init_machine_status = eco32_init_machine_status;
}


enum reg_class
eco32_reg_class(int regno)
{
  if (is_ECO32_GENERAL_REG(regno))
    {
      return GENERAL_REGS;
    }
  if (is_ECO32_REG(regno))
    {
      return ALL_REGS;
    }
  return NO_REGS;
}

/* Worker function for TARGET_RETURN_IN_MEMORY.  */

static bool
eco32_return_in_memory (const_tree type,
    const_tree fntype ATTRIBUTE_UNUSED)
  {
    const HOST_WIDE_INT size = int_size_in_bytes (type);
    return (size == -1 || size > UNITS_PER_WORD);
  }

/* Emit an error message when we're in an asm, and a fatal error for
 "normal" insns.  Formatted output isn't easily implemented, since we
 use output_operand_lossage to output the actual message and handle the
 categorization of the error.  */

static void
eco32_operand_lossage(const char *msgid, rtx op)
{
  debug_rtx(op);
  output_operand_lossage("%s", msgid);
}

/* On the ECO32 the first ECO32_MAX_PARM_REGS args are normally in registers
   and the rest are pushed. */
static rtx
eco32_function_arg (cumulative_args_t cum_v, enum machine_mode mode,
		    const_tree type, bool named)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);

  if (targetm.calls.must_pass_in_stack (mode, type))
    return NULL_RTX;

  if (!named || (*cum >= ECO32_NUM_ARG_REGS))
    return NULL_RTX;

  return gen_rtx_REG (mode, *cum + ECO32_FIRST_ARG_REGNO);
}

/* Update the data in CUM to advance over an argument
   of mode MODE and data type TYPE.
   (TYPE is null for libcalls where that information may not be available.)  */
static void
eco32_function_arg_advance (cumulative_args_t cum, enum machine_mode mode,
			    const_tree type, bool named ATTRIBUTE_UNUSED)
{
  *get_cumulative_args (cum) += eco32_num_arg_regs (mode, type);
}

/* The PRINT_OPERAND_ADDRESS worker.  */

void
eco32_print_operand_address(FILE *file, rtx x)
{
  switch (GET_CODE(x))
    {
  case REG:
    fprintf(file, "%s,0", reg_names[REGNO(x)]);
    break;

  case PLUS:
    switch (GET_CODE(XEXP(x, 1)))
      {
    case CONST_INT:
      fprintf(file, "%s,%ld",
          reg_names[REGNO(XEXP(x, 0))],
          INTVAL(XEXP(x, 1)));
      break;
    case SYMBOL_REF:
          fprintf(file, "%s,",
          reg_names[REGNO(XEXP(x, 0))]);
		output_addr_const(file, XEXP(x, 1));
      break;
    case CONST:
      {
        rtx plus = XEXP(XEXP(x, 1), 0);
        if (GET_CODE(XEXP(plus, 0)) == SYMBOL_REF
            && CONST_INT_P(XEXP(plus, 1)))
          {
	    fprintf (file, "%s,", reg_names[REGNO (XEXP (x, 0))]);
	    output_addr_const (file, plus);
          }
        else
          abort();
      }
      break;
    default:
      abort();
      }
    break;

  default:
    output_addr_const(file, x);
    break;
    }
}

/* The PRINT_OPERAND worker.  */

void
eco32_print_operand(FILE *file, rtx x, int code)
{
  rtx operand = x;

  /* New code entries should just be added to the switch below.  If
   handling is finished, just return.  If handling was just a
   modification of the operand, the modified operand should be put in
   "operand", and then do a break to let default handling
   (zero-modifier) output the operand.  */
  switch (code)
    {
    case 0:
      /* No code, print as usual.  */
      break;

    case 'H':
      if (GET_CODE (x) == REG)
	{
	  fprintf (file, "%s", reg_names[REGNO (x) + 1]);
	  return;
	}
      /* fall-through */
    default:
      LOSE_AND_RETURN ("invalid operand modifier letter", x);
    }

  /* Print an operand as without a modifier letter.  */
  switch (GET_CODE(operand))
    {
  case REG:
    if (REGNO(operand) >= FIRST_PSEUDO_REGISTER)
      internal_error(
          "internal error: bad register: %d",
          REGNO(operand));
    fprintf(file, "%s",
        reg_names[REGNO(operand)]);
    return;

  case MEM:
    PRINT_OPERAND_ADDRESS(file,XEXP(operand, 0));
    return;

  default:
    /* No need to handle all strange variants, let output_addr_const
     do it for us.  */
    if (CONSTANT_P(operand))
      {
        output_addr_const(file, operand);
        return;
      }

    LOSE_AND_RETURN("unexpected operand", x);
    }
}

/* Compute the size of the local area and the size to be adjusted by 
 the prologue and epilogue.  */

static void
eco32_compute_frame(void)
{

  int regno;
  int args_size;
  int pretend_args_size;
  
  args_size=(ACCUMULATE_OUTGOING_ARGS ? crtl->outgoing_args_size : 0);
  pretend_args_size=crtl->args.pretend_args_size;
  
  if(pretend_args_size>0)
  {
	/*args_size+=pretend_args_size;*/
	cfun->machine->pretend_size = pretend_args_size;
  }
  
  cfun->machine->fp_needed = FALSE;
	
  if(eco32_frame_pointer_required())
	cfun->machine->fp_needed = TRUE;

  cfun->machine->local_vars_size =
      get_frame_size();

  cfun->machine->saved_reg_size = 0;

  /* Save callee-saved registers.  */
  for (regno = 0; regno < FIRST_PSEUDO_REGISTER;
      regno++)
    if (df_regs_ever_live_p(regno)
        && (!call_used_regs[regno]))
      cfun->machine->saved_reg_size += ECO32_REG_SIZE;

  if (!crtl->is_leaf)
    {
      cfun->machine->save_ret = 1;
    }

  cfun->machine->sp_fp_offset = args_size
      + cfun->machine->saved_reg_size
      + (cfun->machine->save_ret ? ECO32_REG_SIZE : 0);

  cfun->machine->size_for_adjusting_sp = cfun->machine->local_vars_size
          + cfun->machine->saved_reg_size
          + (cfun->machine->save_ret ? ECO32_REG_SIZE : 0)
          + args_size;
}

void
eco32_expand_prologue(void)
{
  int regno, temp = 0;
  rtx insn, reg, slot;

  eco32_compute_frame();

  /* adjust sp size */
  if (cfun->machine->size_for_adjusting_sp > 0)
    {
      insn =
          emit_insn(
              gen_subsi3(stack_pointer_rtx,
                  stack_pointer_rtx,
                  GEN_INT(
                      cfun->machine->size_for_adjusting_sp)));
      RTX_FRAME_RELATED_P(insn) = 1;
      temp = 0;
    }

  /* skip outgoing args*/
  temp += (
      ACCUMULATE_OUTGOING_ARGS ?
          crtl->outgoing_args_size : 0);

  /* Save callee-saved registers.*/
  for (regno = 0; regno < FIRST_PSEUDO_REGISTER;
      regno++)
    {
      if (!fixed_regs[regno]
          && df_regs_ever_live_p(regno)
          && !call_used_regs[regno])
        {
          reg = gen_rtx_REG(SImode, regno);
          slot = gen_rtx_PLUS(SImode,
              stack_pointer_rtx, GEN_INT(temp));

          insn = gen_movsi(
              gen_rtx_MEM(SImode, slot), reg);
          insn = emit(insn);
          RTX_FRAME_RELATED_P(insn) = 1;
          temp += 4;
        }
    }

  if (cfun->machine->save_ret)
    {
      reg = gen_rtx_REG(SImode,
          RETURN_ADDRESS_REGNUM);
      slot = gen_rtx_PLUS(SImode,
          stack_pointer_rtx, GEN_INT(temp));

      insn = gen_movsi(gen_rtx_MEM(SImode, slot),
          reg);
      insn = emit(insn);
      RTX_FRAME_RELATED_P(insn) = 1;
      temp += 4;
    }
    
    /*set fp to sp + sp_fp_offset*/
    temp=cfun->machine->sp_fp_offset;
    insn=gen_addsi3(frame_pointer_rtx,stack_pointer_rtx,GEN_INT(temp));
    insn = emit(insn);
    RTX_FRAME_RELATED_P(insn) = 1;
}

void
eco32_expand_epilogue(void)
{
  int regno, temp=0;
  rtx reg, insn, slot;
  temp = cfun->machine->size_for_adjusting_sp-cfun->machine->local_vars_size;

  if (cfun->machine->save_ret)
    {
      temp -= 4;
      reg = gen_rtx_REG(SImode,
          RETURN_ADDRESS_REGNUM);
      slot = gen_rtx_PLUS(SImode,
          stack_pointer_rtx, GEN_INT(temp));

      insn = gen_movsi(reg,
          gen_rtx_MEM(SImode, slot));
      insn = emit(insn);
    }

  if (cfun->machine->saved_reg_size != 0)
    {
    for (regno = FIRST_PSEUDO_REGISTER;regno-- > 0;)
	{
        if (!fixed_regs[regno]
            && !call_used_regs[regno]
            && df_regs_ever_live_p(regno))
        {
			temp -= 4;
            reg = gen_rtx_REG(SImode, regno);
            slot = gen_rtx_PLUS(SImode,
                stack_pointer_rtx, GEN_INT(temp));

			insn = gen_movsi(reg,
			gen_rtx_MEM(SImode, slot));
			insn = emit(insn);
		}
	}
    }	
    	
  if (cfun->machine->size_for_adjusting_sp > 0)
    {
      insn =
          emit_insn(
              gen_addsi3(stack_pointer_rtx,
                  stack_pointer_rtx,
                  GEN_INT(
                      cfun->machine->size_for_adjusting_sp)));
    }
  emit_jump_insn (gen_returner());
  }

  /* Implements the macro INITIAL_ELIMINATION_OFFSET, 
   return the OFFSET.  */
int
eco32_initial_elimination_offset(int from, int to)
{
		int ret=0;
	eco32_compute_frame();
  if ((from) == FRAME_POINTER_REGNUM
      && (to) == STACK_POINTER_REGNUM)
    {
      ret = cfun->machine->sp_fp_offset;
    }

  else if ((from) == ARG_POINTER_REGNUM
      && (to) == FRAME_POINTER_REGNUM)
    {
      ret = cfun->machine->local_vars_size;
    }
   else
   {
	abort();
	}
	
	return ret;
}

static int
eco32_arg_partial_bytes (cumulative_args_t cum_v, enum machine_mode mode,
			tree type, bool named ATTRIBUTE_UNUSED)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);

  int words;
  unsigned int size = eco32_num_arg_regs (mode, type);

  if (*cum >= ECO32_MAX_PARM_REGS)
    words = 0;
  else if ((*cum + size) > ECO32_MAX_PARM_REGS)
    words = (*cum + size) - ECO32_MAX_PARM_REGS;
  else
    words = 0;

  return words * UNITS_PER_WORD;
}

/* Implement RETURN_ADDR_RTX (COUNT, FRAMEADDR).

   We currently only support calculating the return address for the
   current frame. */
rtx
eco32_return_addr_rtx (int count, rtx frame ATTRIBUTE_UNUSED)
{
  if (count)
    return NULL_RTX;

  eco32_compute_frame ();

  if (cfun->machine->save_ret)
    return gen_rtx_MEM (Pmode, plus_constant (Pmode, arg_pointer_rtx,
					      -UNITS_PER_WORD));

  return get_hard_reg_initial_val (Pmode, RETURN_ADDRESS_REGNUM);
}

/* Return non-zero if the function argument described by TYPE is to be
 passed by reference.  */
static bool
eco32_pass_by_reference (cumulative_args_t cum ATTRIBUTE_UNUSED,
			 enum machine_mode mode, const_tree type,
			 bool named ATTRIBUTE_UNUSED)
{
unsigned HOST_WIDE_INT size;

if (type)
  {
    if (AGGREGATE_TYPE_P(type))
    return TRUE;
    size = int_size_in_bytes (type);
  }
else
size = GET_MODE_SIZE (mode);

return size > 32; /* > 32bit */
}


bool
eco32_frame_pointer_required(void)
{
return 1;
}

static bool
eco32_can_eliminate(int from ATTRIBUTE_UNUSED, int to ATTRIBUTE_UNUSED)
{
	return 1;
}

static bool
eco32_must_pass_in_stack (enum machine_mode mode, const_tree type)
{
  if (mode == BLKmode)
    return true;
  if (type == NULL)
    return false;
  return AGGREGATE_TYPE_P (type);
}


/* Compute the number of word sized registers needed to hold a
   function argument of mode INT_MODE and tree type TYPE.  */
static int
eco32_num_arg_regs (enum machine_mode mode, const_tree type)
{
  int size;

  if (targetm.calls.must_pass_in_stack (mode, type))
    return 0;

  if (type && mode == BLKmode)
    size = int_size_in_bytes (type);
  else
    size = GET_MODE_SIZE (mode);

  return (size + UNITS_PER_WORD - 1) / UNITS_PER_WORD;
}

/* Setup incoming varargs by copying the anonymous argument register into
   their respective spot on the stack */
static void
eco32_setup_incoming_varargs (cumulative_args_t cum_v,
			      enum machine_mode mode, tree type,
			      int *pretend_size, int no_rtl)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);
  int first_anon_arg;

  if (no_rtl)
    return;

  /* All BLKmode values are passed by reference.  */
  gcc_assert (mode != BLKmode);

  if (targetm.calls.strict_argument_naming (cum_v))
    /* If TARGET_STRICT_ARGUMENT_NAMING returns true, then the last named
       arg must not be treated as an anonymous arg.  */
    *cum += eco32_num_arg_regs (mode, type);

  first_anon_arg = *cum + ECO32_FIRST_ARG_REGNO;

  if (first_anon_arg < (ECO32_FIRST_ARG_REGNO + ECO32_NUM_ARG_REGS))
    {
      int size = ECO32_FIRST_ARG_REGNO + ECO32_NUM_ARG_REGS - first_anon_arg;
      rtx regblock;
      int offset = (first_anon_arg - ECO32_FIRST_ARG_REGNO) * UNITS_PER_WORD;

      regblock = gen_rtx_MEM (BLKmode,
			      plus_constant (Pmode, arg_pointer_rtx, offset));
      move_block_from_reg (first_anon_arg, regblock, size);

      *pretend_size = size * UNITS_PER_WORD;
    }
}

static bool
eco32_legitimate_constant_p (enum machine_mode mode ATTRIBUTE_UNUSED,
			     rtx x ATTRIBUTE_UNUSED)
{
  return true;
}

static int
eco32_return_pops_args (tree fundecl ATTRIBUTE_UNUSED,
			tree funtype ATTRIBUTE_UNUSED,
			int size ATTRIBUTE_UNUSED)
{
  return 0;
}

#include "gt-eco32.h"
