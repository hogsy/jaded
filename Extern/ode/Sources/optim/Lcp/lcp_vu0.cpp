#include <libvu0.h>
#include <libdma.h>
#include <eestruct.h>
#include <math.h>

#include "lcpmath.h"
#include "lcp_vu0.h"
//#include "system.h"

//#include "debugvariables.h"

#pragma inline_depth(3)

#define ZERO(v)         ((v)[0] = (v)[1] = (v)[2] = (v)[3] = 0.0f)
#define SET(v,x,y,z,w)  ((v)[0] = (x),(v)[1] = (y),(v)[2] = (z),(v)[3] = (w))
#define DIFFERENT(v1,v2)((v1)[0] != (v2)[0] || (v1)[1] != (v2)[1] || (v1)[2] != (v2)[2] || (v1)[3] != (v2)[3])
#define DIFF(v1,v2) ((fabsf (v2[0] - v1[0]) > 0.000001f) || \
                     (fabsf (v2[1] - v1[1]) > 0.000001f) || \
                     (fabsf (v2[2] - v1[2]) > 0.000001f) || \
                     (fabsf (v2[3] - v1[3]) > 0.000001f))

extern u_long128 dSolveL1Program __attribute__((section (".vudata")));

static int bCodeLoaded = 0;
static sceDmaChan *dmaVif0;
static int ellAddr = 0;
static int exAddr = 0xC0;

//__________________________________________________________________________

 void CopyAlignedQWords (u_long128 *pSrc, u_long128 *pDst, u_int uiNbQWords)
{
    while (uiNbQWords--)
        *pDst++ = *pSrc++;
}

//__________________________________________________________________________

 void TransferdSolveL1ProgramToVu0 ()
{
    dmaVif0 = sceDmaGetChan (SCE_DMA_VIF0);
    dmaVif0->chcr.TTE = 1;
    
    sceDmaSend (dmaVif0, &dSolveL1Program);
}

// LoadDataAndActivateMicroProgram_dSolveL1_________________________________________________________________________

 void LoadDataAndActivateMicroProgram_dSolveL1 (const float *ell0, 
                                               const float *ell1,
                                               const float *ell2,
                                               const float *ell3,
                                               float *ex, 
                                               u_int _loop)
{
    // sending information about where and how data are stored
    asm __volatile__ (" 
    
        ctc2    %0, $vi01
        ctc2    %1, $vi02
        ctc2    %2, $vi03
    ": : "r" ((_loop >> 2) - 1),
         "r" (ellAddr), 
         "r" (exAddr));
                 
    // Test if data is bigger than vu0 memory (4KB)
    // TODO: moverlo a la llamada de la funcion y en caso de q no quepa en 
    // memoria, hacerlo de manera normal...
    if ((_loop + (_loop >> 2)) > 256)  // 256 = 4K
        error ("Data size is bigger than 4KB");

        
    #ifdef PS2_DEBUG
        d_fNbQWords = (float)(_loop + (_loop >> 2));
    #endif
    
    // Create a new DMA packet
    u_long *pPkt  = (u_long *)(0x70000000);
    float  *pData;
    u_int  *pVif  = (u_int *)(pPkt + 1);
    
    // DMA END tag
    pPkt[0] = ( 1 + _loop + (_loop >> 2)) | (7 << 28);
    pPkt[1] = 0;
    
    pVif[0] = SCE_VIF0_SET_STCYCL (4, 4, 0);
    
    pVif[1] = SCE_VIF0_SET_UNPACK (0, _loop, 108 /*V4-32*/, 0);
    
    pData = (float *)&pVif[2];
    for (int i = 0; i < _loop; i++)
    {
        pData[0] = ell0[0];
        pData[1] = ell1[0];
        pData[2] = ell2[0];
        pData[3] = ell3[0];
        
        pData += 4;
        ell0++;
        ell1++;
        ell2++;
        ell3++;
    }
    
    pVif = (u_int *)&pData[0];
    pVif[0] = SCE_VIF0_SET_UNPACK (192, _loop >> 2, 108 /*V4-32*/, 0);
    
    pData++;
    for (int i = 0; i < (_loop >> 2); i++)
    {
        pData[0] = ex[0];
        pData[1] = ex[1];
        pData[2] = ex[2];
        pData[3] = ex[3];
        
        pData += 4;
        ex    += 4;
    }
    
    pVif = (u_int *)&pData[0];
    pVif[0] = SCE_VIF0_SET_MSCAL (0,0);
    pVif[1] = SCE_VIF0_SET_NOP (0);
    pVif[2] = SCE_VIF0_SET_NOP (0);
    
    sceDmaSend (dmaVif0, (void *)( 0x70000000 | 0x80000000));
    
    asm ("sync.l");
            
    // copying data to vu0 mem
    // VU0 memory is mapped into main memory at address 0x11004000

    // jm. IMPORTANT!!! _skip % 4 has to be ALWAYS equal to 0!!!    
    /*CopyAlignedQWords ((u_long128*) (ell),  (u_long128 *)0x11004000, _skip);
    CopyAlignedQWords ((u_long128*) ex,     (u_long128 *)0x11004C00, _skip >> 2);

    asm ("vcallms 0");*/
}

