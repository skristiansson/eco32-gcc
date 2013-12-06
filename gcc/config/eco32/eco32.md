(define_mode_iterator MMODE [QI HI SI])

;; ---------------------------------------------------------------------
;; Constraints
;; ---------------------------------------------------------------------

(define_constraint "A"
  "An absolute address."
  (and (match_code "mem")
       (ior (match_test "GET_CODE(XEXP (op, 0)) == SYMBOL_REF")
	    (match_test "GET_CODE (XEXP (op, 0)) == LABEL_REF")
	    (match_test "GET_CODE (XEXP (op, 0)) == CONST"))))

(define_constraint "B"
  "An offset address."
  (and (match_code "mem")
       (match_test "GET_CODE (XEXP (op, 0)) == PLUS")))

(define_constraint "W"
  "A register indirect memory operand."
  (and (match_code "mem")
       (match_test "REG_P (XEXP (op, 0))
       && REGNO_OK_FOR_BASE_P (REGNO (XEXP (op, 0)))")))

(define_constraint "O"
  "The constant zero"
  (and (match_code "const_int")
       (match_test "ival == 0")))

;; ---------------------------------------------------------------------
;; Predicates
;; ---------------------------------------------------------------------

;; Nonzero if OP can be source of a simple move operation.
(define_predicate "eco32_movsrc"
  (match_code "mem,const_int,reg,subreg,symbol_ref,label_ref,const")
{
  /* Any (MEM LABEL_REF) is OK.  That is a pc-relative load.  */
  if (MEM_P (op) && GET_CODE (XEXP (op, 0)) == LABEL_REF)
    return 1;

  if (MEM_P (op)
      && GET_CODE (XEXP (op, 0)) == PLUS
      && GET_CODE (XEXP (XEXP (op, 0), 0)) == REG
      && GET_CODE (XEXP (XEXP (op, 0), 1)) == CONST)
    return 1;

  return general_operand (op, mode);
})

;; ---------------------------------------------------------------------
;; nop instruction
;; ---------------------------------------------------------------------

(define_insn "nop"
  [(const_int 0)]
  ""
  "")

;; ---------------------------------------------------------------------
;; Arithmetic instructions
;; ---------------------------------------------------------------------
(define_insn "addsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (plus:SI
	   (match_operand:SI 1 "general_operand" "r,r")
	   (match_operand:SI 2 "nonmemory_operand" "r,i")))]
  ""
  "@
  add  %0,%1,%2
  addi  %0,%1,%2")

(define_insn "subsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (minus:SI
	   (match_operand:SI 1 "register_operand" "r,r")
	   (match_operand:SI 2 "nonmemory_operand" "r,i")))]
  ""
  "@
  sub  %0,%1,%2
  subi  %0,%1,%2")
  
(define_insn "mulsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (mult:SI
	   (sign_extend:SI (match_operand:SI 1 "register_operand" "r,r"))
	   (sign_extend:SI (match_operand:SI 2 "nonmemory_operand" "r,i"))))]
  ""
  "@
  mul  %0,%1,%2
  muli  %0,%1,%2")
  
(define_insn "divsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (div:SI
	   (match_operand:SI 1 "register_operand" "r,r")
	   (match_operand:SI 2 "nonmemory_operand" "r,i")))]
  ""
  "@
  div  %0,%1,%2
  divi  %0,%1,%2")
  
(define_insn "udivsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (udiv:SI
	   (match_operand:SI 1 "register_operand" "r,r")
	   (match_operand:SI 2 "nonmemory_operand" "r,i")))]
  ""
  "@
  divu  %0,%1,%2
  divui  %0,%1,%2")
  
 (define_insn "modsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (mod:SI
	   (match_operand:SI 1 "register_operand" "r,r")
	   (match_operand:SI 2 "nonmemory_operand" "r,i")))]
  ""
  "@
  rem  %0,%1,%2
  remi  %0,%1,%2")
  
(define_insn "umodsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (umod:SI
	   (match_operand:SI 1 "register_operand" "r,r")
	   (match_operand:SI 2 "nonmemory_operand" "r,i")))]
  ""
  "@
  remu  %0,%1,%2
  remui  %0,%1,%2")

;; ---------------------------------------------------------------------
;; Logic instructions
;; ---------------------------------------------------------------------

(define_insn "andsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (and:SI
	   (match_operand:SI 1 "register_operand" "r,r")
	   (match_operand:SI 2 "nonmemory_operand" "r,i")))]
  ""
  "@
  and  %0,%1,%2
  andi  %0,%1,%2")
  
 (define_insn "iorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (ior:SI
	   (match_operand:SI 1 "register_operand" "r,r")
	   (match_operand:SI 2 "nonmemory_operand" "r,i")))]
  ""
  "@
  or  %0,%1,%2
  ori  %0,%1,%2")
  
