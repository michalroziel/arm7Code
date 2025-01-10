#ifndef __C_UEBUNG
#define __C_UEBUNG

unsigned int bcdToSegment(unsigned char* bcd);

/* IO */
#define OUTPUT_DIRECTION    0x01FC0000
#define BCD0                10
#define BCD1                11
#define BCD2                12
#define BCD3                13

/* 7-Segment-Anzeige */
#define SEGMENT0            0x00FC0000
#define SEGMENT1            0x00180000
#define SEGMENT2            0x016C0000
#define SEGMENT3            0x013C0000
#define SEGMENT4            0x01980000
#define SEGMENT5            0x01B40000
#define SEGMENT6            0x01F40000
#define SEGMENT7            0x001C0000
#define SEGMENT8            0x01FC0000
#define SEGMENT9            0x01BC0000
#define SEGMENTF            0x01C40000

#endif  // __C_UEBUNG
