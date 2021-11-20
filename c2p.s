    xdef _c2p
    
data EQUR a0
dest EQUR a1

    section code,code
    
_c2p
    movem.l d0-d7,-(sp)
    
    move.w  #$5555,d6
    move.w  #$AAAA,d7
    
    REPT 180
    
*             d0 = *data++;
*             d0 <<= 4;

    move.b  (data)+,d0
    lsl.w   #4,d0

*             d1 = *data++;
*             d1 <<= 4;

    move.b  (data)+,d1
    lsl.w   #4, d1

*             d2 = *data++;
*             d2 <<= 4;

    move.b  (data)+,d2
    lsl.w   #4, d2

*             d3 = *data++;
*             d3 <<= 4;

    move.b  (data)+,d3
    lsl.w   #4, d3
    
*             d0 |= *data++;
*             d0 <<= 4;

    or.b  (data)+,d0
    lsl.w   #4, d0

*             d1 |= *data++;
*             d1 <<= 4;

    or.b  (data)+,d1
    lsl.w   #4, d1

*             d2 |= *data++;
*             d2 <<= 4;

    or.b  (data)+,d2
    lsl.w   #4, d2

*             d3 |= *data++;
*             d3 <<= 4;


    or.b  (data)+,d3
    lsl.w   #4, d3

*             d0 |= *data++;
*             d0 <<= 4;

    or.b  (data)+,d0
    lsl.w   #4, d0
    
*             d1 |= *data++;
*             d1 <<= 4;


    or.b  (data)+,d1
    lsl.w   #4, d1

*             d2 |= *data++;
*             d2 <<= 4;

    or.b  (data)+,d2
    lsl.w   #4, d2

*             d3 |= *data++;
*             d3 <<= 4;

    or.b  (data)+,d3
    lsl.w   #4, d3

*             d0 |= *data++;
    
    or.b    (data)+,d0

*             d1 |= *data++;

    or.b    (data)+,d1
    
*             d2 |= *data++;

    or.b    (data)+,d2

*             d3 |= *data++;

    or.b    (data)+,d3

*             d0 = d0 << 2;
*             d0 = (d0 | d2);
    
    lsl.w   #2,d0
    or.w    d2,d0

*             d1 = d1 << 2;
*             d1 = (d1 | d3);

    lsl.w   #2,d1
    or.w    d3,d1

*             *dest++ = ((d0 << 1) & 0xAAAA)|(d1 & 0x5555);

    move.w   d0,d4
    move.w   d1,d5
    
    ;lsl.w   #1,d4
    add.w   d4,d4
    and.w   d7,d4    ; AND #$AAAA
    and.w   d6,d5    ; AND #$5555
    or.w    d4,d5
    move.w  d5,(dest)+
    
*             *dest++ = (d0 & 0xAAAA)|((d1 >> 1) & 0x5555);

    lsr.w   #1,d1
    and.w   d6,d1    ; AND #$5555
    and.w   d7,d0    ; AND #$AAAA
    or.w    d1,d0
    move.w  d0,(dest)+
    
*             data+=16;

    lea 16(data),data
    
    ENDR
    
    movem.l (sp)+,d0-d7
    rts