(define_insn "xorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (xor:SI
	   (match_operand:SI 1 "register_operand" "r,r")
	   (match_operand:SI 2 "nonmemory_operand" "r,i")))]
  ""
  "@
  xor  %0,%1,%2
  xori  %0,%1,%2")
  
(define_insn "xnorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (not:SI(xor:SI
	   (match_operand:SI 1 "register_operand" "r,r")
	   (match_operand:SI 2 "nonmemory_operand" "r,i"))))]
  ""
  "@
  xnor  %0,%1,%2
  xnori  %0,%1,%2")
  
  (define_insn "one_cmpl<mode>2"
  [(set (match_operand:MMODE 0 "register_operand" "=r")
	(not:MMODE (match_operand:MMODE 1 "register_operand" "r")))]
  ""
  "xnori  %0,%1,0")
  
;; ---------------------------------------------------------------------
;; Shift instructions
;; ---------------------------------------------------------------------

; logical

(define_insn "usashlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (us_ashift:SI
	   (match_operand:SI 1 "register_operand" "r,r")
	   (match_operand:SI 2 "nonmemory_operand" "r,i")))]
  ""
  "@
  sll  %0,%1,%2
  slli  %0,%1,%2")

(define_insn "ashlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (ashift:SI
	   (match_operand:SI 1 "register_operand" "r,r")
	   (match_operand:SI 2 "nonmemory_operand" "r,i")))]
  ""
  "@
  sll  %0,%1,%2
  slli  %0,%1,%2")
  
(define_insn "lshrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (lshiftrt:SI
	   (match_operand:SI 1 "register_operand" "r,r")
	   (match_operand:SI 2 "nonmemory_operand" "r,i")))]
  ""
  "@
  slr  %0,%1,%2
  slri  %0,%1,%2")
  
; arithmetical

(define_insn "ashrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (ashiftrt:SI
	   (match_operand:SI 1 "register_operand" "r,r")
	   (match_operand:SI 2 "nonmemory_operand" "r,i")))]
  ""
  "@
  sar  %0,%1,%2
  sari  %0,%1,%2")

;; ---------------------------------------------------------------------
;; Branches
;; ---------------------------------------------------------------------

(define_expand "cbranchsi4"
  [(set (pc)
	(if_then_else (match_operator 0 "ordered_comparison_operator"
		       [(match_operand:SI 1 "register_operand" "")
			(match_operand:SI 2 "nonmemory_operand" "")])
		      (label_ref (match_operand 3 "" ""))
		      (pc)))]
  ""
  {
  /* Force the compare operands into registers.  */
  if (GET_CODE (operands[1]) != REG)
	operands[1] = force_reg (SImode, operands[1]);
  if (GET_CODE (operands[2]) != REG)
	operands[2] = force_reg (SImode, operands[2]);
 })
 
(define_insn "*cbranchsi4"
  [(set (pc)
	(if_then_else (match_operator 0 "ordered_comparison_operator"
		       [(match_operand:SI 1 "register_operand" "r")
			(match_operand:SI 2 "register_operand" "r")])
		      (label_ref (match_operand 3 "" ""))
		      (pc)))]
  ""
  {
  
  switch(GET_CODE(operands[0]))
  {
	case EQ:
		return "beq  %1,%2,%l3";
		
	case NE:
		return "bne  %1,%2,%l3";
		
	case GT:
		return "bgt  %1,%2,%l3";
	case GTU:
		return "bgtu %1,%2,%l3";
			
	case LT:
		return "blt  %1,%2,%l3";
	case LTU:
		return "bltu %1,%2,%l3";
		
	case GE:
		return "bge  %1,%2,%l3";
	case GEU:
		return "bgeu %1,%2,%l3";
		
	case LE:
		return "ble  %1,%2,%l3";
	case LEU:
		return "bleu %1,%2,%l3";
		
	default:
		gcc_unreachable ();
  }
	return "";
  })


;; ---------------------------------------------------------------------
;; Move instructions
;; ---------------------------------------------------------------------
;; this builds all kind of moves

