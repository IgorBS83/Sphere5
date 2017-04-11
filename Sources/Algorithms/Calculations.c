typedef unsigned int Uint32;
typedef unsigned short Uint16;
typedef unsigned char Uint8;
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "main.h"
#include <MathFuncs.h>

Uint32 cossin[180] = {
	0x3FFF0000,0x3FFD011E,0x3FF6023C,0x3FE9035A,0x3FD80477,0x3FC10594,0x3FA606B1,0x3F8507CD,0x3F6008E9,0x3F360A03,0x3F070B1D,0x3ED20C37,0x3E990D4F,0x3E5C0E66,0x3E190F7C,0x3DD11091,0x3D8511A4,0x3D3412B6,
	0x3CDE13C7,0x3C8314D6,0x3C2315E4,0x3BBF16F0,0x3B5717FA,0x3AE91902,0x3A771A08,0x3A011B0C,0x39851C0E,0x39061D0E,0x38821E0C,0x37F91F07,0x376D2000,0x36DB20F6,0x364621EA,0x35AC22DB,0x350F23CA,0x346D24B5,
	0x33C7259E,0x331D2684,0x326E2767,0x31BC2847,0x31072923,0x304D29FD,0x2F8F2AD3,0x2ECE2BA6,0x2E092C75,0x2D412D41,0x2C752E09,0x2BA62ECE,0x2AD32F8F,0x29FD304D,0x29233107,0x284731BC,0x2767326E,0x2684331D,
	0x259E33C7,0x24B5346D,0x23CA350F,0x22DB35AC,0x21EA3646,0x20F636DB,0x2000376D,0x1F0737F9,0x1E0C3882,0x1D0E3906,0x1C0E3985,0x1B0C3A01,0x1A083A77,0x19023AE9,0x17FA3B57,0x16F03BBF,0x15E43C23,0x14D63C83,
	0x13C73CDE,0x12B63D34,0x11A43D85,0x10913DD1,0x0F7C3E19,0x0E663E5C,0x0D4F3E99,0x0C373ED2,0x0B1D3F07,0x0A033F36,0x08E93F60,0x07CD3F85,0x06B13FA6,0x05943FC1,0x04773FD8,0x035A3FE9,0x023C3FF6,0x011E3FFD,
	0x00013FFF,0xFEE33FFD,0xFDC53FF6,0xFCA73FE9,0xFB8A3FD8,0xFA6D3FC1,0xF9503FA6,0xF8343F85,0xF7183F60,0xF5FE3F36,0xF4E43F07,0xF3CA3ED2,0xF2B23E99,0xF19B3E5C,0xF0853E19,0xEF703DD1,0xEE5D3D85,0xED4B3D34,
	0xEC3A3CDE,0xEB2B3C83,0xEA1D3C23,0xE9113BBF,0xE8073B57,0xE6FF3AE9,0xE5F93A77,0xE4F53A01,0xE3F33985,0xE2F33906,0xE1F53882,0xE0FA37F9,0xE001376D,0xDF0B36DB,0xDE173646,0xDD2635AC,0xDC37350F,0xDB4C346D,
	0xDA6333C7,0xD97D331D,0xD89A326E,0xD7BA31BC,0xD6DE3107,0xD604304D,0xD52E2F8F,0xD45B2ECE,0xD38C2E09,0xD2C02D41,0xD1F82C75,0xD1332BA6,0xD0722AD3,0xCFB429FD,0xCEFA2923,0xCE452847,0xCD932767,0xCCE42684,
	0xCC3A259E,0xCB9424B5,0xCAF223CA,0xCA5522DB,0xC9BB21EA,0xC92620F6,0xC8942000,0xC8081F07,0xC77F1E0C,0xC6FB1D0E,0xC67C1C0E,0xC6001B0C,0xC58A1A08,0xC5181902,0xC4AA17FA,0xC44216F0,0xC3DE15E4,0xC37E14D6,
	0xC32313C7,0xC2CD12B6,0xC27C11A4,0xC2301091,0xC1E80F7C,0xC1A50E66,0xC1680D4F,0xC12F0C37,0xC0FA0B1D,0xC0CB0A03,0xC0A108E9,0xC07C07CD,0xC05B06B1,0xC0400594,0xC0290477,0xC018035A,0xC00B023C,0xC004011E};


