#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "custom_defines.h"

#include "System.h"

#include "clist.h"

UBYTE frame = 0;
ULONG fps = 0;

void __asm c2p(register __a0 *data, register __a1 *dest);

void c2pC(UBYTE *data, UWORD *dest) {
    UWORD i, d0, d1, d2, d3;
    
    for (i = 0; i < 180; i++) {
            d0 = *data++;
            d0 <<= 4;
            d1 = *data++;
            d1 <<= 4;
            d2 = *data++;
            d2 <<= 4;
            d3 = *data++;
            d3 <<= 4;
            
            d0 |= *data++;
            d0 <<= 4;
            d1 |= *data++;
            d1 <<= 4;
            d2 |= *data++;
            d2 <<= 4;
            d3 |= *data++;
            d3 <<= 4;
            
            d0 |= *data++;
            d0 <<= 4;
            d1 |= *data++;
            d1 <<= 4;
            d2 |= *data++;
            d2 <<= 4;
            d3 |= *data++;
            d3 <<= 4;
            
            d0 |= *data++;
            d1 |= *data++;
            d2 |= *data++;
            d3 |= *data++;
            
            d0 = d0 << 2;
            d0 = (d0 | d2);
   
            d1 = d1 << 2;
            d1 = (d1 | d3);
            
            // Plane 1 merge even and odd bits
            *dest++ = ((d0 << 1) & 0xAAAA)|(d1 & 0x5555);
            // Plane 2 merge even and odd bits
            *dest++ = (d0 & 0xAAAA)|((d1 >> 1) & 0x5555);
            
            data+=16;
    }
}


void c2p_byte(UBYTE *data, UBYTE *dest) {
    UBYTE even_p1, even_p2, odd_p1, odd_p2, d0, d1;
    
    // Odd bits
    d0 = (data[0] << 4) + data[4];
    d1 = (data[2] << 4) + data[6];
    d0 = d0 << 2;
    d0 = (d0 | d1);
    odd_p2 = d0 & 0xAA;
    odd_p1 = (d0 << 1) & 0xAA;
    
    // Even bits
    d0 = (data[1] << 4) + data[5];
    d1 = (data[3] << 4) + data[7];
    d0 = d0 << 2;
    d0 = (d0 | d1);
    even_p2 = (d0 >> 1) & 0x55;
    even_p1 = d0 & 0x55;
    
    // Plane 1 merge even and odd bits
    dest[0] = even_p1|odd_p1;
    // Plane 2 merge even and odd bits
    dest[8] = even_p2|odd_p2;
}

