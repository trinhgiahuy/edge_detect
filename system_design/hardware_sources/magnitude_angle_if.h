/***************************************************************************  
 *  accelerator interface header                                              
 ***************************************************************************/ 

#define ACCEL_ADDR                        (0x60000000)

// register map 
   
#define WORD_SIZE (8)

#define GO_ADDR                           (ACCEL_ADDR + ( 0 * (WORD_SIZE))) 
#define GO_READY_ADDR                     (ACCEL_ADDR + ( 1 * (WORD_SIZE))) 
#define DONE_ADDR                         (ACCEL_ADDR + ( 2 * (WORD_SIZE))) 
#define DONE_VALID_ADDR                   (ACCEL_ADDR + ( 3 * (WORD_SIZE))) 
#define INPUT_OFFSET_ADDR                 (ACCEL_ADDR + ( 4 * (WORD_SIZE))) 
#define OUTPUT_OFFSET_ADDR                (ACCEL_ADDR + ( 5 * (WORD_SIZE))) 
#define HEIGHT_ADDR                       (ACCEL_ADDR + ( 6 * (WORD_SIZE))) 
#define WIDTH_ADDR                        (ACCEL_ADDR + ( 7 * (WORD_SIZE))) 

#ifdef EMBEDDED

#define TB_READ_8(ADDR)                   (*((volatile unsigned char *) (ADDR))) 
#define TB_WRITE_8(ADDR, DATA)            *((volatile unsigned char *) (ADDR)) = (DATA)

#define TB_READ_16(ADDR)                  (*((volatile unsigned short *) (ADDR))) 
#define TB_WRITE_16(ADDR, DATA)           *((volatile unsigned short *) (ADDR)) = (DATA)

#define TB_READ_32(ADDR)                  (*((volatile unsigned int *) (ADDR))) 
#define TB_WRITE_32(ADDR, DATA)           *((volatile unsigned int *) (ADDR)) = (DATA)

#define TB_READ_64(ADDR)                  (*((volatile unsigned long *) (ADDR))) 
#define TB_WRITE_64(ADDR, DATA)           *((volatile unsigned long *) (ADDR)) = (DATA)

#define REG_READ(ADDR)                    (*((volatile unsigned long *) (ADDR)))
#define REG_WRITE(ADDR, DATA)             *((volatile unsigned long *) (ADDR)) = (DATA)

#else // HOST SystemC/Matchlib

//#define TB_READ_8(ADDR)                   (r_master.read_8(ADDR).data)
//#define TB_READ_8(ADDR)                   (r_master.small_read(ADDR, 8).data)
#define TB_READ_8(ADDR)                   (r_master.read_8((int)(long)(ADDR)).data)
#define TB_WRITE_8(ADDR, DATA)            (w_master.write_8((int)(long)(ADDR), (DATA)))

#define TB_READ_16(ADDR)                  (r_master.read_16((int)(long)(ADDR)).data)
#define TB_WRITE_16(ADDR, DATA)           (w_master.write_16((int)(long)(ADDR), (DATA)))

#define TB_READ_32(ADDR)                  (r_master.read_32((int)(long)(ADDR)).data)
#define TB_WRITE_32(ADDR, DATA)           (w_master.write_32((int)(long)(ADDR), (DATA)))

//#define TB_READ_64(ADDR)                  (r_master.single_read(ADDR).data)
//#define TB_WRITE_64(ADDR, DATA)           (w_master.single_write((ADDR), (DATA)))

#define TB_READ_64(ADDR)                  (r_master.single_read((int)(long)(ADDR)).data)
#define TB_WRITE_64(ADDR, DATA)           (w_master.single_write((int)(long)(ADDR), (DATA)))

#define REG_READ(ADDR)                    (r_master.single_read((int)(long)(ADDR)).data)
#define REG_WRITE(ADDR, DATA)             (w_master.single_write((int)(long)(ADDR), (DATA)))

#endif

#define GO                                { while (0 == REG_READ(GO_READY_ADDR)); REG_WRITE(GO_ADDR, 1); } 
#define WAIT_FOR_DONE                     { while (0 == REG_READ(DONE_VALID_ADDR)); (volatile void) REG_READ(DONE_ADDR); } 
#define SET_INPUT_OFFSET(X)               { REG_WRITE(INPUT_OFFSET_ADDR, X); } 
#define SET_OUTPUT_OFFSET(X)              { REG_WRITE(OUTPUT_OFFSET_ADDR, X); } 
#define SET_HEIGHT(X)                     { REG_WRITE(HEIGHT_ADDR, X); } 
#define SET_WIDTH(X)                      { REG_WRITE(WIDTH_ADDR, X); } 

