//#include <string.h>
#include "AllBus.h"
#define Beffer_size 256

/* длина для CRC */
#define W 16
/* побайтное*/
#define B 8

const unsigned short crctab[1<<B] = {
  0x0000,  0x1021,  0x2042,  0x3063,  0x4084,  0x50a5,  0x60c6,  0x70e7,
  0x8108,  0x9129,  0xa14a,  0xb16b,  0xc18c,  0xd1ad,  0xe1ce,  0xf1ef,
  0x1231,  0x0210,  0x3273,  0x2252,  0x52b5,  0x4294,  0x72f7,  0x62d6,
  0x9339,  0x8318,  0xb37b,  0xa35a,  0xd3bd,  0xc39c,  0xf3ff,  0xe3de,
  0x2462,  0x3443,  0x0420,  0x1401,  0x64e6,  0x74c7,  0x44a4,  0x5485,
  0xa56a,  0xb54b,  0x8528,  0x9509,  0xe5ee,  0xf5cf,  0xc5ac,  0xd58d,
  0x3653,  0x2672,  0x1611,  0x0630,  0x76d7,  0x66f6,  0x5695,  0x46b4,
  0xb75b,  0xa77a,  0x9719,  0x8738,  0xf7df,  0xe7fe,  0xd79d,  0xc7bc,
  0x48c4,  0x58e5,  0x6886,  0x78a7,  0x0840,  0x1861,  0x2802,  0x3823,
  0xc9cc,  0xd9ed,  0xe98e,  0xf9af,  0x8948,  0x9969,  0xa90a,  0xb92b,
  0x5af5,  0x4ad4,  0x7ab7,  0x6a96,  0x1a71,  0x0a50,  0x3a33,  0x2a12,
  0xdbfd,  0xcbdc,  0xfbbf,  0xeb9e,  0x9b79,  0x8b58,  0xbb3b,  0xab1a,
  0x6ca6,  0x7c87,  0x4ce4,  0x5cc5,  0x2c22,  0x3c03,  0x0c60,  0x1c41,
  0xedae,  0xfd8f,  0xcdec,  0xddcd,  0xad2a,  0xbd0b,  0x8d68,  0x9d49,
  0x7e97,  0x6eb6,  0x5ed5,  0x4ef4,  0x3e13,  0x2e32,  0x1e51,  0x0e70,
  0xff9f,  0xefbe,  0xdfdd,  0xcffc,  0xbf1b,  0xaf3a,  0x9f59,  0x8f78,
  0x9188,  0x81a9,  0xb1ca,  0xa1eb,  0xd10c,  0xc12d,  0xf14e,  0xe16f,
  0x1080,  0x00a1,  0x30c2,  0x20e3,  0x5004,  0x4025,  0x7046,  0x6067,
  0x83b9,  0x9398,  0xa3fb,  0xb3da,  0xc33d,  0xd31c,  0xe37f,  0xf35e,
  0x02b1,  0x1290,  0x22f3,  0x32d2,  0x4235,  0x5214,  0x6277,  0x7256,
  0xb5ea,  0xa5cb,  0x95a8,  0x8589,  0xf56e,  0xe54f,  0xd52c,  0xc50d,
  0x34e2,  0x24c3,  0x14a0,  0x0481,  0x7466,  0x6447,  0x5424,  0x4405,
  0xa7db,  0xb7fa,  0x8799,  0x97b8,  0xe75f,  0xf77e,  0xc71d,  0xd73c,
  0x26d3,  0x36f2,  0x0691,  0x16b0,  0x6657,  0x7676,  0x4615,  0x5634,
  0xd94c,  0xc96d,  0xf90e,  0xe92f,  0x99c8,  0x89e9,  0xb98a,  0xa9ab,
  0x5844,  0x4865,  0x7806,  0x6827,  0x18c0,  0x08e1,  0x3882,  0x28a3,
  0xcb7d,  0xdb5c,  0xeb3f,  0xfb1e,  0x8bf9,  0x9bd8,  0xabbb,  0xbb9a,
  0x4a75,  0x5a54,  0x6a37,  0x7a16,  0x0af1,  0x1ad0,  0x2ab3,  0x3a92,
  0xfd2e,  0xed0f,  0xdd6c,  0xcd4d,  0xbdaa,  0xad8b,  0x9de8,  0x8dc9,
  0x7c26,  0x6c07,  0x5c64,  0x4c45,  0x3ca2,  0x2c83,  0x1ce0,  0x0cc1,
  0xef1f,  0xff3e,  0xcf5d,  0xdf7c,  0xaf9b,  0xbfba,  0x8fd9,  0x9ff8,
  0x6e17,  0x7e36,  0x4e55,  0x5e74,  0x2e93,  0x3eb2,  0x0ed1,  0x1ef0
};