//#pragma DATA_SECTION (H, "cL2RAM");
Uint16 H[180][500];

//#pragma DATA_SECTION (H1, "cL2RAM");
Uint16 H1[4][500];

//#pragma DATA_SECTION (H2, "cL2RAM");
Uint16 H2[500];

int HistLine[11];

Uint16 haf(Uint32 Data, Uint16 cols, Uint16 rows, Uint16 num){
	memset(H, 0, 2*180*500);
	memset(H1, 0, 2*4*500);
	memset(H2, 0, 2*500);
	Uint16 tMax, HMax = 0, Rmax;//длина и угол перпендикулярной линии
//	Uint16  HMaxP = 0, tMaxP;
	Uint16 rRazm = 500;
	int zz, i, j;
	Uint16 t, t1, t2, t3, r, r1, r2, r3;
	Uint8* buf = (Uint8*)Data;
	for(i = 0; i < rows; i++){//первичный расчет угла
		for(j = 0; j < cols; j++)
			if((*buf++ == num) && ((j < BorderCols) || (j > cols - BorderCols))){//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				zz = _pack2(i,j);
#pragma UNROLL(5);
				for(t = 0, t1 = 15, t2 = 150, t3 = 165; t < 15; t+=3, t1+=3, t2+=3, t3+=3){
					r = (_dotp2(cossin[t], zz)>>14) + cols;	//r = ((i * cos1[t] + j * sin1[t])>>14) + cols;
					r1 = (_dotp2(cossin[t1], zz)>>14) + cols;
					r2 = (_dotp2(cossin[t2], zz)>>14) + cols;
					r3 = (_dotp2(cossin[t3], zz)>>14) + cols;
					H[t][r]++;
					H[t1][r1]++;
					H[t2][r2]++;
					H[t3][r3]++;
				}
			}
	}
	for (t = 0; t < 30; t += 3){
		for(r = 0; r < rRazm; r ++){
			if (H[t][r] > HMax) {HMax = H[t][r]; tMax = t; Rmax = r;}
		}
	}
	for (t = 150; t < 180; t += 3){
		for(r = 0; r < rRazm; r ++){
			if (H[t][r] > HMax) {HMax = H[t][r]; tMax = t; Rmax = r;}
		}
	}
	buf = (Uint8*)Data;////восстановления адреса
	Uint16 tmm2 = tMax-2, tmm1 = tMax-1, tmp1 = tMax+1, tmp2 = tMax+2;
	if(tMax == 0){
		tmm2 = 178;
		tmm1 = 179;
	}
	for(i = 0; i < rows; i++){//уточненный расчет угла
		for(j = 0; j < cols; j++){
			if(*buf++ == num && (j < 22 || j > (cols - 22))){
				zz = _pack2(i,j);
				r = (_dotp2(cossin[tmm2], zz)>>14) + cols;	//r = ((i * cos1[t] + j * sin1[t])>>14) + cols;
				r1 = (_dotp2(cossin[tmm1], zz)>>14) + cols;
				r2 = (_dotp2(cossin[tmp1], zz)>>14) + cols;
				r3 = (_dotp2(cossin[tmp2], zz)>>14) + cols;
				H1[0][r]++;
				H1[1][r1]++;
				H1[2][r2]++;
				H1[3][r3]++;
			}
		}
	}
	for(r = 0; r < rRazm; r++){
		if (H1[0][r] > HMax) {HMax = H1[0][r]; tMax = tmm2; Rmax = r;}
		if (H1[1][r] > HMax) {HMax = H1[1][r]; tMax = tmm1; Rmax = r;}
		if (H1[2][r] > HMax) {HMax = H1[2][r]; tMax = tmp1; Rmax = r;}
		if (H1[3][r] > HMax) {HMax = H1[3][r]; tMax = tmp2; Rmax = r;}
	}
	int porog = 3;
	if(HMax > 3*(22 >> 3))//удаление линии
	{
		double tpi180 = tMax * 0.017453;//	pi/180 = 0.017453
		double a = -tan(tpi180);
		double b = (Rmax - cols + 1)/cos(tpi180);
		Uint32 y;
		Uint8 *out, *out1;
		int cnt;
		int size = 5;
		memset(HistLine, 0, 11 * 4);
		for(j = 0; j < BorderCols; j++)//расчет ширины линии
		{
			y = (Uint32)(a * j + b);
			out = (Uint8*)Data + (y - size) * cols + j;
			out1 = out;
			cnt = 0;
			for(i = -size; i <= size; i++, out += cols)//проверка ширины линии
				cnt += *out;
			cnt /= 255;
			HistLine[cnt]++;
		}
		for(j = cols - BorderCols; j < cols; j++)//расчет ширины линии
		{
			y = (Uint32)(a * j + b);
			out = (Uint8*)Data + (y - size) * cols + j;
			out1 = out;
			cnt = 0;
			for(i = -size; i <= size; i++, out += cols)//проверка ширины линии
				cnt += *out;
			cnt /= 255;
			HistLine[cnt]++;
		}
//		cnt = 0;
//		for(j = 0; j < 11; j++)
//			if (HistLine[j] > cnt)
//			{
//				cnt = HistLine[j];
//				porog = j + 2;
//			}
		porog *= 255;
		j = 0;
		int done = 0;

		while(done == 0)//слеваа направо
		{
			y = (Uint32)(a * j + b);
			out = (Uint8*)Data + (y - size) * cols + j;
			out1 = out;
			cnt = 0;
			for(i = -size; i <= size; i++, out += cols)//проверка ширины линии
				cnt += *out;
			if (cnt > porog) done = 1;
			for(i = -size; i <= size; i++, out1 += cols)//удаление линии
				*out1 = 0;
			j++;
			if (j >= cols) done = 1;
		}
		done = 0;
		zz = cols - 1;
		while(done == 0)//справа налево
		{
			y = (Uint32)(a * zz + b);
			out = (Uint8*)Data + (y - size) * cols + zz;
			out1 = out;
			cnt = 0;
			for(i = -size; i <= size; i++, out += cols)//проверка ширины линии
				cnt += *out;
			if (cnt > porog) done = 1;
			for(i = -size; i <= size; i++, out1 += cols)//удаление линии
				*out1 = 0;
			zz--;
			if (zz <= j) done = 1;
		}

	}
	return porog;
}
//
//short EihgtSv(Uint8* Cadr, Uint32 rows, Uint32 cols){
//	memset(stack, 0, 1024 * sizeof(RECT));
//    int i ,j ,i1 , j1 ;
//    int ist, def;
//    Uint32 maxi = cols * rows;
//    Uint32 maxicmp = cols * (rows - 1);
//    Uint32 colscmp = cols - 1;
//    short cnt = 0, num = 255;
//    Zone_type b;
//	memset(&b, 0, sizeof(Zone_type));
//    for(i = 0; i < maxi; i += cols){
//        for(j = 0; j < cols; j++){
//            if (Cadr[i + j] == num){
//                cnt++;
//                if(cnt > 253) break;
//                ist=0;
//              	Cadr[i + j] = cnt;
//               	j1 = j;
//               	i1 = i;
//               	def = 0;
//
//                b.NumPix = 1;
//                b.left = j1;
//                b.top = i1;
//                b.right = j1;
//                b.bottom = i1;
//
//                while(ist>=0){
//					switch (def){
//						case 0:
//							if ((j1 > 0) && (Cadr[i1 + j1 - 1] == num)){//left
//									stack[ist].def = 4;
//									stack[ist].i = i1;
//									stack[ist].j = j1;
//									ist++;
//
//									j1 -= 1;
//									def = 0;
//					               	Cadr[i1 + j1] = cnt;
//
//					               	b.NumPix++;
//									if(b.left > j1) b.left = j1;
//									if(b.right < j1) b.right = j1;
//									if(b.top > i1) b.top = i1;
//									if(b.bottom < i1) b.bottom = i1;
//									break;
//							}
//						case 4:
//							if ((j1 > 0) && (i1 > 0) && (Cadr[i1 - cols + j1 - 1] == num)){//left//up
//									stack[ist].def = 5;
//									stack[ist].i = i1;
//									stack[ist].j = j1;
//									ist++;
//
//									i1 -= cols;
//									j1 -= 1;
//									def = 0;
//					               	Cadr[i1 + j1] = cnt;
//
//					               	b.NumPix++;
//									if(b.left > j1) b.left = j1;
//									if(b.right < j1) b.right = j1;
//									if(b.top > i1) b.top = i1;
//									if(b.bottom < i1) b.bottom = i1;
//									break;
//							}
//						case 5:
//							if ((j1 > 0) && (i1 < maxicmp) && (Cadr[i1 + cols + j1 - 1] == num)){//left//down
//									stack[ist].def = 6;
//									stack[ist].i = i1;
//									stack[ist].j = j1;
//									ist++;
//
//									i1 += cols;
//									j1 -= 1;
//									def = 0;
//					               	Cadr[i1 + j1] = cnt;
//
//					               	b.NumPix++;
//									if(b.left > j1) b.left = j1;
//									if(b.right < j1) b.right = j1;
//									if(b.top > i1) b.top = i1;
//									if(b.bottom < i1) b.bottom = i1;
//									break;
//							}
//						case 6:
//							if ((j1 < colscmp) && (i1 < maxicmp) && (Cadr[i1 + cols + j1 + 1] == num)){//right//down
//									stack[ist].def = 7;
//									stack[ist].i = i1;
//									stack[ist].j = j1;
//									ist++;
//
//									i1 += cols;
//									j1 += 1;
//									def = 0;
//					               	Cadr[i1 + j1] = cnt;
//
//					               	b.NumPix++;
//									if(b.left > j1) b.left = j1;
//									if(b.right < j1) b.right = j1;
//									if(b.top > i1) b.top = i1;
//									if(b.bottom < i1) b.bottom = i1;
//									break;
//							}
//						case 7:
//							if ((j1 < colscmp) && (i1 > 0) && (Cadr[i1 - cols + j1 + 1] == num)){//right//up
//									stack[ist].def = 1;
//									stack[ist].i = i1;
//									stack[ist].j = j1;
//									ist++;
//
//									i1 -= cols;
//									j1 += 1;
//									def = 0;
//					               	Cadr[i1 + j1] = cnt;
//
//					               	b.NumPix++;
//									if(b.left > j1) b.left = j1;
//									if(b.right < j1) b.right = j1;
//									if(b.top > i1) b.top = i1;
//									if(b.bottom < i1) b.bottom = i1;
//									break;
//							}
//						case 1:
//							if ((j1 < colscmp) && (Cadr[i1 + j1 + 1] == num)){//right
//									stack[ist].def = 2;
//									stack[ist].i = i1;
//									stack[ist].j = j1;
//									ist++;
//
//									j1 += 1;
//									def = 0;
//					               	Cadr[i1 + j1] = cnt;
//
//					               	b.NumPix++;
//									if(b.left > j1) b.left = j1;
//									if(b.right < j1) b.right = j1;
//									if(b.top > i1) b.top = i1;
//									if(b.bottom < i1) b.bottom = i1;
//									break;
//							}
//						case 2:
//							if ((i1 > 0) && (Cadr[i1 - cols + j1] == num)){//up
//									stack[ist].def = 3;
//									stack[ist].i = i1;
//									stack[ist].j = j1;
//									ist++;
//
//									i1 -= cols;
//									def = 0;
//					               	Cadr[i1 + j1] = cnt;
//
//					               	b.NumPix++;
//									if(b.left > j1) b.left = j1;
//									if(b.right < j1) b.right = j1;
//									if(b.top > i1) b.top = i1;
//									if(b.bottom < i1) b.bottom = i1;
//									break;
//							}
//						case 3:
//							if ((i1 < maxicmp) && (Cadr[i1 + cols + j1] == num)){//down
//									i1 += cols;
//									def = 0;
//					               	Cadr[i1 + j1] = cnt;
//
//					               	b.NumPix++;
//									if(b.left > j1) b.left = j1;
//									if(b.right < j1) b.right = j1;
//									if(b.top > i1) b.top = i1;
//									if(b.bottom < i1) b.bottom = i1;
//									break;
//							}
//							ist--;
//							j1 = stack[ist].j;
//							i1 = stack[ist].i;
//							def = stack[ist].def;
//					}
//                	if(ist >= MaxStack)
//                	{
//						ist--;
//						j1 = stack[ist].j;
//						i1 = stack[ist].i;
//						def = stack[ist].def;
//                	}
//                }
//                b.top /= cols;
//                b.bottom /= cols;
//                Zones[cnt] = b;
//    }}}
//
//    //восстановление эталона
//
//    Uint32* a = (Uint32*)Cadr;
//    for(i = 0; i < (cols * rows) >> 2; i ++)
//          *a++ = _xpnd4(_cmpgtu4(*a, 0));
//    return cnt + 0;
//}

