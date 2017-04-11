#include "main.h"
#include <string.h>
#include <math.h>
#include <MathFuncs.h>

#define ZONE_MIN_PIX_AMOUNT 50

int ZoneCheck(Zone_type);
Zone_type AddZone(Zone_type, Zone_type);
Zone_type TakeZone2(int, Zone_type*);
Zone_type FindZone2(int, Zone_type*);



void Energy(int fl_first_cadr)
{
	static int target_color;
	volatile Uint16  NumZone_light, NumZone_dark;
	Zone_type *Zones_light, *Zones_dark;
	Params_track_type *params_track = (Params_track_type*)&params;

	if(field_index)
	{
		NumZone_light = *(Uint16*)(0xA0000000 + (0x2005 << 3)) - 1;
		NumZone_dark = 	*(Uint16*)(0xA0000000 + (0x2007 << 3)) - 1;
		Zones_light =  	(Zone_type*)(0xA0000000 + (1 << (12 + 3)) + (1 << (10 + 3)));
		Zones_dark =  	(Zone_type*)(0xA0000000 + (1 << (12 + 3)) + (1 << (11 + 3)));
	}
	else
	{
		NumZone_light = *(Uint16*)(0xA0000000 + (0x2004 << 3)) - 1;
		NumZone_dark = 	*(Uint16*)(0xA0000000 + (0x2006 << 3)) - 1;
		Zones_light =  	(Zone_type*)(0xA0000000 + (1 << (10 + 3)));
		Zones_dark =  	(Zone_type*)(0xA0000000 + (1 << (11 + 3)));
	}

	if(fl_first_cadr)
	{
		//прямая картинка
		Zone_type zone_light = TakeZone2(NumZone_light, Zones_light);
		//инверсная картинка
		Zone_type zone_dark = TakeZone2(NumZone_dark, Zones_dark);

		if(zone_dark.NumPix > zone_light.NumPix)
		{
			EtZone = zone_dark;
			target_color = target_color_dark;
		}
		else
		{
			EtZone = zone_light;
			target_color = target_color_light;
		}
	}
	else
	{
		Zone_type  Zone1;
		if(target_color == target_color_light) Zone1 = FindZone2(NumZone_light, Zones_light);
		else 								   Zone1 = FindZone2(NumZone_dark, Zones_dark);

		if(Zone1.NumPix > 0) {
			EtZone = Zone1;

			Mxy.x = (float)EtZone.SumBrightX/EtZone.SumBright;
			Mxy.y = (float)EtZone.SumBrightY/EtZone.SumBright;

			XStrob = EtZone.left - BorderCols;
			YStrob = EtZone.top - BorderRows;
			ColStrob = EtZone.right - EtZone.left + (BorderCols << 1) + 1;
			RowStrob = EtZone.bottom - EtZone.top + (BorderRows << 1) + 1;

			if(XStrob < 0) XStrob = 0;
			if(YStrob < 0) YStrob = 0;
			if(XStrob > 719) XStrob = 719;
			if(YStrob > 287) YStrob = 287;

			if(XStrob + ColStrob > 719) ColStrob = 720 - XStrob;
			if(YStrob + RowStrob > 287) RowStrob = 288 - YStrob;
		}
		else
		{
			freeze++;
			if(freeze > 25)
			{
				freeze = 0;
				params_track->track_OK = 0;//цель потеряна
			}
		}
	}
}

Zone_type TakeZone2(int Zone_amount, Zone_type *Zones)
{
	Zone_type Zone, Zone1;
	int i;
	memset((Uint32*)&Zone1, 0, sizeof(Zone_type));

	for(i = 0; i < Zone_amount; i++)
	{
		Zone = Zones[i];
		if (ZoneCheck(Zone) > 0)
			if((Zone.left >= XStrob + 1) && (Zone.right <= XStrob + ColStrob - 1) && (Zone.top >= YStrob + 1) && (Zone.bottom <= YStrob + RowStrob - 1))
			//накопление зон
				if(Zone1.NumPix < Zone.NumPix) Zone1 = Zone;
	}
	return Zone1;
}

Zone_type FindZone2(int Zone_amount, Zone_type *Zones)
{
	Zone_type Zone, Zone1;
	int i;
	float K_kor = 0, K_kor_rez = 0.5;
	memset((Uint32*)&Zone1, 0, sizeof(Zone_type));


	for(i = 0; i < Zone_amount; i++)
	{
		Zone = Zones[i];
		if (ZoneCheck(Zone) > 0)
			if((Zone.left >= XStrob + 1) && (Zone.right <= XStrob + ColStrob - 1) && (Zone.top >= YStrob + 1) && (Zone.bottom <= YStrob + RowStrob - 1)){
				K_kor = fabs((float)(EtZone.NumPix - Zone.NumPix) / EtZone.NumPix);
				if(K_kor < K_kor_rez){
					K_kor_rez = K_kor;
					Zone1 = Zone;
				}
			}
	}
	return Zone1;

}

int ZoneCheck(Zone_type z)
{
	volatile int rez = 1;
	if((z.NumPix < ZONE_MIN_PIX_AMOUNT) ||
		(z.left > 720) ||
		(z.right > 720) ||
		(z.top > 288) ||
		(z.bottom > 288)) rez = 0;
	return rez;
}

