#ifndef PARAMS_H_
#define PARAMS_H_
#include <csl_edma3.h>
#include "AllBus.h"

//	typedef unsigned int Uint32;
//	typedef unsigned short Uint16;
//	typedef unsigned char Uint8;

	typedef struct {
		short x;
		short y;
		short cols;
		short rows;
	}
	COORDS;

	typedef struct {
		short x;
		short y;
	}
	XY;

	typedef struct {
		float x;
		float y;
	}
	MASS;

	typedef struct {
		Uint16 NumPix;
		Uint8  Color;
		Uint8  Num;
		Uint32 SumBright;
		short left;
		short top;
		short right;
		short bottom;
		Uint32 SumBrightX;
		Uint32 InvSumBrightX;
		Uint32 SumBrightY;
		Uint32 InvSumBrightY;
	}
	Zone_type;

	typedef struct {
		Uint16 j;
		Uint16 i;
		Uint16 def;
	}
	RECT;

	typedef struct {
		Uint32 j;
		Uint32 i;
		Uint32 def;
	}
	RECT32;

	typedef struct {
		Uint8 device_addr;
		Uint8 reg_addr;
		Uint8 reg_data;
		Uint8 rezult;
	}
	I2C_process_type;

	typedef struct
	{
		Uint32 freq 		:1;
		Uint32 fl_strob		:1;
		Uint32 strob_size	:2;
		Uint32 alg_num		:2;
		Uint32 track_OK		:1;
		Uint32 track_on		:1;
	}
	Params_track_type;

	typedef struct
	{
		Uint32 freq 		:1;
		Uint32 fl_used_cam2	:1;
		Uint32 cams_mode	:2;
		Uint32 alg_num		:2;
		Uint32 axis_ok		:1;
		Uint32 detect_on	:1;
	}
	Params_detect_type;

	enum target_color
	{
		target_color_dark,
		target_color_light
	};

	enum device_modes
	{
		device_mode_idle,
		device_mode_detect,
		device_mode_track,
		device_mode_I2C_read,
		device_mode_I2C_write
	};

	enum algorithms
	{
		algorithm_contour,
		algorithm_energy1,
		algorithm_energy2,
		algorithm_correl
	};

	enum detect_modes
	{
		detect_every_cadr,
		detect_axis_ok
	};

	enum Input_Commands
	{
	    r0,
	    Command_request_version,
	    r2,
	    Command_switch_on,
	    Command_request_params,
	    Command_change_params,
	    Command_service,
	    r7,
	    r8,
	    Command_save_params,
	    Command_switch_off,
	    r11,
	    Command_I2C_read,
	    Command_I2C_write
	};


	//ФУНКЦИИ
	void PeriphInit();
	//GPIO
	int GpioInit();
	int GpioBlink(int);
	int GpioSet(int, Uint16);
	//EDMA
	int EdmaEventCheck();
	void Edma3Init();
	void EDMA_channels_update(Uint32, Uint32, Uint32, Uint32, Uint16, Uint16, Uint16, Uint16);
	void EDMA_channel_update(Uint32 src, Uint32 dst, Uint16 x, Uint16 y, Uint16 cols, Uint16 rows);



//	Uint8 I2C_write_byte(Uint8 addr, Uint8 data);
//	Uint8 I2C_read_byte(Uint8 addr, Uint8* data);
	void I2C_Init();
	void I2C_write_byte(I2C_process_type* i2c);
	void I2C_read_byte(I2C_process_type* i2c);
//	void I2C_test();

	//ПРОТОКОЛ



//	extern void CommandWrite(Uint32);
//	extern void InterpretCom();
//	extern interrupt void CommandRead();
//	extern void CommandTake();
	void InputProtocol(Allbus_type*, Allbus_type*, I2C_process_type*);
	void OutputProtocol(Allbus_type* msg_out, I2C_process_type* i2c, int command);

	//СИСТЕМНЫЕ ФУНКЦИИ
	extern void IntInit();
	extern int EmifaInit();
	extern void PllInit();
	extern void Timer_Init();
	extern void TimerReset();

	//АЛГОРИТМЫ
	void Energy(int fl_first_cadr);
	void AirCont();
	Uint16 findObj(Uint8* Cadr, Uint32 rows, Uint32 cols, Uint32 rezult);
	Uint16 haf(Uint32 Data, Uint16 cols, Uint16 rows, Uint16 num);
	Zone_type findObjPerim(Uint8* Cadr, Uint8* CAD, int cols, int rows, Zone_type* Bzones, Uint16 fl_start);
	Zone_type fill8sv(Uint8* Cadr, Uint8* CAD, short cols, short rows, int j1, int i1, Uint8 Color, Uint8 cnt, Uint8 porog);
	short Fill_Connectivity(Uint8* Cadr, Uint32 rows, Uint32 cols);
	void fill_Zones(Uint8* Cadr, short cols, short rows, int zone_amount);

	void Calc_Rcoords();
	void Haf_Circle(int);

	void Detection();

	#define Protocol_host_addr 1
	//мин и макс возможные размеры эталона
	#define CADR_WIDTH   720
	#define CADR_HEIGHT  288
	#define BorderCols 	16
	#define BorderRows 	8
	#define RowMin 		5
	#define RowMax 		128
	#define ColMin		10
	#define ColMax		256


	#define MaxStack 	0x10000
	#define MaxStrob 	0x10000//(ColMax + (BorderCols << 1) + 1) * (RowMax + (BorderRows << 1) + 1)
	#define MaxCadr 	0x40000//(ColMax + (BorderCols << 1) + 1) * (RowMax + (BorderRows << 1) + 1)

	//МАССИВЫ
	//стробы по полукадрам
	extern Uint8 C10[MaxStrob];
	extern Uint8 G10[MaxCadr];
	extern Uint8 Cbuf[MaxStrob];
	extern Uint8 Etalon[MaxStrob];