Uint16 findObj(Uint8* Cadr, Uint32 rows, Uint32 cols, Uint32 rezult)
{
	memset(Zones, 0, 256 * sizeof(Zone_type));
//	short ZoneCnt = EihgtSv(Cadr, RowStrob, ColStrob);
	short ZoneCnt = Fill_Connectivity(Cadr, RowStrob, ColStrob);
	COORDS* rez = (COORDS*)rezult;
	int i, k, Npmax = 0;

	fill_Zones(Cadr, cols, rows, ZoneCnt);


	for(i = 1; i <= ZoneCnt; i++)
	{
		if(Zones[i].NumPix > Npmax)
		{
			Npmax = Zones[i].NumPix;
			k = i;
		}
	}
	rez->x = Zones[k].left;
	rez->y = Zones[k].top;
	rez->cols = Zones[k].right - Zones[k].left + 1;
	rez->rows = Zones[k].bottom - Zones[k].top + 1;
	return Npmax;
}

#define MIN_OBJECT_POINTS 20

Zone_type Determine_Object(Uint8* Cadr, Uint8* CAD, short cols, short rows, int j1, int i1, Uint8 Color, Uint8 cnt, Uint8 *porog){
    int num_pix_prev, fl_done;
    float num_pix_div_prev, num_pix_div;
	int color_range = 30;
	Zone_type Object, Object_rez;
	Object.NumPix = 0;
	while(Object.NumPix < MIN_OBJECT_POINTS){
		Object = fill8sv(Cadr, CAD, cols, rows, j1, i1 * cols, Color, cnt, color_range);
		color_range += 3;
	}
	num_pix_div_prev = Object.NumPix;
	Object_rez = fill8sv(Cadr, CAD, cols, rows, j1, i1 * cols, Color, cnt, color_range);
	num_pix_div_prev = (float)Object_rez.NumPix / num_pix_div_prev;
	num_pix_prev = Object_rez.NumPix;
	fl_done = 1;
	while(fl_done){
		color_range += 3;
		Object = fill8sv(Cadr, CAD, cols, rows, j1, i1 * cols, Color, cnt, color_range);
		num_pix_div = (float)Object.NumPix / num_pix_prev;
		if(num_pix_div > num_pix_div_prev) {
			fl_done = 0;
			color_range -= 3;
		}
		else{
			num_pix_prev = Object.NumPix;
			num_pix_div_prev = num_pix_div;
			Object_rez = Object;
		}
	}
	*porog = color_range;
	return Object_rez;
}

