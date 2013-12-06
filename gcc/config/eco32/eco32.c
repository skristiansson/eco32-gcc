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
#include "toplev.h"
#include "ggc.h"
#include "integrate.h"
#include "df.h"
#include "tm_p.h"
#include "target.h"
#include "target-def.h"
#include "tm-constrs.h"

/* The Global `targetm' Variable.  */
static bool eco32_pass_by_reference (CUMULATIVE_ARGS *, enum machine_mode, const_tree, bool);
static bool eco32_return_in_memory (const_tree, const_tree);
static bool eco32_frame_pointer_required(void);
static bool eco32_can_eliminate(int, int);
static bool eco32_must_pass_in_stack (enum machine_mode, const_tree);
static void eco32_setup_incoming_varargs (CUMULATIVE_ARGS *, enum machine_mode, tree, int *, int);


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
  return ((struct machine_function *)
	  ggc_alloc_cleared (sizeof (struct machine_function)));
}

/* The OPTION_OVERRIDE worker.
 All this curently does is set init_machine_status.  */
void
eco32_override_options(void)
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
            fprintf(file, "%s,%ld",
                reg_names[REGNO(XEXP(x, 0))],
                INTVAL(XEXP(plus, 1)));
                //output_addr_const(file,XEXP(plus, 0));
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

  default:
    LOSE_AND_RETURN(
        "invalid operand modifier letter", x);
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

  if (!current_function_is_leaf)
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
	
	/* push args into their respective stack space */
  if(cfun->machine->pretend_size>0)
  {
	temp=cfun->machine->pretend_size-UNITS_PER_WORD;
	for(regno=ECO32_LAST_ARG_REGNO;regno>=ECO32_FIRST_ARG_REGNO;regno--)
	{
		reg = gen_rtx_REG(SImode, regno);
        slot = gen_rtx_PLUS(SImode,stack_pointer_rtx, GEN_INT(temp));
        insn = gen_movsi(gen_rtx_MEM(SImode, slot), reg);
        insn = emit(insn);
        RTX_FRAME_RELATED_P(insn) = 1;
		temp-=UNITS_PER_WORD;
	}
  }
	
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
      RTX_FRAME_RELATED_P(insn) = 1;
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
			RTX_FRAME_RELATED_P(insn) = 1;
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
      RTX_FRAME_RELATED_P(insn) = 1;
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


/* Return non-zero if the function argument described by TYPE is to be
 passed by reference.  */
static bool
eco32_pass_by_reference (CUMULATIVE_ARGS* cum ATTRIBUTE_UNUSED,
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
int
eco32_num_arg_regs (enum machine_mode mode, tree type)
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

/* pushed in function prologue */
void
eco32_setup_incoming_varargs (CUMULATIVE_ARGS *arg_regs_used_so_far,
			     enum machine_mode mode ATTRIBUTE_UNUSED,
			     tree type ATTRIBUTE_UNUSED,
			     int *pretend_size,
			     int second_time ATTRIBUTE_UNUSED)
{
  int size;
  /* All BLKmode values are passed by reference.  */
  gcc_assert (mode != BLKmode);
  
    if (targetm.calls.strict_argument_naming (arg_regs_used_so_far))
    /* If TARGET_STRICT_ARGUMENT_NAMING returns true, then the last named
       arg must not be treated as an anonymous arg.  */
    arg_regs_used_so_far += eco32_num_arg_regs (mode, type);

  size = ECO32_NUM_ARG_REGS - (* arg_regs_used_so_far);
  if (size <= 0)
    return;

  * pretend_size = (size * UNITS_PER_WORD);
}

#include "gt-eco32.h"
