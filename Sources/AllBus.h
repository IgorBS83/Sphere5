#define ALLBUS_MESSAGE_SIZE 100
typedef struct
{
	unsigned char  fl_new;
	unsigned char  address;
	unsigned int command 	:6;
	unsigned int size		:10;
	unsigned short header_crc;
	unsigned char data[ALLBUS_MESSAGE_SIZE];
}
Allbus_type;

void input_data_analyse(unsigned char RX_cnt, unsigned char* RX, Allbus_type* msg);
void ALLBUS_send(unsigned char *const Addr, Allbus_type* msg);