Zone_type Correct_Object(Uint8* Cadr, Uint8* CAD, short cols, short rows, int j1, int i1, Uint8 Color, Uint8 cnt, Uint8 *porog){
    float div_min, div;
	int color_range = *porog;
	Zone_type Object, Object_down;
	int i, i_rez = 2;
	Object = fill8sv(Cadr, CAD, cols, rows, j1, i1 * cols, Color, cnt, color_range + i_rez);
	div_min = fabs((float)(EtZone.NumPix - Object.NumPix) / EtZone.NumPix);
	for(i = -2; i < 2; i++){
//		if(i != 0){
			Object_down = fill8sv(Cadr, CAD, cols, rows, j1, i1 * cols, Color, cnt, color_range + i);
			div = fabs((float)(EtZone.NumPix - Object_down.NumPix) / EtZone.NumPix);
			if(div < div_min){
				div_min = div;
				Object = Object_down;
				i_rez = i;
			}
//		}
	}
//	if(div_down > div_up) {
//		*porog = color_range + 1;
//		Object = Object_up;
//	}
//	else if(Object_down.NumPix > MIN_OBJECT_POINTS) {
//		*porog = color_range - 1;
//		Object = Object_down;
//	}
	*porog = color_range + i_rez;
	return Object;
}

Zone_type findObjPerim(Uint8* Cadr, Uint8* CAD, int cols, int rows, Zone_type* Bzones, Uint16 fl_start){
    int i;

    static int color_target;
    static Uint8 color_range;
    Uint8 color_range_black, color_range_white;

    volatile int num_pix_prev, fl_done;
    static float num_pix_div_prev;
    float num_pix_div;

	Uint8 min = 255, max = 0;
	int min_i, min_j, max_i, max_j;

    if(fl_start)
    {
		for(i = 0; i < rows * cols; i ++)
		{
			if(min > Cadr[i]){
				min = Cadr[i];
				min_i = i / cols;
				min_j = i % cols;
			}
			if(max < Cadr[i]){
				max = Cadr[i];
				max_i = i / cols;
				max_j = i % cols;
			}
		}

		Bzones[1] = Determine_Object(Cadr, CAD, cols, rows, max_j, max_i, max, 200, (Uint8*)&color_range_white);
		Bzones[0] = Determine_Object(Cadr, CAD, cols, rows, min_j, min_i, min, 100, (Uint8*)&color_range_black);
		Bzones[0].SumBright = Bzones[0].NumPix * 255 - Bzones[0].SumBright;
		Bzones[0].SumBrightX = Bzones[0].InvSumBrightX;
		Bzones[0].SumBrightY = Bzones[0].InvSumBrightY;

		//проверка границ зон
		for(i = 0; i < 2; i++)
			if(Bzones[i].left <= 1 ||
					Bzones[i].top <= 1 ||
					Bzones[i].right >= cols - 3 ||
					Bzones[i].bottom >= rows - 3)
				Bzones[i].NumPix = 0xffff;

		if(Bzones[0].NumPix < Bzones[1].NumPix) {
			color_target = 0;
			color_range = color_range_black;
		}
		else if(Bzones[1].NumPix < 0xffff){
			color_target = 1;
			color_range = color_range_white;
		}
		else
		{
			Bzones[1].NumPix = 0;
			Bzones[0].NumPix = 0;
		}
    }
    else{
    	if(color_target){
			for(i = 0; i < rows * cols; i ++)
				if(max < Cadr[i]){
					max = Cadr[i];
					max_i = i / cols;
					max_j = i % cols;
				}
			Bzones[1] = Correct_Object(Cadr, CAD, cols, rows, max_j, max_i, max, 200, (Uint8*)&color_range);

			if(Bzones[1].left <= 1 ||
					Bzones[1].top <= 1 ||
					Bzones[1].right >= cols - 3 ||
					Bzones[1].bottom >= rows - 3)
				Bzones[1].NumPix = 0;

//
			Bzones[0] = fill8sv(Cadr, CAD, cols, rows, max_j, max_i * cols, max, 200, color_range);
			Bzones[1] = fill8sv(Cadr, CAD, cols, rows, max_j, max_i * cols, max, 200, color_range-1);
			Bzones[2] = fill8sv(Cadr, CAD, cols, rows, max_j, max_i * cols, max, 200, color_range+1);
			num_pix_div_prev = (float)Bzones[0].NumPix / Bzones[1].NumPix;
			num_pix_div = (float)Bzones[2].NumPix / Bzones[0].NumPix;
			if(num_pix_div > num_pix_div_prev && Bzones[1].NumPix > 50) {
				color_range--;
//				num_pix_div_prev = num_pix_div;
			}
//			num_pix_div = (float)Bzones[2].NumPix / Bzones[0].NumPix;
			if(num_pix_div <= num_pix_div_prev) {
				color_range++;
//				num_pix_div_prev = num_pix_div;
			}
    	}
    	else{
			for(i = 0; i < rows * cols; i ++)
				if(min > Cadr[i]){
					min = Cadr[i];
					min_i = i / cols;
					min_j = i % cols;
				}
//			Bzones[0] = Determine_Object(Cadr, CAD, cols, rows, min_j, min_i, min, 100, (Uint8*)&color_range_black);
//			Bzones[0] = Correct_Object(Cadr, CAD, cols, rows, min_j, min_i, min, 100, (Uint8*)&color_range);
			memset(CAD, 0, MaxCadr);
			Bzones[0] = fill8sv(Cadr, CAD, cols, rows, min_j, min_i * cols, min, 100, color_range);
			memset(CAD, 0, MaxCadr);
			Bzones[1] = fill8sv(Cadr, CAD, cols, rows, min_j, min_i * cols, min, 100, color_range-1);
			memset(CAD, 0, MaxCadr);
			Bzones[2] = fill8sv(Cadr, CAD, cols, rows, min_j, min_i * cols, min, 100, color_range+1);
			num_pix_div_prev = (float)Bzones[0].NumPix / Bzones[1].NumPix;
			num_pix_div = (float)Bzones[2].NumPix / Bzones[0].NumPix;
			if(num_pix_div > num_pix_div_prev && Bzones[1].NumPix > 50) {
				color_range--;
//				num_pix_div_prev = num_pix_div;
			}
//			num_pix_div = (float)Bzones[2].NumPix / Bzones[0].NumPix;
			if(num_pix_div <= num_pix_div_prev) {
				color_range++;
//				num_pix_div_prev = num_pix_div;
			}

			Bzones[0].SumBright = Bzones[0].NumPix * 255 - Bzones[0].SumBright;
			Bzones[0].SumBrightX = Bzones[0].InvSumBrightX;
			Bzones[0].SumBrightY = Bzones[0].InvSumBrightY;
			if(Bzones[0].left <= 1 ||
					Bzones[0].top <= 1 ||
					Bzones[0].right >= cols - 3 ||
					Bzones[0].bottom >= rows - 3)
				Bzones[0].NumPix = 0;
   	}
    	if(color_range < 30) color_range = 10;
    	if(color_range > 100) color_range = 100;

    }

//	//проверка границ зон
//	for(i = 0; i < 2; i++)
//		if(Bzones[i].left <= 1 ||
//				Bzones[i].top <= 1 ||
//				Bzones[i].right >= cols - 3 ||
//				Bzones[i].bottom >= rows - 3)
//			Bzones[i].NumPix = 0;



//	if(Bzones[i].NumPix > 100) color_range--;
//	if(Bzones[i].NumPix < 50) color_range++;
//	if(color_range > 100) color_range = 100;
//	if(color_range < 10) color_range = 10;

    return Bzones[color_target];
}
