#ifndef MODBUS_COMMON_H_
#define MODBUS_COMMON_H_

//----------------------------------------------
/* To set/reset single coil with function 5 */
//----------------------------------------------
#define MODBUS_BIT_OFF 0x0000
#define MODBUS_BIT_ON 0xFF00


//----------------------------------------------
//Modbus Protocol Constant
//----------------------------------------------
#define MB_BROADCAST_ADDRESS   0
#define MB_DEFAULT_ADDRESS     1

//Bit Access
#define MB_FC_R_COILS          01
#define MB_FC_R_DISCRETE_INPUT 02
#define MB_FC_W_SINGLE_COIL    05
#define MB_FC_W_MULTI_COILS    15

//16 bits access
#define MB_FC_R_HOLDING_R   03
#define MB_FC_R_INPUT_R     04
#define MB_FC_W_SINGLE_R    06
#define MB_FC_W_MULTI_R     16

#define MB_FC_RW_MULTI_R  23
#define MB_FC_MASKW_R     22
#define MB_FC_R_FIFO      24

//File Record Access
#define MB_FC_R_FILERECORD    20
#define MB_FCSUB_R_FILERECORD 6
#define MB_FC_W_FILERECORD    21
#define MB_FCSUB_W_FILERECORD 6

//Diagnostic
#define MB_FC_R_EXCEPTIONSTATUS  7
#define MB_FC_DIAGNOSTIC         8
#define MB_FC_GETCOMEVENTCOUNTER 11
#define MB_FC_GETCOMEVENTLOG     12
#define MB_FC_REPORTSLAVEID      17
#define MB_FC_READDEVICEID       43
#define MB_FCSUB_READDEVICEID    14

//Exceptions list
#define MB_ILLEGAL_FUNCTION     0x01
#define MB_ILLEGAL_DATA_ADDRESS 0x02
#define MB_ILLEGAL_DATA_VALUE   0x03
#define MB_SLAVE_DEVICE_FAILURE 0x04
#define MB_SLAVE_DEVICE_BUSY    0x06
#define MB_NEGATIVE_ACKNOWLEDGE 0x07
#define MB_MEMORY_PARITY_ERROR  0x08

//modbus ascii
#define MB_HEAD_ASCII        ':'     //modbus asc header  3a=58
#define MB_PRE_END_ASCII     0x0D   //modbus asc tail
#define MB_END_ASCII         0x0A   //modbus asc tail


#endif /* MODBUS_COMMON_H_ */
