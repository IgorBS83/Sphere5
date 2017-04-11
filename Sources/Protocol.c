#include "main.h"
#include <string.h>

//Uint32 FlagCommand = 0;		//флаг команды
//Uint32 DataNum = 0;			//кол-во слов данных в посылке
//Uint32 DataCount = 0;		//счетчик слов данных в посылке
//Uint8 ComData[31];			//флаг команды
//Uint8 CheckSum = 0;			//контрольная сумма
//Uint8 Comtmp;

#define ADDR_track 	7
#define ADDR_detect 6

volatile int current_addr;

void InputProtocol(Allbus_type* msg_in, Allbus_type* msg_out, I2C_process_type* i2c){
	if(msg_in->fl_new)
	{
		msg_in->fl_new = 0;
		if(msg_in->address == ADDR_track)
		{
			Altera_regs_write->_00_Input_control.ARU_en = 1;
			Params_track_type *params_track = (Params_track_type*)&params;
			switch(msg_in->command){//command
			case Command_switch_on:
				if(device_mode == device_mode_idle)
				{
					device_mode = device_mode_track;
					fl_first_cadr = 1;
					params_track->track_on = 1;
					params_track->track_OK = 1;
					switch(params_track->strob_size)
					{
					case 0:
						ColStrob = 256;
						RowStrob = 128;
						XStrob = (CADR_WIDTH - ColStrob) >> 1;
						YStrob = (CADR_HEIGHT - RowStrob) >> 1;
						break;
					case 1:
						ColStrob = 128;
						RowStrob = 64;
						XStrob = (CADR_WIDTH - ColStrob) >> 1;
						YStrob = (CADR_HEIGHT - RowStrob) >> 1;
						break;
					case 2:
						ColStrob = 64;
						RowStrob = 32;
						XStrob = (CADR_WIDTH - ColStrob) >> 1;
						YStrob = (CADR_HEIGHT - RowStrob) >> 1;
						break;
					case 3:
						ColStrob = 32;
						RowStrob = 16;
						XStrob = (CADR_WIDTH - ColStrob) >> 1;
						YStrob = (CADR_HEIGHT - RowStrob) >> 1;
						break;
					}
				}

				break;
			case Command_switch_off:
				device_mode = device_mode_idle;
				params_track->track_on = 0;
				params_track->track_OK = 0;
				OutputProtocol(msg_out, i2c, Command_switch_off);
				break;
			case Command_change_params:
				if(device_mode == device_mode_idle)
				{
					Params_track_type* params_loc = (Params_track_type*)&msg_in->data[0];
					params_track->alg_num 		= params_loc->alg_num;
					params_track->fl_strob 	= 1;//params_loc->fl_strob;
					params_track->freq 		= 1;//params_loc->freq;
					params_track->strob_size 	= params_loc->strob_size;
					OutputProtocol(msg_out, i2c, Command_change_params);
				}
				break;
			}
		}
		if(msg_in->address == ADDR_detect)
		{
			Altera_regs_write->_00_Input_control.ARU_en = 0;
			Params_detect_type *params_detect = (Params_detect_type*)&params;
			Params_detect_type* params_loc = (Params_detect_type*)&msg_in->data[0];
			switch(msg_in->command)
			{
			case Command_switch_on:
				if(device_mode == device_mode_idle)
				{
					device_mode = device_mode_detect;
					params_detect->detect_on = 1;
				}
				break;
			case Command_switch_off:
				device_mode = device_mode_idle;
				params_detect->detect_on = 0;
				OutputProtocol(msg_out, i2c, Command_switch_off);
				break;
			case Command_change_params:
				params_detect->axis_ok 		= params_loc->axis_ok;
				if(device_mode == device_mode_idle)
				{
					params_detect->alg_num 		= params_loc->alg_num;
					params_detect->fl_used_cam2 = 1;//params_loc->fl_used_cam2;
					params_detect->cams_mode 	= 2;//params_loc->cams_mode;
					params_detect->freq 		= 1;//params_loc->freq;
					OutputProtocol(msg_out, i2c, Command_change_params);
				}
				break;
			}
		}
		if(msg_in->address == ADDR_detect || msg_in->address == ADDR_track)
		{
			current_addr = msg_in->address;
			switch(msg_in->command){//command
			case Command_request_version:
				OutputProtocol(msg_out, i2c, Command_request_version);
				break;
			case Command_request_params:
				if(device_mode == device_mode_idle)
					OutputProtocol(msg_out, i2c, Command_request_params);
				break;
			case Command_service:
				break;
			case Command_save_params:
				break;
			case Command_I2C_read:
				device_mode = device_mode_I2C_read;
				i2c->device_addr = msg_in->data[0];
				i2c->reg_addr = msg_in->data[1];
				break;
			case Command_I2C_write:
				device_mode = device_mode_I2C_write;
				i2c->device_addr = msg_in->data[0];
				i2c->reg_addr = msg_in->data[1];
				i2c->reg_data = msg_in->data[2];
				break;
			}
		}
	}
}

