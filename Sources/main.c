#include "main.h"
#include <string.h>

//МАССИВЫ

#define UART_BUF_SIZE 256
#define MSG_BUF_SIZE 40
Uint8 ComPort_RX[UART_BUF_SIZE];
Uint8 ComPort_TX[UART_BUF_SIZE];
Uint8 MSG_in0[MSG_BUF_SIZE];

//стробы по полукадрам
Uint8 C10[MaxStrob];
Uint8 G10[MaxCadr];
Uint8 Cbuf[MaxStrob];
Uint8 Etalon[MaxStrob];

//RECT stack[MaxStack];
//RECT stack1[MaxStack];
RECT32 stack32[MaxStack];

Zone_type Zones[256];

XY xy;			//координаты эталона в стробе нечетный
COORDS coordsEt, coordsEt1;			//координаты эталона в стробе четный

//ПЕРЕМЕННЫЕ

MASS Mxy;			//координаты центров масс
Uint32 strobAddr; 			//адрес текущий строб
Uint32 strobAddrG; 			//адрес текущий строб
int field_index;
Uint32 colsStEt;			//столбцов в стробе эталоне
Zone_type EtZone;
Uint32 RowStrob = 128, ColStrob = 256;	//размеры строба
int XStrob = 0, YStrob = 0;	//смещение строба


//КОНСТАНТЫ
const volatile Uint32 fields[2] = {//поля во внешней памяти с кадрами
		0xD0040000,
		0xD0140000};
const Uint32 fieldsG[2] = { //поля градаций серого во внешней памяти с кадрами
		0xD0000000,
		0xD0100000};



//ФЛАГИ
volatile Uint32 fl_first_cadr;
volatile Uint32 IntHalfCadr = 0;	//сигнал о приходе полукадра
volatile Uint32 freeze = 0;			//цель потеряна


Uint8 params;


Altera_regs_write_type* Altera_regs_write = (Altera_regs_write_type*)(0xA0000000 + (0x2000 << 3));
Altera_regs_read_type* Altera_regs_read = (Altera_regs_read_type*)(0xA0000000 + (0x2000 << 3));

//АДРЕСА

Uint8 *ProtocolFromToAltera = (Uint8*)(0xA0000000 + (0x2036 << 3));
Uint8 *ProtocolInfoFromToAltera = (Uint8*)(0xA0000000 + (0x2037 << 3));

#define ArrGradN 100
Uint16 ArrGrad[ArrGradN];

typedef struct{
	Uint8 data[40];
	Uint16 size;
	Uint8 com;
	Uint8 addr;
	Uint8 flag;
}
message_type;

typedef struct{
	Uint8 address;
	Uint8 command;
	Uint16 size;
	COORDS coords;
}
BCO_state_type;

BCO_state_type MSG_out;

inline int Get_port_state(unsigned long long* port){
	int rezult = 0;
	if(port == &Altera_regs_read->_34_port_rs422) rezult = Altera_regs_read->_37_ports_info.rx_422_rdy;
	else if(port == &Altera_regs_read->_35_port_rs232) rezult = Altera_regs_read->_37_ports_info.rx_232_rdy;
	else if(port == &Altera_regs_read->_36_port_com) rezult = Altera_regs_read->_37_ports_info.rx_com_rdy;
	else if(port == &Altera_regs_read->_38_port_usb) rezult = Altera_regs_read->_37_ports_info.rx_usb_rdy;
	return rezult;
}

Uint8 ComPort_RX_cnt = 0, ComPort_TX_aim = 0, ComPort_TX_cnt = 0;

volatile  Uint32 mult = 15;

Allbus_type msg_in, msg_out;
I2C_process_type I2C_process;

int device_mode = device_mode_idle;