int main()
{
    UWORD *cl;
    UBYTE *chunky;
    UWORD *pic;
    UBYTE *ptr;
    UBYTE *ptr10 = NULL, *ptr11, *ptr12, *ptr13, *ptr14;
    UWORD *sprdata0 = (UWORD*) AllocMem((16+(200<<3)<<1),MEMF_CHIP|MEMF_CLEAR);
	UWORD *sprdata1 = (UWORD*) AllocMem((16+(200<<3)<<1),MEMF_CHIP|MEMF_CLEAR);
    UWORD coplistSize;
    
    chunky = (UBYTE*) AllocMem(32*180, MEMF_ANY);

    sprdata0[0] = 0x5240;
    sprdata0[1] = 0x0602;
	
    sprdata1[0] = 0x5248;
    sprdata1[1] = 0x0602;
    
    // Open libs
    IntuitionBase = (struct IntuitionBase*)OldOpenLibrary("intuition.library");
    
   	// open gfx lib and save original copperlist
  	GfxBase = (struct GfxBase*)OldOpenLibrary("graphics.library");
   	
    oldCopinit = GfxBase->copinit;
   	oldview = GfxBase->ActiView;
    oldViewPort = &IntuitionBase->ActiveScreen->ViewPort;
    
    if (file_ptr = Open("pattern.raw", MODE_OLDFILE))
    {
        pic = (UWORD*)AllocMem(5760, MEMF_ANY);
                   
        Read(file_ptr, pic, 5760);
        
        Close(file_ptr);
    }
    

    cop1 = (UWORD*)AllocMem(sizeof(coplist), MEMF_CHIP | MEMF_CLEAR);
    
    CopyMem(coplist, cop1, sizeof(coplist));
    
    
    ptr10 = AllocMem(((WIDTH * HEIGHT) >> 3) * 5, MEMF_CHIP | MEMF_CLEAR);
    ptr11 = ptr10 + 7200;
    ptr12 = ptr11 + 7200;
    ptr13 = ptr12 + 7200;
    ptr14 = ptr13 + 7200;
    
    ptr = ptr10;

    if (cop1   != NULL &&
        ptr10  != NULL &&
        chunky != NULL) {
       
        palflag = SysBase->PowerSupplyFrequency < 59;
    
        vbr = (SysBase->AttnFlags & AFF_68010) ?
                (void *)Supervisor((void *)GetVBR) : NULL;
                
        LoadView(NULL);
        WaitTOF();
        WaitTOF();
        
        Forbid();
        
        // Save interrupts and DMA
        oldInt = custom->intenar;
        oldDMA = custom->dmaconr;

        // disable all interrupts and DMA
        custom->intena = 0x7fff;
        custom->intreq = 0x7fff; 
        custom->intreq = 0x7fff;
        custom->dmacon = 0x7fff;

        custom->dmacon = DMAF_SETCLR | DMAF_MASTER | DMAF_RASTER | DMAF_COPPER | DMAF_SPRITE;

        cop1[BPL0]     = (ULONG)ptr10 >> 16;
        cop1[BPL0 + 2] = (ULONG)ptr10 & 0xffff;
        cop1[BPL1]     = (ULONG)ptr11 >> 16;
        cop1[BPL1 + 2] = (ULONG)ptr11 & 0xffff;
        cop1[BPL2]     = (ULONG)ptr12 >> 16;
        cop1[BPL2 + 2] = (ULONG)ptr12 & 0xffff;
        cop1[BPL3]     = (ULONG)ptr13 >> 16;
        cop1[BPL3 + 2] = (ULONG)ptr13 & 0xffff;
        cop1[BPL4]     = (ULONG)ptr14 >> 16;
        cop1[BPL4 + 2] = (ULONG)ptr14 & 0xffff;

        cop1[SPR0 ]    = (ULONG)sprdata0 >> 16;
        cop1[SPR0 + 2] = (ULONG)sprdata0 & 0xffff;
        
        cop1[SPR1 ]    = (ULONG)sprdata1 >> 16;
        cop1[SPR1 + 2] = (ULONG)sprdata1 & 0xffff;
		
        fps = 0;
            
        ptr = ptr10;
        
        custom->cop1lc = (ULONG)cop1;
    
        waitVBlank();
        installVBInterrupt();
        
        while ((custom->potinp&0x400) && (cia->ciapra & CIAF_GAMEPORT0)) { 

            waitVBlank();
            fps++;
            
            c2p((APTR)pic,(APTR)&sprdata0[2]);
			
            c2p((APTR)&pic[8],(APTR)&sprdata1[2]);
            
			//custom->color[0] = 0xfff;
        }
        
        removeVBInterrupt();
        
        // restore DMA
        custom->dmacon = 0x7fff;
        custom->dmacon = oldDMA | DMAF_SETCLR | DMAF_MASTER;


        // restore original copper
        custom->cop1lc = (ULONG) oldCopinit;

        // restore interrupts
        custom->intena = oldInt | 0xc000;
        
        Permit();
        
        
        LoadView(oldview);
        WaitTOF();
        WaitTOF();
       
    }
    else
        printf("Not enough memory\n");

    printf("vbcount %i real %i\n", vbcounter, fps);
    
    FreeMem(ptr10, ((WIDTH * HEIGHT) >> 3) * 5);
    
    FreeMem(pic, 5760);
    
    FreeMem(cop1, sizeof(coplist));
    
    FreeMem(chunky, 32*180);
 
    CloseLibrary((struct Library *)GfxBase);
    CloseLibrary((struct Library *)IntuitionBase);


   return 0;
}