void OutputProtocol(Allbus_type* msg_out, I2C_process_type* i2c, int command)
{
	short val;
	msg_out->fl_new = 1;
	msg_out->command = command;
	if(current_addr == ADDR_track)
	{
		switch(command)
		{
		case Command_switch_on:
		case Command_change_params:
		case Command_request_params:
			msg_out->size = 5;
			msg_out->data[0] = params;
			val = (short)(Mxy.x * 0xffff / CADR_WIDTH) - 0x7fff;
			msg_out->data[1] = val;
			msg_out->data[2] = val >> 8;
			val = (short)(Mxy.y * 0xffff / CADR_HEIGHT) - 0x7fff;
			msg_out->data[3] = val;
			msg_out->data[4] = val >> 8;
			break;
		}
	}
	if(current_addr == ADDR_detect)
	{
		Params_detect_type *params_detect = (Params_detect_type*)&params;
		int i, zone_width, zone_height, zones_amount, fl_send_zones;
		switch(command)
		{
		case Command_switch_on:
		case Command_change_params:
		case Command_request_params:
			msg_out->data[0] = params;
			msg_out->data[1] = Zones[0].Num;
			msg_out->size = 2;
			fl_send_zones = 0;
			if(params_detect->alg_num == 1 && params_detect->axis_ok) fl_send_zones = 1;
			if(params_detect->alg_num == 0)  fl_send_zones = 1;
			if(fl_send_zones)
			{

				zones_amount = Zones[0].Num > 30 ? 30 : Zones[0].Num;
				int zone_idx = 2;
				for(i = 1; i <= zones_amount; i++)
				{
					msg_out->data[zone_idx] = 1;//tpv channel detected target
					val = (short)((Zones[i].right + Zones[i].left) * 0x7fff / CADR_WIDTH) - 0x7fff;
					msg_out->data[zone_idx + 1] = val;
					msg_out->data[zone_idx + 2] = val >> 8;
					val = (short)((Zones[i].top + Zones[i].bottom) * 0x7fff / CADR_HEIGHT) - 0x7fff;
					msg_out->data[zone_idx + 3] = val;
					msg_out->data[zone_idx + 4] = val >> 8;
					zone_width = Zones[i].right - Zones[i].left;
					zone_height = Zones[i].bottom - Zones[i].top;
					msg_out->data[zone_idx + 5] = zone_width > zone_height ? zone_width : zone_height;//Zones[i].NumPix >> 2;//
					zone_idx += 6;
				}
				msg_out->size += zones_amount * 6;
			}
			break;
		}
	}
	if(current_addr == ADDR_detect || current_addr == ADDR_track)
	{
		switch(command)
		{
		case Command_I2C_read:
			msg_out->size = 2;
			msg_out->data[0] = i2c->rezult;
			msg_out->data[1] = i2c->reg_data;
			break;
		case Command_I2C_write:
			msg_out->size = 1;
			msg_out->data[0] = i2c->rezult;
			msg_out->data[1] = i2c->reg_data;
			break;
		case Command_switch_off:
			msg_out->size = 0;
			break;
		}
	}
}
