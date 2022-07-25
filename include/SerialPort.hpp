//
// Created by luojunhui on 7/22/20.
//

#ifndef TRANFORM_T_SERIAL_H
#define TRANFORM_T_SERIAL_H

#include <iostream>
#include <unistd.h>     // UNIX Standard Definitions
#include <fcntl.h>      // File Control Definitions
#include <cerrno>      // ERROR Number Definitions
#include <termios.h>
#include <cstring>
#include <cstdio>


#define PC2STM32 "/dev/ttyUSB0"//串口位置


/*--------------------------------暂定协议-------------------------------------*/

//暂定 4 字节,头 1 字节,数据 2 字节,尾 1 字节
#define    SEND_LENGTH        8
#define     RECEIVE_LENGTH      3
//起始字节,协议固定为0xA5
#define    VISION_SOF         (0xA5)
//end字节,协议固定为0xA5
#define    VISION_TOF         (0xA6)

/**---------------------------------------SEND DATA PROTOCOL--------------------------------------------**/
/**    ----------------------------------------------------------------------------------------------------
FIELD  |  A5  |  POSE  |  identify  |  A6  |
       ----------------------------------------------------------------------------------------------------
BYTE   |  1  |    1    |      1     |  1   |
       ----------------------------------------------------------------------------------------------------
**/
/**---------------------------------------SEND DATA PROTOCOL--------------------------------------------**/


/**---------------------------------------RECEIVE DATA PROTOCOL-----------------------------------------------------------------------------**/
/**    -----------------------------------------------------------------------------------------------------------------------------------------
FIELD  |  A5  |  identify  |  A6  |
       ----------------------------------------------------------------------------------------------------
BYTE   |  1   |     1      |  1   |
------------------------------------------------------------------------------------------------------------------------------------------------
**/
/**---------------------------------------RECEIVE DATA PROTOCOL------------------------------------------------------------------------------**/

using namespace std;

/**
 * @brief receive data structure
 */
struct ReceiveData
{
    uint8_t flag = 0;
};

/**
 * @brief SerialPort
 * @param filename 串口名字
 * @param buadrate 串口波特率,用于stm32通信是0-B115200,用于stm32通信是1-B921600
 */
class Serial
{
private:
    int fd;
    int nSpeed;
    char nEvent;
    int nBits;
    int nStop;
    uint8_t buff[SEND_LENGTH];
    uint8_t  buffRead[100];
    uint8_t curBuf;
    int readCount;
    int maxReadTime;
    static int set_opt(int fd, int nSpeed, char nEvent, int nBits, int nStop);

public:
    explicit Serial(int nSpeed = 115200, char nEvent = 'N', int nBits = 8, int nStop = 1);
    ~Serial();
    void pack(uint8_t pose, uint8_t identify, int dx);
    bool InitPort(int nSpeed = 115200, char  nEvent = 'N', int nBits = 8, int nStop = 1);
    bool WriteData();
    bool ReadData(struct ReceiveData& buffer);
};

#endif //TRANFORM_T_SERIAL_H