inline unsigned short Calc_crc(unsigned short crc, unsigned char *cp, unsigned short cnt, unsigned char i){ ////
  while( cnt-- )
    crc = (crc<<B) ^ crctab[(crc>>(W-B)) ^ cp[i++]];
  return( crc );
}// end: Calc_crc

int ALLBUS_recieve(unsigned char* in_buf, Allbus_type* msg, unsigned char first, unsigned char last){
  unsigned char addr, com;
  unsigned short crc_in, crc_sum;
  unsigned char i = first, j, k;
  unsigned char pocket_size, size;
  int rezult;

  addr = in_buf[i++];//Адрес получателя
  com = in_buf[i++];//Тип данных/команда
  size = (com >> 6) + ((unsigned short)in_buf[i++] << 2);//Размер поля данных
  com &= 0x3F;
  crc_in = (unsigned short)in_buf[i++];//Младший байт контрольной суммы заголовка
  crc_in |=  ((unsigned short)in_buf[i++] << 8);//Старший байт контрольной суммы заголовка
  crc_sum = Calc_crc(0, in_buf, 3, first);//подсчет контрольной суммы заголовка
  if(crc_in == crc_sum)//заголовок верный
  {
    if(size == 0)
    {
    	msg->fl_new = 1;
    	msg->address = addr;
    	msg->command = com;
    	msg->size = 0;
        rezult = i;
    }
    else
    {
      pocket_size = last - first;
      if((size + 7) <= pocket_size)//пакет пришел полностью //"5" поставил костя (было 7) по согласованию с игорем, надо будет проверить!!!
      {
          i += size;
          crc_in = (unsigned short)in_buf[i++];//Младший байт контрольной суммы пакета
          crc_in |=  ((unsigned short)in_buf[i++] << 8);//Старший байт контрольной суммы пакета
          crc_sum = Calc_crc(crc_sum, in_buf, size + 2, (unsigned char)(first + 3));//подсчет контрольной суммы поля данных
          if(crc_in == crc_sum)//пакет верный
          {
  //перенесено выше для случая пакета без данных
          	msg->fl_new = 1;
          	msg->address = addr;
          	msg->command = com;
          	msg->size = size;
            for(j = 0, k = (unsigned char)(first + 5); j < size; j++, k++) msg->data[j] = in_buf[k];
            rezult = i;
          }
          else{//ошибка в пакете
            rezult = (unsigned char)(first + 5);
          }
      }
      else rezult = -1;//пакет пришел не полностью
    }
  }
  //заголовок неверный
  else rezult = -2;
  return rezult;
}

void ALLBUS_send(unsigned char *const Addr, Allbus_type* msg)
{
	if(msg->fl_new)
	{
		msg->fl_new = 0;
		unsigned char *msg_char = (unsigned char *)&msg->address;
		volatile int transfer_size, sum, i;

		msg->header_crc = Calc_crc(0, msg_char, 3, 0);

		if(msg->size > 0)
		{
			sum = Calc_crc(0, msg_char, msg->size + 5, 0);             //подсчет контрольной суммы поля данных

			msg->data[msg->size] = (unsigned char)sum ;//Младший байт контрольной суммы поля данных
			msg->data[msg->size + 1] = (unsigned char)(sum >> 8);//Старший байт контрольной суммы поля данных

			transfer_size = msg->size + 7;
		}
		else transfer_size = 5;

		for(i = 0; i < transfer_size; i++) *Addr = *msg_char++;
	}
}

inline unsigned char Compare_counters(int last, int first, int difference){
  int larger = 0;
  if(first >= last)
  {
    if(first >= last + difference) larger = 1;
  }
  else
    if(first + Beffer_size >= last + difference) larger = 1;
  return larger;
}