// LoadDataAndActivateMicroProgram_dSolveL1T ________________________________________________________________________

 void LoadDataAndActivateMicroProgram_dSolveL1T (const float *ell, float *ex, int _skip, int _loop)
{
    // sending information about where and how data are stored
    asm __volatile__ (" 
    
        ctc2    %0, $vi01
        ctc2    %1, $vi02
        ctc2    %2, $vi03
    ": : "r" ((_loop >> 2) - 1),
         "r" (ellAddr), 
         "r" (exAddr));
                 
    // Test if data is bigger than vu0 memory (4KB)
    // TODO: moverlo a la llamada de la funcion y en caso de q no quepa en 
    // memoria, hacerlo de manera normal...
    if ((_loop + (_loop >> 2)) > 256)  // 256 = 4K
        error ("Data size is bigger than 4KB");

        
    #ifdef PS2_DEBUG
        d_fNbQWords = (float)(_loop + (_loop >> 2));
    #endif
    
    // Create a new DMA packet
    u_long *pPkt  = (u_long *)(0x70000000);
    float  *pData;
    u_int  *pVif  = (u_int *)(pPkt + 1);
    
    // DMA END tag
    pPkt[0] = ( 1 + _loop + (_loop >> 2)) | (7 << 28);
    pPkt[1] = 0;
    
    pVif[0] = SCE_VIF0_SET_STCYCL (4, 4, 0);
    
    pVif[1] = SCE_VIF0_SET_UNPACK (0, _loop, 108 /*V4-32*/, 0);
    
    pData = (float *)&pVif[2];
    for (int i = 0; i < _loop; i++)
    {
        pData[0] = ell[0];
        pData[1] = ell[-1];
        pData[2] = ell[-2];
        pData[3] = ell[-3];
        
        pData += 4;
        ell   += _skip;
    }
    
    pVif = (u_int *)&pData[0];
    pVif[0] = SCE_VIF0_SET_UNPACK (192, _loop >> 2, 108 /*V4-32*/, 0);
    
    pData++;
    for (int i = 0; i < (_loop >> 2); i++)
    {
        pData[0] = ex[0];
        pData[1] = ex[-1];
        pData[2] = ex[-2];
        pData[3] = ex[-3];
        
        pData += 4;
        ex    -= 4;
    }
    
    pVif = (u_int *)&pData[0];
    pVif[0] = SCE_VIF0_SET_MSCAL (0,0);
    pVif[1] = SCE_VIF0_SET_NOP (0);
    pVif[2] = SCE_VIF0_SET_NOP (0);
    
    sceDmaSend (dmaVif0, (void *)( 0x70000000 | 0x80000000));
    
    asm ("sync.l");
}

 void WaitForMicroProgramEnd ()
{
    while ((*VIF0_STAT & 3) != 0) {};
    
    // Wait for DMA and program to complete
    while (dmaVif0->chcr.STR == 1) {};
    asm ("vnop");
}

//__________________________________________________________________________