//	extern RECT stack[MaxStack];
//	extern RECT stack1[MaxStack];
	extern RECT32 stack32[MaxStack];
	extern Zone_type Zones[256];



	extern XY xy;			//координаты эталона в стробе
	extern COORDS coordsEt/*, coordsEt1*/;

	//ПЕРЕМЕННЫЕ
	extern int device_mode;

	extern Uint32 RowStrob, ColStrob;	//размеры строба
	extern int XStrob, YStrob;			//смещение строба
	extern MASS Mxy;			//координаты центров масс
	extern Uint32 colsStEt;			//столбцов в стробе эталоне
	extern Uint32 strobAddr; 			//адрес текущий строб
	extern Uint32 strobAddrG; 			//адрес текущий строб
	extern int field_index;				//номер полученного кадра
	extern Zone_type EtZone;
	extern Uint8 params;

	//КОНСТАНТЫ
	extern short UstavX, UstavY;	//уставка
	extern const volatile Uint32 fields[2]; //поля во внешней памяти с кадрами
	extern const Uint32 fieldsG[2]; //поля градаций серого во внешней памяти с кадрами

	//АДРЕСА

	typedef struct{
		int Strob_is_calculation_area	:1;
		int ARU_en						:1;
		int Contour_median_en			:1;
		int Errosion_and_Delatation_en	:1;
		int Strob_is_output_area		:1;
		int GrayScale_median_en			:1;
		int ARU_in_Strob_en				:1;
		int res2						:1;
		int TPV_en						:1;
		int Invert_1st_channel			:1;
		int Invert_2nd_channel			:1;
		int res0 						:21;
		int res1;
	}
	Altera_Input_control_type;

	typedef struct{
		Uint8 min;
		Uint8 max;
		short res0;
		int res1;
	}
	Altera_Zone_Count_type;

	typedef struct{
		Uint32 rx_422_rdy :1;
		Uint32 rx_232_rdy :1;
		Uint32 rx_com_rdy :1;
		Uint32 rx_usb_rdy :1;
		Uint32 tx_422_full :1;
		Uint32 tx_232_full :1;
		Uint32 tx_com_full :1;
		Uint32 tx_usb_full :1;
		Uint8 r0;
		Uint16 r1;
		Uint32 r2;
	}
	Altera_ports_type;

	typedef struct {
		Altera_Input_control_type _00_Input_control;
		unsigned long long _01_Gradient_threshold;
		unsigned long long r02_03[2];
		unsigned long long _04_1st_Bright_max;//зоны чет кадр
		unsigned long long _05_1st_Bright_min;//зоны нечет
		unsigned long long _06_2nd_Bright_max;
		unsigned long long _07_2nd_Bright_min;
		unsigned long long r08_09[2];
		unsigned long long _0A_ARU_drop_pixels;
		unsigned long long r0b_0f[5];
		unsigned long long _10_Strob_Coords;
		unsigned long long _11_Target_Center;
		unsigned long long r12_33[0x22];
		unsigned long long _34_port_rs422;
		unsigned long long _35_port_rs232;
		unsigned long long _36_port_com;
		unsigned long long r37;
		unsigned long long _38_port_usb;
	}
	Altera_regs_write_type;

	typedef struct {
		unsigned long long r0;
		unsigned long long _01_Gradient_Max;
		unsigned long long r2;
		unsigned long long _03_No_TV_Signal;
		Altera_Zone_Count_type _04_1st_zone_count_even;//зоны чет кадр
		Altera_Zone_Count_type _05_1st_zone_count_odd;//зоны нечет
		Altera_Zone_Count_type _06_2nd_zone_count_even;
		Altera_Zone_Count_type _07_2nd_zone_count_odd;
		unsigned long long r08_09[2];
		Altera_Zone_Count_type _0A_contour_zone_count;//число зон в контуре
		unsigned long long rb;
		unsigned long long _0C_Cadr_counter;
		unsigned long long _0D_Gradient_Sum;
		unsigned long long _0E_Gradient_Sum_Area_columns;
		unsigned long long _0F_Gradient_Sum_Area_rows;
		unsigned long long r10_33[0x24];
		unsigned long long _34_port_rs422;
		unsigned long long _35_port_rs232;
		unsigned long long _36_port_com;
		Altera_ports_type _37_ports_info;
		unsigned long long _38_port_usb;
	}
	Altera_regs_read_type;

	extern Altera_regs_write_type* Altera_regs_write;
	extern Altera_regs_read_type* Altera_regs_read;

	extern Uint8 *ProtocolFromToAltera;
	extern Uint8 *ProtocolInfoFromToAltera;

	//ФЛАГИ
	extern volatile Uint32 fl_first_cadr;
	extern volatile Uint32 IntHalfCadr;	//сигнал о приходе полукадра
	extern volatile Uint32 freeze;


#endif