void input_data_analyse(unsigned char RX_cnt, unsigned char* RX, Allbus_type* msg){
  static unsigned char slave_cnt = 0;
  while(Compare_counters(slave_cnt, RX_cnt, 5))
  {
	int result = ALLBUS_recieve(RX, msg, slave_cnt, RX_cnt);
    if(result >= 0)//сообщение получено и обработано;
    {
      slave_cnt = result;
      break;
    }
    if(result == -1) break;//пакет пришел неполностью
    if(result == -2) slave_cnt++;//заголовок неверный переход поиск заголовка
  }
}

//
//int ALLBUS_recieve(unsigned char* in_buf, unsigned char* msg, unsigned char first, unsigned char last){
//  unsigned char addr, com;
//  unsigned short crc_in, crc_sum;
//  unsigned char i = first, j, k;
//  unsigned char pocket_size, size;
//  int rezult;
//
//  addr = in_buf[i++];//Адрес получателя
//  com = in_buf[i++];//Тип данных/команда
//  size = (com >> 6) + ((unsigned short)in_buf[i++] << 2);//Размер поля данных
//  com &= 0x3F;
//  crc_in = (unsigned short)in_buf[i++];//Младший байт контрольной суммы заголовка
//  crc_in |=  ((unsigned short)in_buf[i++] << 8);//Старший байт контрольной суммы заголовка
//  crc_sum = Calc_crc(0, in_buf, 3, first);//подсчет контрольной суммы заголовка
//  if(crc_in == crc_sum)//заголовок верный
//  {
//    if(size == 0)
//    {
//        msg[0] = 1;
//        msg[1] = addr;
//        msg[2] = com;
//        msg[3] = 0;
//        msg[4] = 0;
//        rezult = i;
//    }
//    else
//    {
//      pocket_size = last - first;
//      if((size + 7) <= pocket_size)//пакет пришел полностью //"5" поставил костя (было 7) по согласованию с игорем, надо будет проверить!!!
//      {
//          i += size;
//          crc_in = (unsigned short)in_buf[i++];//Младший байт контрольной суммы пакета
//          crc_in |=  ((unsigned short)in_buf[i++] << 8);//Старший байт контрольной суммы пакета
//          crc_sum = Calc_crc(crc_sum, in_buf, size + 2, first + 3);//подсчет контрольной суммы поля данных
//          if(crc_in == crc_sum)//пакет верный
//          {
//  //перенесено выше для случая пакета без данных
//            msg[0] = 1;
//            msg[1] = addr;
//            msg[2] = com;
//            msg[3] = (unsigned char)(size >> 8);
//            msg[4] = (unsigned char)size;
//            for(j = 5, k = first + 5; j < (size + 5); j++, k++) msg[j] = in_buf[k];
//            rezult = i;
//          }
//          else{//ошибка в пакете
//            rezult = first + 5;
//          }
//      }
//      else rezult = -1;//пакет пришел не полностью
//    }
//  }
//  //заголовок неверный
//  else rezult = -2;
//  return rezult;
//}


//
//void ALLBUS_send(unsigned char *const Addr, unsigned char* Uart, unsigned int msg_addr){
//	unsigned char *msg = (unsigned char *)msg_addr;
//	volatile int transfer_size, sum, i = 0;
//
//	unsigned char addr = msg[0];
//	unsigned char com = msg[1];
//	int data_size = (((int)msg[3]) << 8) + msg[2];
//
//	Uart[i++] = addr;                                   //Адрес получателя
//	Uart[i++] = com | ((unsigned char)data_size << 6);        //Тип данных/команда
//	Uart[i++] = (unsigned char)data_size >> 2;                //Размер поля данных
//
//	sum = Calc_crc(0, Uart, 3, 0);                      //подсчет контрольной суммы заголовка
//
//	Uart[i++] = (unsigned char)sum ;                          //Младший байт контрольной суммы заголовка
//	Uart[i++] = (unsigned char)(sum >> 8);                    //Старший байт контрольной суммы заголовка
//
//	if(data_size > 0)
//	{
//		memcpy(Uart + 5, msg + 4, data_size); //копирование данных
//
//		sum = Calc_crc(sum, Uart, data_size + 2, 3);             //подсчет контрольной суммы поля данных
//
//		i += data_size;
//		Uart[i++] = (unsigned char)sum ;//Младший байт контрольной суммы поля данных
//		Uart[i] = (unsigned char)(sum >> 8);//Старший байт контрольной суммы поля данных
//
//		transfer_size = data_size + 7;
//	}
//	else transfer_size = 5;
//	for(i = 0; i < transfer_size; i++) *Addr = Uart[i];
//}
//

