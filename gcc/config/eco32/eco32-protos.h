extern void  eco32_expand_prologue (void);
extern void  eco32_expand_epilogue (void);
extern int   eco32_initial_elimination_offset (int, int);
extern void  eco32_print_operand (FILE *, rtx, int);
extern void  eco32_print_operand_address (FILE *, rtx);
extern enum reg_class eco32_reg_class(int);
extern rtx eco32_return_addr_rtx (int, rtx);