/* solve L*X=B, with B containing 1 right hand sides.
 * L is an n*n lower triangular matrix with ones on the diagonal.
 * L is stored by rows and its leading dimension is lskip.
 * B is an n*1 matrix that contains the right hand sides.
 * B is stored by columns and its leading dimension is also lskip.
 * B is overwritten with X.
 * this processes blocks of 4*4.
 * if this is in the factorizer source file, n must be a multiple of 4.*/
 


  void sceVu0AddScaled (sceVu0FVECTOR dest, sceVu0FVECTOR src, float f)
{
    asm __volatile__("
    	lqc2        vf4,0x0(%1)
    	lqc2        vf7,0x0(%0)
    	mfc1        $8,%2
    	qmtc2       $8,vf5
    	vmulx.xyzw	vf6,vf4,vf5
    	vadd.xyzw	vf7,vf7,vf6
    	sqc2        vf7,0x0(%0)
    	": : "r" (dest) , "r" (src), "f" (f):"$f8");
    	
    /*float *pdest = &dest[0];
    float *psrc  = &src[0];
    
    *pdest++ = *psrc++ * f;
    *pdest++ = *psrc++ * f;
    *pdest++ = *psrc++ * f;
    *pdest   = *psrc * f;*/
}

  void sceVu0AddScaled2 (sceVu0FVECTOR dest, sceVu0FVECTOR src, sceVu0FVECTOR src2)
{
    asm __volatile__("
    	lqc2        vf4,0x0(%0)
    	lqc2        vf5,0x0(%1)
    	lqc2        vf6,0x0(%2)
    	
    	vmulx.xyzw	vf7,vf5,vf6
    	vadd.xyzw	vf4,vf4,vf7
    	sqc2        vf4,0x0(%0)
    	": : "r" (dest) , "r" (src), "r" (src2):);
    	
    /*
    float *pdest = &dest[0];
    float *psrc1 = &src[0];
    float *psrc2 = &src[0];
    
    *pdest++ = (*psrc1++) * (*psrc2++);
    *pdest++ = (*psrc1++) * (*psrc2++);
    *pdest++ = (*psrc1++) * (*psrc2++);
    *pdest   = (*psrc1)   * (*psrc2);*/
}

 void dSolveL1_Normal (const float *L, float *B, int n, int lskip1)
{  
    // declare variables - Z matrix, p and q vectors, etc 
    float Z11,Z21,Z31,Z41,p1,q1,p2,p3,p4,*ex;
    const float *ell;
    int lskip2,lskip3,i,j;
    // compute lskip values 
    lskip2 = 2*lskip1;
    lskip3 = 3*lskip1;
    // compute all 4 x 1 blocks of X 
    for (i=0; i <= n-4; i+=4) {
    // compute all 4 x 1 block of X, from rows i..i+4-1 
    // set the Z matrix to 0 
    Z11=0;
    Z21=0;
    Z31=0;
    Z41=0;
    ell = L + i*lskip1;
    ex = B;
    // the inner loop that computes outer products and adds them to Z 
    for (j=i-12; j >= 0; j -= 12) {
      // load p and q values 
      p1=ell[0];
      q1=ex[0];
      p2=ell[lskip1];
      p3=ell[lskip2];
      p4=ell[lskip3];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      // load p and q values 
      p1=ell[1];
      q1=ex[1];
      p2=ell[1+lskip1];
      p3=ell[1+lskip2];
      p4=ell[1+lskip3];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      // load p and q values 
      p1=ell[2];
      q1=ex[2];
      p2=ell[2+lskip1];
      p3=ell[2+lskip2];
      p4=ell[2+lskip3];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      // load p and q values 
      p1=ell[3];
      q1=ex[3];
      p2=ell[3+lskip1];
      p3=ell[3+lskip2];
      p4=ell[3+lskip3];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      // load p and q values 
      p1=ell[4];
      q1=ex[4];
      p2=ell[4+lskip1];
      p3=ell[4+lskip2];
      p4=ell[4+lskip3];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      // load p and q values 
      p1=ell[5];
      q1=ex[5];
      p2=ell[5+lskip1];
      p3=ell[5+lskip2];
      p4=ell[5+lskip3];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      // load p and q values 
      p1=ell[6];
      q1=ex[6];
      p2=ell[6+lskip1];
      p3=ell[6+lskip2];
      p4=ell[6+lskip3];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      // load p and q values 
      p1=ell[7];
      q1=ex[7];
      p2=ell[7+lskip1];
      p3=ell[7+lskip2];
      p4=ell[7+lskip3];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      // load p and q values 
      p1=ell[8];
      q1=ex[8];
      p2=ell[8+lskip1];
      p3=ell[8+lskip2];
      p4=ell[8+lskip3];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      // load p and q values 
      p1=ell[9];
      q1=ex[9];
      p2=ell[9+lskip1];
      p3=ell[9+lskip2];
      p4=ell[9+lskip3];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      // load p and q values 
      p1=ell[10];
      q1=ex[10];
      p2=ell[10+lskip1];
      p3=ell[10+lskip2];
      p4=ell[10+lskip3];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      // load p and q values 
      p1=ell[11];
      q1=ex[11];
      p2=ell[11+lskip1];
      p3=ell[11+lskip2];
      p4=ell[11+lskip3];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      // advance pointers 
      ell += 12;
      ex += 12;
      // end of inner loop 
    }
    // compute left-over iterations 
    j += 12;
    for (; j > 0; j--) {
      // load p and q values 
      p1=ell[0];
      q1=ex[0];
      p2=ell[lskip1];
      p3=ell[lskip2];
      p4=ell[lskip3];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      Z21 += p2 * q1;
      Z31 += p3 * q1;
      Z41 += p4 * q1;
      // advance pointers 
      ell += 1;
      ex += 1;
    }
    // finish computing the X(i) block 
    Z11 = ex[0] - Z11;
    ex[0] = Z11;
    p1 = ell[lskip1];
    Z21 = ex[1] - Z21 - p1*Z11;
    ex[1] = Z21;
    p1 = ell[lskip2];
    p2 = ell[1+lskip2];
    Z31 = ex[2] - Z31 - p1*Z11 - p2*Z21;
    ex[2] = Z31;
    p1 = ell[lskip3];
    p2 = ell[1+lskip3];
    p3 = ell[2+lskip3];
    Z41 = ex[3] - Z41 - p1*Z11 - p2*Z21 - p3*Z31;
    ex[3] = Z41;
    // end of outer loop 
    }
    // compute rows at end that are not a multiple of block size 
    for (; i < n; i++) {
    // compute all 1 x 1 block of X, from rows i..i+1-1 
    // set the Z matrix to 0 
    Z11=0;
    ell = L + i*lskip1;
    ex = B;
    // the inner loop that computes outer products and adds them to Z 
    for (j=i-12; j >= 0; j -= 12) {
      // load p and q values 
      p1=ell[0];
      q1=ex[0];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      // load p and q values 
      p1=ell[1];
      q1=ex[1];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      // load p and q values 
      p1=ell[2];
      q1=ex[2];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      // load p and q values 
      p1=ell[3];
      q1=ex[3];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      // load p and q values 
      p1=ell[4];
      q1=ex[4];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      // load p and q values 
      p1=ell[5];
      q1=ex[5];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      // load p and q values 
      p1=ell[6];
      q1=ex[6];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      // load p and q values 
      p1=ell[7];
      q1=ex[7];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      // load p and q values 
      p1=ell[8];
      q1=ex[8];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      // load p and q values 
      p1=ell[9];
      q1=ex[9];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      // load p and q values 
      p1=ell[10];
      q1=ex[10];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      // load p and q values 
      p1=ell[11];
      q1=ex[11];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      // advance pointers 
      ell += 12;
      ex += 12;
      // end of inner loop 
    }
    // compute left-over iterations 
    j += 12;
    for (; j > 0; j--) {
      // load p and q values 
      p1=ell[0];
      q1=ex[0];
      // compute outer product and add it to the Z matrix 
      Z11 += p1 * q1;
      // advance pointers 
      ell += 1;
      ex += 1;
    }
    // finish computing the X(i) block 
    Z11 = ex[0] - Z11;
    ex[0] = Z11;
    }
}

 void dSolveL1_MacroMode (const float *L, float *B, int n, int lskip1)
{
      // declare variables - Z matrix, p and q vectors, etc
      sceVu0FVECTOR Z;
      sceVu0FVECTOR p;
      
      float *pex, *pZ;
      const float *plskip1, *plskip2, *plskip3, *plskip4;
      float *ex;
      const float *ell;
      int lskip2,lskip3,i,j;
      //compute lskip values
      lskip2 = 2*lskip1;
      lskip3 = 3*lskip1;
      // compute all 4 x 1 blocks of X
      
      for (i=0; i <= n-4; i+=4) {
        // compute all 4 x 1 block of X, from rows i..i+4-1
        // set the Z matrix to 0
        ZERO (Z);
        
        ell = L + i*lskip1;
        ex = B;
        
        pex     = &ex[0];
        plskip1 = &ell[0];
        plskip2 = &ell[lskip1];
        plskip3 = &ell[lskip2];
        plskip4 = &ell[lskip3];
        
        #ifdef PS2_DEBUG
            d_oTimerDebug.On ();
        #endif
        
        // the inner loop that computes outer products and adds them to Z
        for (j=i-4; j >= 0; j -= 4) 
        {
          SET(p, *plskip1++, *plskip2++, *plskip3++, *plskip4++);
          sceVu0AddScaled (Z, p, *pex++);
          
          SET(p, *plskip1++, *plskip2++, *plskip3++, *plskip4++);
          sceVu0AddScaled (Z, p, *pex++);
          
          SET(p, *plskip1++, *plskip2++, *plskip3++, *plskip4++);
          sceVu0AddScaled (Z, p, *pex++);
          
          SET(p, *plskip1++, *plskip2++, *plskip3++, *plskip4++);
          sceVu0AddScaled (Z, p, *pex++);      
        }        
    
        #ifdef PS2_DEBUG    
            d_oTimerDebug.Off ();
            d_fLoopTime = 10000.0f * d_oTimerDebug.fGetElapsedTime ();
        #endif
        
        // finish computing the X(i) block
        pZ = &Z[0];
        
        *pZ = *pex - *pZ;
        *pex++ = *pZ;
        
        *(pZ + 1) = *pex - *(pZ + 1) - *plskip2 * (*pZ);
        *pex++ = *(pZ + 1);
        
        *(pZ + 2) = *pex - *(pZ + 2) - *plskip3 * (*pZ) - *(plskip3 + 1) * *(pZ + 1);
        *pex++ = *(pZ + 2);
        
        *(pZ + 3) = *pex - *(pZ + 3) - *plskip4 * (*pZ) - *(plskip3 + 1) * *(pZ + 1) - *(plskip4 + 2) * *(pZ + 2);
        *pex++ = *(pZ + 3);
      }
      
      // compute rows at end that are not a multiple of block size
      for (; i < n; i++) {
        // compute all 1 x 1 block of X, from rows i..i+1-1
        // set the Z matrix to 0
        Z[0]=0;
        ell = L + i*lskip1;
        ex = B;
        
        pZ      = &Z[0];
        pex     = &ex[0];
        plskip1 = &ell[0];
        
        // the inner loop that computes outer products and adds them to Z
        for (j=i-12; j >= 0; j -= 12) {
          *pZ += (*plskip1++) * (*pex++);
          
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
        }
        // compute left-over iterations
        j += 12;
        for (; j > 0; j--) {
          *pZ += (*plskip1++) * (*pex++);
        }
        // finish computing the X(i) block
        *pZ = *pex - *pZ;
        *pex = *pZ;
      }
}

 void dSolveL1_MicroMode (const float *L, float *B, int n, int lskip1)
{
    // declare variables - Z matrix, p and q vectors, etc
    sceVu0FVECTOR Z;
    
    float *pex, *pZ;
    const float *plskip1, *plskip2, *plskip3, *plskip4;
    float *ex;
    const float *ell;
    int lskip2,lskip3,i,j;
    //compute lskip values
    lskip2 = 2*lskip1;
    lskip3 = 3*lskip1;
    // compute all 4 x 1 blocks of X
    
    i = 0;
    for (i=0; i <= n-4; i+=4) {
        // compute all 4 x 1 block of X, from rows i..i+4-1
        // set the Z matrix to 0
        ZERO (Z);
        
        ell = L + i*lskip1;
        ex = B;
        
        pex     = &ex[0];
        plskip1 = &ell[0];
        plskip2 = &ell[lskip1];
        plskip3 = &ell[lskip2];
        plskip4 = &ell[lskip3];
        
        if (i > 0)
        {
            #ifdef PS2_DEBUG
                d_oTimerDebug.On ();
                d_fLoop = (float)i;
            #endif
            
            LoadDataAndActivateMicroProgram_dSolveL1 (plskip1, plskip2, plskip3, plskip4, pex, i);
            
            WaitForMicroProgramEnd ();
            
            // recovering the calculated Z value
            asm __volatile__ ("
            
                sqc2    $vf09, 0x0(%0)
                
            ": : "r" (Z) : );
            
            #ifdef PS2_DEBUG            
                d_oTimerDebug.Off ();
                d_fVu0Time = 10000.0f * d_oTimerDebug.fGetElapsedTime ();
            #endif            
        }
        
        // finish computing the X(i) block
        pZ = &Z[0];
        
        *pZ = *pex - *pZ;
        *pex++ = *pZ;
        
        *(pZ + 1) = *pex - *(pZ + 1) - *plskip2 * (*pZ);
        *pex++ = *(pZ + 1);
        
        *(pZ + 2) = *pex - *(pZ + 2) - *plskip3 * (*pZ) - *(plskip3 + 1) * *(pZ + 1);
        *pex++ = *(pZ + 2);
        
        *(pZ + 3) = *pex - *(pZ + 3) - *plskip4 * (*pZ) - *(plskip3 + 1) * *(pZ + 1) - *(plskip4 + 2) * *(pZ + 2);
        *pex++ = *(pZ + 3);
      }
      
      // compute rows at end that are not a multiple of block size
      for (; i < n; i++) {
        // compute all 1 x 1 block of X, from rows i..i+1-1
        // set the Z matrix to 0
        Z[0]=0;
        ell = L + i*lskip1;
        ex = B;
        
        pZ      = &Z[0];
        pex     = &ex[0];
        plskip1 = &ell[0];
        
        // the inner loop that computes outer products and adds them to Z
        for (j=i-12; j >= 0; j -= 12) {
          *pZ += (*plskip1++) * (*pex++);
          
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
          *pZ += (*plskip1++) * (*pex++);
        }
        // compute left-over iterations
        j += 12;
        for (; j > 0; j--) {
          *pZ += (*plskip1++) * (*pex++);
        }
        // finish computing the X(i) block
        *pZ = *pex - *pZ;
        *pex = *pZ;
      }

}

/* solve L*X=B, with B containing 1 right hand sides.
 * L is an n*n lower triangular matrix with ones on the diagonal.
 * L is stored by rows and its leading dimension is lskip.
 * B is an n*1 matrix that contains the right hand sides.
 * B is stored by columns and its leading dimension is also lskip.
 * B is overwritten with X.
 * this processes blocks of 4*4.
 * if this is in the factorizer source file, n must be a multiple of 4.*/
 
 
/* solve L^T * x=b, with b containing 1 right hand side.
 * L is an n*n lower triangular matrix with ones on the diagonal.
 * L is stored by rows and its leading dimension is lskip.
 * b is an n*1 matrix that contains the right hand side.
 * b is overwritten with x.
 * this processes blocks of 4.*/
 

 void dSolveL1T_Normal (const float *L, float *B, int n, int lskip1)
{  
	// sv. (debug)
	// oSerie.Add( (float) n , 1.0f);

  // declare variables - Z matrix, p and q vectors, etc 
  float Z11,m11,Z21,m21,Z31,m31,Z41,m41,p1,q1,p2,p3,p4,*ex;
  const float *ell;
  int lskip2,lskip3,i,j;
  // special handling for L and B because we're solving L1 *transpose* 
  L = L + (n-1)*(lskip1+1);
  B = B + n-1;
  lskip1 = -lskip1;
  // compute lskip values 
  lskip2 = 2*lskip1;
  lskip3 = 3*lskip1;
  // compute all 4 x 1 blocks of X 
  for (i=0; i <= n-4; i+=4) {
    // compute all 4 x 1 block of X, from rows i..i+4-1 
    // set the Z matrix to 0 
    Z11=0;
    Z21=0;
    Z31=0;
    Z41=0;
    ell = L - i;
    ex = B;
    // the inner loop that computes outer products and adds them to Z 
    for (j=i-4; j >= 0; j -= 4) {
      // load p and q values 
      p1=ell[0];
      q1=ex[0];
      p2=ell[-1];
      p3=ell[-2];
      p4=ell[-3];
      // compute outer product and add it to the Z matrix 
      m11 = p1 * q1;
      m21 = p2 * q1;
      m31 = p3 * q1;
      m41 = p4 * q1;
      ell += lskip1;
      Z11 += m11;
      Z21 += m21;
      Z31 += m31;
      Z41 += m41;
      // load p and q values 
      p1=ell[0];
      q1=ex[-1];
      p2=ell[-1];
      p3=ell[-2];
      p4=ell[-3];
      // compute outer product and add it to the Z matrix 
      m11 = p1 * q1;
      m21 = p2 * q1;
      m31 = p3 * q1;
      m41 = p4 * q1;
      ell += lskip1;
      Z11 += m11;
      Z21 += m21;
      Z31 += m31;
      Z41 += m41;
      // load p and q values 
      p1=ell[0];
      q1=ex[-2];
      p2=ell[-1];
      p3=ell[-2];
      p4=ell[-3];
      // compute outer product and add it to the Z matrix 
      m11 = p1 * q1;
      m21 = p2 * q1;
      m31 = p3 * q1;
      m41 = p4 * q1;
      ell += lskip1;
      Z11 += m11;
      Z21 += m21;
      Z31 += m31;
      Z41 += m41;
      // load p and q values 
      p1=ell[0];
      q1=ex[-3];
      p2=ell[-1];
      p3=ell[-2];
      p4=ell[-3];
      // compute outer product and add it to the Z matrix 
      m11 = p1 * q1;
      m21 = p2 * q1;
      m31 = p3 * q1;
      m41 = p4 * q1;
      ell += lskip1;
      ex -= 4;
      Z11 += m11;
      Z21 += m21;
      Z31 += m31;
      Z41 += m41;
      // end of inner loop 
    }
    // compute left-over iterations 
    j += 4;
    for (; j > 0; j--) {
      // load p and q values 
      p1=ell[0];
      q1=ex[0];
      p2=ell[-1];
      p3=ell[-2];
      p4=ell[-3];
      // compute outer product and add it to the Z matrix 
      m11 = p1 * q1;
      m21 = p2 * q1;
      m31 = p3 * q1;
      m41 = p4 * q1;
      ell += lskip1;
      ex -= 1;
      Z11 += m11;
      Z21 += m21;
      Z31 += m31;
      Z41 += m41;
    }
    // finish computing the X(i) block 
    Z11 = ex[0] - Z11;
    ex[0] = Z11;
    p1 = ell[-1];
    Z21 = ex[-1] - Z21 - p1*Z11;
    ex[-1] = Z21;
    p1 = ell[-2];
    p2 = ell[-2+lskip1];
    Z31 = ex[-2] - Z31 - p1*Z11 - p2*Z21;
    ex[-2] = Z31;
    p1 = ell[-3];
    p2 = ell[-3+lskip1];
    p3 = ell[-3+lskip2];
    Z41 = ex[-3] - Z41 - p1*Z11 - p2*Z21 - p3*Z31;
    ex[-3] = Z41;
    // end of outer loop 
  }
  // compute rows at end that are not a multiple of block size 
  for (; i < n; i++) {
    // compute all 1 x 1 block of X, from rows i..i+1-1 
    // set the Z matrix to 0 
    Z11=0;
    ell = L - i;
    ex = B;
    // the inner loop that computes outer products and adds them to Z 
    for (j=i-4; j >= 0; j -= 4) {
      // load p and q values 
      p1=ell[0];
      q1=ex[0];
      // compute outer product and add it to the Z matrix 
      m11 = p1 * q1;
      ell += lskip1;
      Z11 += m11;
      // load p and q values 
      p1=ell[0];
      q1=ex[-1];
      // compute outer product and add it to the Z matrix 
      m11 = p1 * q1;
      ell += lskip1;
      Z11 += m11;
      // load p and q values 
      p1=ell[0];
      q1=ex[-2];
      // compute outer product and add it to the Z matrix 
      m11 = p1 * q1;
      ell += lskip1;
      Z11 += m11;
      // load p and q values 
      p1=ell[0];
      q1=ex[-3];
      // compute outer product and add it to the Z matrix 
      m11 = p1 * q1;
      ell += lskip1;
      ex -= 4;
      Z11 += m11;
      // end of inner loop 
    }
    // compute left-over iterations 
    j += 4;
    for (; j > 0; j--) {
      // load p and q values 
      p1=ell[0];
      q1=ex[0];
      // compute outer product and add it to the Z matrix 
      m11 = p1 * q1;
      ell += lskip1;
      ex -= 1;
      Z11 += m11;
    }
    // finish computing the X(i) block 
    Z11 = ex[0] - Z11;
    ex[0] = Z11;
  }
}

// dSolveL1T_MacroMode __________________________________________________________________

 void dSolveL1T_MacroMode (const float *L, float *B, int n, int lskip1)
{  
  
  sceVu0FVECTOR Z;
  sceVu0FVECTOR p;
  
  // declare variables - Z matrix, p and q vectors, etc 
  float *ex;
  const float *ell;
  int lskip2,lskip3,i,j;
  // special handling for L and B because we're solving L1 *transpose* 
  L = L + (n-1)*(lskip1+1);
  B = B + n-1;
  lskip1 = -lskip1;
  // compute lskip values 
  lskip2 = 2*lskip1;
  lskip3 = 3*lskip1;
  // compute all 4 x 1 blocks of X 
  for (i=0; i <= n-4; i+=4) 
  {
    // compute all 4 x 1 block of X, from rows i..i+4-1
    // set the Z matrix to 0
    ZERO (Z);
    
    ell = L - i;
    ex = B;
    
    // the inner loop that computes outer products and adds them to Z
    for (j=i-4; j >= 0; j -= 4) 
    {
      // load p and q values
      SET (p, ell[0], ell[-1], ell[-2], ell[-3]);
      // compute outer product and add it to the Z matrix
      sceVu0AddScaled (Z, p, ex[0]);
      // updating pointers
      ex--;
      ell += lskip1;
      
      // load p and q values
      SET (p, ell[0], ell[-1], ell[-2], ell[-3]);
      // compute outer product and add it to the Z matrix
      sceVu0AddScaled (Z, p, ex[0]);
      // updating pointers
      ex--;
      ell += lskip1;
      
      // load p and q values
      SET (p, ell[0], ell[-1], ell[-2], ell[-3]);
      // compute outer product and add it to the Z matrix
      sceVu0AddScaled (Z, p, ex[0]);
      // updating pointers
      ex--;
      ell += lskip1;
      
      // load p and q values
      SET (p, ell[0], ell[-1], ell[-2], ell[-3]);
      // compute outer product and add it to the Z matrix
      sceVu0AddScaled (Z, p, ex[0]);
      // updating pointers
      ex--;
      ell += lskip1;
      // end of inner loop 
    }
        
    // finish computing the X(i) block 
    Z[0] = ex[0] - Z[0];
    ex[0] = Z[0];
    p[0] = ell[-1];
    Z[1] = ex[-1] - Z[1] - p[0]*Z[0];
    ex[-1] = Z[1];
    p[0] = ell[-2];
    p[1] = ell[-2+lskip1];
    Z[2] = ex[-2] - Z[2] - p[0]*Z[0] - p[1]*Z[1];
    ex[-2] = Z[2];
    p[0] = ell[-3];
    p[1] = ell[-3+lskip1];
    p[2] = ell[-3+lskip2];
    Z[3] = ex[-3] - Z[3] - p[0]*Z[0] - p[1]*Z[1] - p[2]*Z[2];
    ex[-3] = Z[3];
    // end of outer loop 
  }
  // compute rows at end that are not a multiple of block size 
  for (; i < n; i++) {
    // compute all 1 x 1 block of X, from rows i..i+1-1 
    // set the Z matrix to 0 
    Z[0]=0;
    ell = L - i;
    ex = B;
    
    ex[0];
    ell[0];
    
    // the inner loop that computes outer products and adds them to Z 
    for (j=i-4; j >= 0; j -= 4) 
    {
      Z[0] += ex[0] * ell[0];
      
      Z[0] += ex[-1] * ell[lskip1];
      
      Z[0] += ex[-2] * ell[lskip2];
      
      Z[0] += ex[-3] * ell[lskip3];
      
      ex  -= 4;
      ell += lskip1;
      // end of inner loop 
    }
    // compute left-over iterations 
    j += 4;
    for (; j > 0; j--) 
    {
      Z[0] += ex[0] * ell[0];
      
      ex--;
      ell += lskip1;
    }
    // finish computing the X(i) block 
    Z[0] = ex[0] - Z[0];
    ex[0] = Z[0];
  }
}

// dSolveL1T_MicroMode __________________________________________________________________

 void dSolveL1T_MicroMode (const float *L, float *B, int n, int lskip1)
{  
  
  sceVu0FVECTOR Z;
  sceVu0FVECTOR p;
  
  // declare variables - Z matrix, p and q vectors, etc 
  float *ex;
  const float *ell;
  int lskip2,lskip3,i,j;
  // special handling for L and B because we're solving L1 *transpose* 
  L = L + (n-1)*(lskip1+1);
  B = B + n-1;
  lskip1 = -lskip1;
  // compute lskip values 
  lskip2 = 2*lskip1;
  lskip3 = 3*lskip1;
  // compute all 4 x 1 blocks of X 
  for (i=0; i <= n-4; i+=4) 
  {
    // compute all 4 x 1 block of X, from rows i..i+4-1
    // set the Z matrix to 0
    ZERO (Z);
    
    ell = L - i;
    ex = B;
    
    if (i > 0)
    {
        LoadDataAndActivateMicroProgram_dSolveL1T (ell, ex, lskip1, i);
        
        WaitForMicroProgramEnd ();
            
            // recovering the calculated Z value
            asm __volatile__ ("
            
                sqc2    $vf9, 0x0(%0)
                
            ": : "r" (Z) : );
        
        
        ell += i * lskip1;
        ex  -= i;        
    }        
    // finish computing the X(i) block 
    Z[0] = ex[0] - Z[0];
    ex[0] = Z[0];
    p[0] = ell[-1];
    Z[1] = ex[-1] - Z[1] - p[0]*Z[0];
    ex[-1] = Z[1];
    p[0] = ell[-2];
    p[1] = ell[-2+lskip1];
    Z[2] = ex[-2] - Z[2] - p[0]*Z[0] - p[1]*Z[1];
    ex[-2] = Z[2];
    p[0] = ell[-3];
    p[1] = ell[-3+lskip1];
    p[2] = ell[-3+lskip2];
    Z[3] = ex[-3] - Z[3] - p[0]*Z[0] - p[1]*Z[1] - p[2]*Z[2];
    ex[-3] = Z[3];
    // end of outer loop 
  }
  // compute rows at end that are not a multiple of block size 
  for (; i < n; i++) {
    // compute all 1 x 1 block of X, from rows i..i+1-1 
    // set the Z matrix to 0 
    Z[0]=0;
    ell = L - i;
    ex = B;
    
    ex[0];
    ell[0];
    
    // the inner loop that computes outer products and adds them to Z 
    for (j=i-4; j >= 0; j -= 4) 
    {
      Z[0] += ex[0] * ell[0];
      
      Z[0] += ex[-1] * ell[lskip1];
      
      Z[0] += ex[-2] * ell[lskip2];
      
      Z[0] += ex[-3] * ell[lskip3];
      
      ex  -= 4;
      ell += lskip1;
      // end of inner loop 
    }
    // compute left-over iterations 
    j += 4;
    for (; j > 0; j--) 
    {
      Z[0] += ex[0] * ell[0];
      
      ex--;
      ell += lskip1;
    }
    // finish computing the X(i) block 
    Z[0] = ex[0] - Z[0];
    ex[0] = Z[0];
  }
}

// _________________________________________________________________________

	



 void LCPMath::dSolveL1 (const float *L, float *B, int n, int lskip1)
{  
    // declare variables - Z matrix, p and q vectors, etc 
    float Z11,p1,q1,p2,p3,*ex;
    const float *ell;
    int lskip2,lskip3,i,j;
    sceVu0FVECTOR pv, qv, Z;
    	
    // compute lskip values 
    lskip2 = 2*lskip1;
    lskip3 = 3*lskip1;
    // compute all 4 x 1 blocks of X 
    for (i=0; i <= n-4; i+=4) 
    {
	    // compute all 4 x 1 block of X, from rows i..i+4-1 
	    // set the Z matrix to 0 	    
		asm __volatile__("
				vsub		vf4,vf0,vf0			# vf4 = 0
		");				
	    ell = L + i*lskip1;
	    ex = B;
	    // the inner loop that computes outer products and adds them to Z 

	    for (j=i; j > 0; j--) 
	    {
	     // load p and q values 
			qv[0] =ex[0];	    
			pv[0] =ell[0];			
			pv[1] =ell[lskip1];		
			pv[2] =ell[lskip2];		
			pv[3] =ell[lskip3];
	      // compute outer product and add it to the Z matrix 		
			asm __volatile__ ("
				lqc2 vf1,%0	
				lqc2 vf2,%1	
				vmulx.xyzw vf3,vf1,vf2 
				vadd	vf4,vf4,vf3 
			": : "r" (pv) , "r" (qv) ); 		
	      // advance pointers 
	      ell ++;
	      ex ++;
	    }
	    
	   	asm __volatile__("
			sqc2        vf4,0x0(%0)
	   	": : "r" (Z));    
	    
   	    // finish computing the X(i) block     
	    Z[0] = ex[0] - Z[0];
	    ex[0] = Z[0];

	    p1 = ell[lskip1];
	    Z[1] = ex[1] - Z[1] - p1*Z[0];
	    ex[1] = Z[1];

	    p1 = ell[lskip2];
	    p2 = ell[1+lskip2];
	    Z[2] = ex[2] - Z[2] - p1*Z[0] - p2*Z[1];
	    ex[2] = Z[2];

	    p1 = ell[lskip3];
	    p2 = ell[1+lskip3];
	    p3 = ell[2+lskip3];
	    Z[3] = ex[3] - Z[3] - p1*Z[0] - p2*Z[1] - p3*Z[2];
	    ex[3] = Z[3];
	    
	    // end of outer loop 
    }
    // compute rows at end that are not a multiple of block size 
    for (; i < n; i++) 
    {
	    // compute all 1 x 1 block of X, from rows i..i+1-1 
    	// set the Z matrix to 0 
	    Z11=0;
	    
	       // set the Z matrix to 0 	    
		asm __volatile__("
				vsub		vf4,vf0,vf0			# vf4 = 0
		");				
		
	    ell = L + i*lskip1;
	    ex = B;
	    // the inner loop that computes outer products and adds them to Z 
    	for (j=i-4; j >= 0; j -= 4) 
	    {	    
		    qv[0] =ex[0];	    
   		    qv[1] =ex[1];	    
   		    qv[2] =ex[2];	    
   		    qv[3] =ex[3];
   		    		    
			pv[0] =ell[0];			
			pv[1] =ell[1];		
			pv[2] =ell[2];		
			pv[3] =ell[3];
			
			asm __volatile__ ("
				lqc2 vf1,%0	
				lqc2 vf2,%1	
				vmul vf3,vf1,vf2 
				vadd vf4,vf4,vf3 
			": : "r" (pv) , "r" (qv) ); 
		    		      
	      // advance pointers 
	      ell += 4;
	      ex += 4;
	      // end of inner loop 
	    }
	    
	    asm __volatile__("
	   		vaddy.x 	vf4, vf4, vf4
	   		vaddz.x 	vf4, vf4, vf4
	   		vaddw.x		vf4, vf4, vf4	    		    	
			sqc2        vf4,0x0(%0)
	   	": : "r" (Z));    
	    
//	    Z11 = Z[0] + Z[1] + Z[2] + Z[3];	  
		Z11 = Z[0];
	    
	    // compute left-over iterations 
	    j += 4;
	    for (; j > 0; j--) 
	    {
	      // load p and q values 
	      p1=ell[0];
	      q1=ex[0];
	      // compute outer product and add it to the Z matrix 
	      Z11 += p1 * q1;
	      // advance pointers 
	      ell += 1;
	      ex += 1;
	    }
	    // finish computing the X(i) block 
	    Z11 = ex[0] - Z11;
	    ex[0] = Z11;
    }
}



/*
void dSolveL1 (const float *L, float *B, int n, int lskip1)
{    
	#ifdef PS2_DEBUG
        if (d_fVu0Enabled)
        {
            if (d_fMicroProgram)
                dSolveL1_MicroMode (L, B, n, lskip1);
            else
                dSolveL1_MacroMode (L, B, n, lskip1);
        }
        else
            dSolveL1_Normal (L, B, n, lskip1);
    #else
        dSolveL1_MacroMode (L, B, n, lskip1);
    #endif
    
    dSolveL1_Normal (L, B, n, lskip1);
}


*/

void LCPMath::dSolveL1T (const float *L, float *B, int n, int lskip1)
{
/*
    #ifdef PS2_DEBUG
        if (d_fVu0Enabled)
        {
            if (d_fMicroProgram)
                dSolveL1T_MicroMode (L, B, n, lskip1);
            else
                dSolveL1T_MacroMode (L, B, n, lskip1);
        }
        else
            dSolveL1T_Normal (L, B, n, lskip1);
    #else
        dSolveL1T_MacroMode (L, B, n, lskip1);
    #endif
    */
    
    dSolveL1T_Normal (L, B, n, lskip1);
}
