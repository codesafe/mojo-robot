#ifndef _E_INK_
#define _E_INK_

// From epd.h file

#define    CMD_SIZE                           512	

/*
frame format
*/
#define    FRAME_B                            0xA5
#define    FRAME_E0                           0xCC
#define    FRAME_E1                           0x33
#define    FRAME_E2                           0xC3
#define    FRAME_E3                           0x3C


/*
color define
*/
#define    WHITE                              0x03
#define    GRAY                               0x02
#define    DARK_GRAY                          0x01
#define    BLACK                              0x00

/*
command define
*/
#define    CMD_HANDSHAKE                      0x00                                                     //handshake
#define    CMD_SET_BAUD                       0x01                                                     //set baud
#define    CMD_READ_BAUD                      0x02                                                     //read baud
#define    CMD_MEMORYMODE                     0x07                                                     //set memory mode
#define    CMD_STOPMODE                       0x08                                                     //enter stop mode 
#define    CMD_UPDATE                         0x0A                                                     //update
#define    CMD_SCREEN_ROTATION                0x0D                                                     //set screen rotation
#define    CMD_LOAD_FONT                      0x0E                                                     //load font
#define    CMD_LOAD_PIC                       0x0F                                                     //load picture

#define    CMD_SET_COLOR                      0x10                                                     //set color
#define    CMD_SET_EN_FONT                    0x1E                                                     //set english font
#define    CMD_SET_CH_FONT                    0x1F                                                     //set chinese font

#define    CMD_DRAW_PIXEL                     0x20                                                     //set pixel
#define    CMD_DRAW_LINE                      0x22                                                     //draw line
#define    CMD_FILL_RECT                      0x24                                                     //fill rectangle
#define    CMD_DRAW_CIRCLE                    0x26                                                     //draw circle
#define    CMD_FILL_CIRCLE                    0x27                                                     //fill circle
#define    CMD_DRAW_TRIANGLE                  0x28                                                     //draw triangle
#define    CMD_FILL_TRIANGLE                  0x29                                                     //fill triangle
#define    CMD_CLEAR                          0x2E                                                     //clear screen use back color

#define    CMD_DRAW_STRING                    0x30                                                     //draw string
#define    CMD_DRAW_BITMAP                    0x70                                                     //draw bitmap

/*
FONT
*/
#define    GBK32                              0x01
#define    GBK48                              0x02
#define    GBK64                              0x03
	
#define    ASCII32                            0x01
#define    ASCII48                            0x02
#define    ASCII64                            0x03

/*
Memory Mode
*/
#define    MEM_NAND                           0
#define    MEM_TF                             1

/*
set screen rotation
*/
#define    EPD_NORMAL                         0                                                        //screen normal
#define    EPD_INVERSION                      1                                                        //screen inversion  


/*
Pin define
*/
#define    EPD_WAKEUP_PIN                        GPIO_Pin_11       
#define    EPD_WAKEUP_PORT                       GPIOA

#define    EPD_RESET_PIN                         GPIO_Pin_12
#define    EPD_RESET_PORT                        GPIOA


#define    EPD_PORTA_PIN_OUT                     EPD_WAKEUP_PIN | EPD_RESET_PIN
#define    EPD_PORTB_PIN_OUT                     

#define    RCC_GPIO_USE                          RCC_APB2Periph_GPIOA       

#define		TXPACKET_MAX_LEN    (250)
#define		RXPACKET_MAX_LEN    (250)

static const unsigned char _cmd_handshake[8] = { 0xA5, 0x00, 0x09, CMD_HANDSHAKE, FRAME_E0, FRAME_E1, FRAME_E2, FRAME_E3 };             //CMD_HANDSHAKE
static const unsigned char _cmd_update[8] = { 0xA5, 0x00, 0x09, CMD_UPDATE, FRAME_E0, FRAME_E1, FRAME_E2, FRAME_E3 };                   //CMD_UPDATE
static const unsigned char _cmd_set_storage[9] = { 0xA5, 0x00, 0x0A, CMD_MEMORYMODE, 0x00, FRAME_E0, FRAME_E1, FRAME_E2, FRAME_E3 };
static const unsigned char _cmd_draw_bmp_head[8] = { 0xA5, 0x00, 0x00, CMD_DRAW_BITMAP, 0x00, 0x00, 0x00, 0x00 };
static const unsigned char _cmd_draw_bmp_end[4] = { FRAME_E0, FRAME_E1, FRAME_E2, FRAME_E3 };



static const unsigned char _cmd_load_font[8] = { 0xA5, 0x00, 0x09, CMD_LOAD_FONT, FRAME_E0, FRAME_E1, FRAME_E2, FRAME_E3 };             //CMD_LOAD_FONT
static const unsigned char _cmd_load_pic[8] = { 0xA5, 0x00, 0x09, CMD_LOAD_PIC, 0xCC, 0x33, 0xC3, 0x3C };								//CMD_LOAD_PIC
static const unsigned char _cmd_read_baud[8] = { 0xA5, 0x00, 0x09, CMD_READ_BAUD, FRAME_E0, FRAME_E1, FRAME_E2, FRAME_E3 };             //CMD_READ_BAUD
static const unsigned char _cmd_stopmode[8] = { 0xA5, 0x00, 0x09, CMD_STOPMODE, FRAME_E0, FRAME_E1, FRAME_E2, FRAME_E3 };               //CMD_STOPMODE


/*
	0xA5,0x00,
	0x16, --> size of packet with parity
	CMD_DRAW_BITMAP,
	0x00,0x00, --> x pos
	0x00,0x00 --> y pos
	0x50 0x49 0x43 0x37 0x2E 0x42 0x4D 0x50 --> PIC7.BMP
	0x00 --> end of name
	0xCC,0x33,0xC3,0x3C
	,0xDF --> checksum
*/
#endif