#ifdef PSX2_TARGET
#ifdef __cplusplus
extern "C" {
#endif

asm void *ps2memcpy(char *dst, char*src, int size);
asm void *ps2memset(char *dst, unsigned char c, int size);
asm void *ps2memmove(char*dst, char*src, int size);

asm void *ps2memcpy(char *dst, char*src, int size)
{
    add     v0, a0, $0;     nop
    beq     a2, $0, l_end;  sltiu   t1, a2, 8;      
    bne     t1, $0, l_cp1;  pref    0, 16(a1)                     

l_cp8:            
    ldr     t0, 0(a1);      addiu   a2, a2, -8
    ldl     t0, 7(a1);      addiu   a1, a1, 8;      
    sdr     t0, 0(a0);      sltiu   t1, a2, 8;
    sdl     t0, 7(a0);      addiu   a0, a0, 8;
    beq     t1, $0, l_cp8;  pref    0, 16(a1) 
    beq     a2, $0, l_end;  pref    0, 16(a1) 

l_cp1:    
    addiu   a2, a2, -1;     lb      t0, 0(a1)
    addiu   a1, a1, 1;      sb      t0, 0(a0)
    bne     a2, $0, l_cp1;  addiu   a0, a0, 1

l_end:
    jr      ra;             nop    
}

asm void *ps2memset(char *dst, unsigned char c, int size)
{
    add     v0, a0, $0;     nop
    beq     a2, $0, l_send; sltiu   t3, a2, 8;      
    bne     t3, $0, l_set1; sll     t1, a1, 8;      
    or      t0, t1, a1;     sll     t1, t0, 16;     
    or      t2, t0, t1;     dsll32  t1, t2, 0;      
    dsrl32  t2, t1, 0;      or      t0, t1, t2;     
           
l_set8:
    sdr     t0, 0(a0);      addiu   a2, a2, -8
    sltiu   t1, a2, 8;      sdl     t0, 7(a0);      
    beq     t1, $0, l_set8; addiu   a0, a0, 8
    beq     a2, $0, l_send; nop
    
l_set1:    
    addiu   a2, a2, -1;     sb      a1, 0(a0)
    nop;                    nop
    bne     a2, $0, l_set1; addiu   a0, a0, 1

l_send:
    jr      ra;             nop    
}

asm void *ps2memmove(char*dst, char*src, int size)
{
    add     v0, a0, $0;     nop
    beq     a2, $0, l_end;  add     t1, a1, a2;     
    slt     t0, a1, a0;     slt     t1, a0, t1; 
    and     t0, t1, t0;     nop    
    beq     t0, $0, l_cp;   sltiu   t1, a2, 8

    add     a0, a0, a2;     add     a1, a1, a2
    addiu   a0, a0, -1;     addiu   a1, a1, -1;
    bne     t1, $0, l_rcp1; pref    0, 16(a1) 
    addiu   a0, a0, -7;     addiu   a1, a1, -7

l_rcp8:            
    ldr     t0, 0(a1);      addiu   a2, a2, -8
    ldl     t0, 7(a1);      addiu   a1, a1, -8;      
    sdr     t0, 0(a0);      sltiu   t1, a2, 8;
    sdl     t0, 7(a0);      addiu   a0, a0, -8;
    beq     t1, $0, l_rcp8; pref    0, 16(a1) 
    beq     a2, $0, l_rend; pref    0, 16(a1) 
    addiu   a0, a0, 7;      addiu   a1, a1, 7

l_rcp1:    
    addiu   a2, a2, -1;     lb      t0, 0(a1)
    addiu   a1, a1, -1;     sb      t0, 0(a0)
    bne     a2, $0, l_rcp1; addiu   a0, a0, -1

l_rend:    
    jr      ra;             nop    
    
l_cp:
    bne     t1, $0, l_cp1;  pref    0, 16(a1)                     

l_cp8:            
    ldr     t0, 0(a1);      addiu   a2, a2, -8
    ldl     t0, 7(a1);      addiu   a1, a1, 8;      
    sdr     t0, 0(a0);      sltiu   t1, a2, 8;
    sdl     t0, 7(a0);      addiu   a0, a0, 8;
    beq     t1, $0, l_cp8;  pref    0, 16(a1) 
    beq     a2, $0, l_end;  pref    0, 16(a1) 

l_cp1:    
    addiu   a2, a2, -1;     lb      t0, 0(a1)
    addiu   a1, a1, 1;      sb      t0, 0(a0)
    bne     a2, $0, l_cp1;  addiu   a0, a0, 1

l_end:
    jr      ra;             nop    
}


#ifdef __cplusplus
}
#endif

#endif