.global disable_irq
.global restore_irq
.type disable_irq,function
.type restore_irq,function

disable_irq:
   STMDB    SP!,{R1}
   MRS      R0,CPSR
   MOV      R1,R0

   ORR      R0,R0,#0x80   
   MSR      CPSR_c, R0
   MOV      R0,R1
   LDMIA    SP!,{R1}
   BX      lr                              
   MOV      pc,lr                           

restore_irq:
   STMDB    SP!,{R1,R2}
   MRS      R1,CPSR
   AND      R2,R0,#0xC0
   BIC      R1,R1,#0xC0
   ORR      R0,R1,R2
   MSR      CPSR_c,R0
   LDMIA    SP!,{R1,R2}
   BX       lr                              
   MOV      pc,lr                           