void neverreturns(){
	static int tresh = 200;			//порог контурирования
	unsigned long long *used_port = &Altera_regs_read->_34_port_rs422;
	Allbus_type* msg_in_ptr = (Allbus_type*)&msg_in;
	Allbus_type* msg_out_ptr = (Allbus_type*)&msg_out;
	I2C_process_type* I2C_process_ptr = (I2C_process_type*) &I2C_process;
	Params_track_type *params_track = (Params_track_type*)&params;
	params = 0;
	msg_out.address = Protocol_host_addr;

	strobAddr = (Uint32)&C10;
	strobAddrG = (Uint32)&G10;

	for(;;)
	{
//		TimerReset();
		if(IntHalfCadr == 1)//полукадр готов для копирования
		{
			IntHalfCadr = 0;
			GpioBlink(6);

			tresh = ((tresh << 4) - tresh + (Altera_regs_read->_01_Gradient_Max * mult / 100)) >> 4;
			Altera_regs_write->_01_Gradient_threshold = tresh;

			while(Get_port_state(used_port)) ComPort_RX[ComPort_RX_cnt++] = *(Uint8*)used_port;
			input_data_analyse(ComPort_RX_cnt, ComPort_RX, msg_in_ptr);

			InputProtocol(msg_in_ptr, msg_out_ptr, I2C_process_ptr);

			if(Altera_regs_read->_03_No_TV_Signal == 0) //tv signal is ok
			{
				field_index = Altera_regs_read->_0C_Cadr_counter & 1;
				switch(device_mode)
				{
					case device_mode_track: 	EDMA_channels_update(fieldsG[field_index], fields[field_index], strobAddrG, strobAddr, XStrob, YStrob, ColStrob, RowStrob); break;
					case device_mode_detect: 	EDMA_channel_update(fieldsG[field_index], strobAddrG, 0, 0, CADR_WIDTH, CADR_HEIGHT); 											break;
				}
			}
		}

		int fl_cadr_ready = EdmaEventCheck();
		switch(device_mode)
		{
		case device_mode_idle:
		//				GpioSet(12, 0);
		//				Sync = 0;
		//				freeze = 0;
		//				failtim = 0;
			break;
		case device_mode_I2C_read:
			I2C_read_byte(I2C_process_ptr);
			OutputProtocol(msg_out_ptr, I2C_process_ptr, Command_I2C_read);
			device_mode = device_mode_idle;
			break;
		case device_mode_I2C_write:
			I2C_write_byte(I2C_process_ptr);
			if(I2C_process.rezult){
				Uint8 reg_data = I2C_process.reg_data;
				I2C_read_byte(I2C_process_ptr);
				if(reg_data != I2C_process.reg_data) I2C_process.rezult = 2;
			}
			OutputProtocol(msg_out_ptr, I2C_process_ptr, Command_I2C_write);
			device_mode = device_mode_idle;
			break;
		case device_mode_track:
			if(fl_cadr_ready)
			{
				fl_cadr_ready = 0;
				if(params_track->track_OK)
				{
					GpioSet(13, 1);
					AirCont();
					GpioSet(13, 0);
					Altera_regs_write->_10_Strob_Coords = _dpack2(((YStrob + RowStrob - BorderRows) << 16) | (YStrob + BorderRows), ((XStrob + ColStrob - BorderCols) << 16) | (XStrob + BorderCols));
					Altera_regs_write->_11_Target_Center = _dpack2(Mxy.y, Mxy.x);
				}
				OutputProtocol(msg_out_ptr, I2C_process_ptr, Command_switch_on);
			}
			break;
		case device_mode_detect:
			if(fl_cadr_ready)
			{
				fl_cadr_ready = 0;
				GpioSet(13, 1);
				Detection();
				GpioSet(13, 0);
				Altera_regs_write->_10_Strob_Coords = _dpack2((Zones[1].bottom << 16) | Zones[1].top, (Zones[1].right << 16) | Zones[1].left);
				OutputProtocol(msg_out_ptr, I2C_process_ptr, Command_switch_on);
			}
			break;
		}
		ALLBUS_send((Uint8*)used_port, msg_out_ptr);
	}
}

int main(void) {
	PllInit();
	PeriphInit();
	GpioInit();
	EmifaInit();
	Edma3Init();
	IntInit();
	Timer_Init();
	I2C_Init();

	ComPort_RX_cnt = 0;
	memset(MSG_in0, 0, MSG_BUF_SIZE);
	memset(ComPort_RX, 0, UART_BUF_SIZE);
	memset(ComPort_TX, 0, UART_BUF_SIZE);

	Altera_regs_write->_00_Input_control.GrayScale_median_en = 1;
	Altera_regs_write->_00_Input_control.Contour_median_en = 1;

	Altera_regs_write->_00_Input_control.Invert_2nd_channel = 1;
	Altera_regs_write->_04_1st_Bright_max = 255;
	Altera_regs_write->_05_1st_Bright_min = 225;
	Altera_regs_write->_06_2nd_Bright_max = 255;
	Altera_regs_write->_07_2nd_Bright_min = 225;

	Altera_regs_write->_0A_ARU_drop_pixels = 1;

#pragma FUNC_NEVER_RETURNS (neverreturns);
	neverreturns();
	return 0;
}