(define_expand "movsi"
   [(set (match_operand:SI 0 "general_operand" "")
 	(match_operand:SI 1 "general_operand" ""))]
   ""
  "
{
  /* If this is a store, force the value into a register.  */
  if (! (reload_in_progress || reload_completed))
  {
    if (MEM_P (operands[0]))
    {
      operands[1] = force_reg (SImode, operands[1]);
      if (MEM_P (XEXP (operands[0], 0)))
        operands[0] = gen_rtx_MEM (SImode, force_reg (SImode, XEXP (operands[0], 0)));
    }
    else 
      if (MEM_P (operands[1])
          && MEM_P (XEXP (operands[1], 0)))
        operands[1] = gen_rtx_MEM (SImode, force_reg (SImode, XEXP (operands[1], 0)));
  }
}")
		
(define_insn "*movsi"
  [(set (match_operand:SI 0 "general_operand" "=r,r,r,W,A,r,r,B,r")
	(match_operand:SI 1 "eco32_movsrc" "O,r,i,r,r,W,A,r,B"))]
  "register_operand (operands[0], SImode)
   || register_operand (operands[1], SImode)"
  "@
   add  %0,$0,$%1
   add  %0,$0,%1
   addi  %0,$0,%1
   stw  %1,%0
   stw  %1,$0,%0
   ldw  %0,%1
   ldw  %0,$0,%1
   stw  %1,%0
   ldw  %0,%1"
)

(define_expand "movqi"
  [(set (match_operand:QI 0 "general_operand" "")
	(match_operand:QI 1 "general_operand" ""))]
  ""
  "
{
  /* If this is a store, force the value into a register.  */
  if (MEM_P (operands[0]))
    operands[1] = force_reg (QImode, operands[1]);
}")

(define_insn "*movqi"
  [(set (match_operand:QI 0 "general_operand" "=r,r,r,W,A,r,r,B,r")
	(match_operand:QI 1 "eco32_movsrc" "          O,r,i,r,r,W,A,r,B"))]
  "register_operand (operands[0], QImode)
   || register_operand (operands[1], QImode)"
  "@
   add  %0,$0,$%1
   add  %0,$0,%1
   addi  %0,$0,%1
   stb  %1,%0
   stb %1,$0,%0
   ldb  %0,%1
   ldb  %0,$0,%1
   stb  %1,%0
   ldb  %0,%1"
)

(define_expand "movhi"
  [(set (match_operand:HI 0 "general_operand" "")
	(match_operand:HI 1 "general_operand" ""))]
  ""
  "
{
  /* If this is a store, force the value into a register.  */
  if (MEM_P (operands[0]))
    operands[1] = force_reg (HImode, operands[1]);
}")

(define_insn "*movhi"
  [(set (match_operand:HI 0 "general_operand" "=r,r,r,W,A,r,r,B,r")
	(match_operand:HI 1 "eco32_movsrc" "O,r,i,r,r,W,A,r,B"))]
  "register_operand (operands[0], HImode)
   || register_operand (operands[1], HImode)"
  "@
   add  %0,$0,$%1
   add  %0,$0,%1
   addi  %0,$0,%1
   sth  %1,%0
   sth %1,$0,%0
   ldh  %0,%1
   ldh  %0,$0,%1
   sth  %1,%0
   ldh  %0,%1"
)

;; ---------------------------------------------------------------------
;; Call and Jump instructions
;; ---------------------------------------------------------------------

(define_expand "call"
  [(call (match_operand:SI 0 "memory_operand" "")
		(match_operand 1 "general_operand" ""))]
  ""
{
  gcc_assert (MEM_P (operands[0]));
})

(define_insn "*call"
  [(call (mem:SI (match_operand:SI
		  0 "nonmemory_operand" "i,r"))
	 (match_operand 1 "" ""))]
  ""
  "@
   jal   %0
   jalr  %0"
)

(define_expand "call_value"
  [(set (match_operand 0 "" "")
		(call (match_operand:SI 1 "memory_operand" "")
		 (match_operand 2 "" "")))]
  ""
{
  gcc_assert (MEM_P (operands[1]));
})

(define_insn "jump"
  [(set (pc)
	(label_ref (match_operand 0 "" "")))]
  ""
  "j  %l0"
)

(define_insn "indirect_jump"
  [(set (pc) (match_operand:SI 0 "nonimmediate_operand" "r"))]
  ""
  "jr  %0")


(define_insn "*call_value"
  [(set (match_operand 0 "register_operand" "=r")
	(call (mem:SI (match_operand:SI
		       1 "immediate_operand" "i"))
	      (match_operand 2 "" "")))]
  ""
  "jal  %1"
)

(define_insn "*call_value_indirect"
  [(set (match_operand 0 "register_operand" "=r")
	(call (mem:SI (match_operand:SI
		       1 "register_operand" "r"))
	      (match_operand 2 "" "")))]
  ""
  "jalr %1")


;; ---------------------------------------------------------------------
;; Prologue & Epilogue
;; ---------------------------------------------------------------------

(define_expand "prologue"
  [(clobber (const_int 0))]
  ""
  "
{
  eco32_expand_prologue ();
  DONE;
}
")

(define_expand "epilogue"
  [(return)]
  ""
  "
{
  eco32_expand_epilogue ();
  DONE;
}
")

(define_insn "returner"
  [(return)]
  "reload_completed"
  "jr  $31")
