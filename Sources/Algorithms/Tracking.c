#include "main.h"
#include <string.h>
#include <MathFuncs.h>

int EtalonCorrect(Uint8* st, Uint32 rezult, Uint32 colsStEt);
XY CorrParts(Uint32, Uint32, Uint32, Uint32, float*, Uint32);
void TakeEt();

const float porog_0_90 = 255 * (1 - 0.90);
const float porog_0_65 = 255 * (1 - 0.65);
const float porog_0_40 = 255 * (1 - 0.40);

short dx, dy;
int horizonX2, horizonX1, horizonX, horizonXavg;
int d1, d2, d3;

volatile Altera_Zone_Count_type a1,a2, a3;

void AirCont()
{
	static float Kkor;
	Zone_type Curr_zone;
	Params_track_type *params_track = (Params_track_type*)&params;
	if(fl_first_cadr)
	{
		fl_first_cadr = 0;
		switch(params_track->alg_num)
		{
			case algorithm_contour:
				haf(strobAddr, ColStrob, RowStrob, 255);
				findObj((Uint8*)strobAddr, RowStrob, ColStrob, (Uint32)&coordsEt);
				TakeEt();
				break;
			case algorithm_energy1:
				Energy(1);
				break;
			case algorithm_energy2:
				EtZone = findObjPerim(G10 , Cbuf, ColStrob, RowStrob, Zones, 1);
				if(EtZone.NumPix > 0)
				{
					coordsEt.cols = EtZone.right - EtZone.left + 1;
					coordsEt.rows = EtZone.bottom - EtZone.top + 1;
					coordsEt.x = EtZone.left;
					coordsEt.y = EtZone.top;
				}
				else params_track->track_OK = 0; //Capture = 0;
				break;
			case algorithm_correl:
				coordsEt.cols = ColStrob;
				coordsEt.rows = RowStrob;
				coordsEt.x = 0;
				coordsEt.y = 0;
				TakeEt();
				break;
		}
		if(coordsEt.cols > ColMax) coordsEt.cols = ColMax;
		if(coordsEt.rows > RowMax) coordsEt.rows = RowMax;
		if(coordsEt.cols < ColMin) coordsEt.cols = ColMin;
		if(coordsEt.rows < RowMin) coordsEt.rows = RowMin;

		XStrob += coordsEt.x - BorderCols;
		if(XStrob < 0) XStrob = 0;

		YStrob += coordsEt.y - BorderRows;
		if(YStrob < 0) YStrob = 0;
	}
	else
	{

		switch(params_track->alg_num)
		{
			case algorithm_contour:
				xy = CorrParts(coordsEt.x, coordsEt.y, (Uint32)&Etalon, strobAddrG, (float*)&Kkor, colsStEt);
				if (Kkor < porog_0_90)
				{
					freeze = 0;
					coordsEt.x = xy.x;
					coordsEt.y = xy.y;
					haf(strobAddr, ColStrob, RowStrob, 255);
					EtalonCorrect((Uint8*)strobAddr,  (Uint32)&coordsEt, ColStrob);
					TakeEt();
					Mxy.x = (float)(xy.x + XStrob) + (float)coordsEt.cols / 2;
					Mxy.y = (float)(xy.y + YStrob) + (float)coordsEt.rows / 2;
				}
				else freeze++;
				break;
			case algorithm_energy1:
				Energy(0);
				break;
			case algorithm_energy2:
				Curr_zone = findObjPerim(G10 , Cbuf, ColStrob, RowStrob, Zones, 0);
				Kkor = fabs((float)(EtZone.NumPix - Curr_zone.NumPix) / EtZone.NumPix);
				if(Kkor < 0.5)
				{
					freeze = 0;
					EtZone = Curr_zone;
					RowStrob = EtZone.bottom - EtZone.top + (BorderRows << 1) + 1;
					ColStrob = EtZone.right - EtZone.left + (BorderCols << 1) + 1;
					xy.x = EtZone.left;
					xy.y = EtZone.top;
					Mxy.x = (float)EtZone.SumBrightX/EtZone.SumBright + XStrob;
					Mxy.y = (float)EtZone.SumBrightY/EtZone.SumBright + YStrob;
				}
				else freeze++;
				break;
			case algorithm_correl:
				xy = CorrParts(coordsEt.x, coordsEt.y, (Uint32)&Etalon, strobAddrG, (float*)&Kkor, colsStEt);
				if (Kkor < porog_0_90)
				{
					freeze = 0;
					Mxy.x = (float)(xy.x + XStrob) + (float)coordsEt.cols / 2;
					Mxy.y = (float)(xy.y + YStrob) + (float)coordsEt.rows / 2;
				}
				else freeze++;
				break;
		}
		//обновление координат
		if(freeze == 0)
		{
			XStrob += xy.x - BorderCols;
			if(XStrob < 0) XStrob = 0;
			if(XStrob > 719) XStrob = 719;
			if((XStrob + ColStrob) >= 720) XStrob = 720 - ColStrob - 1;

			YStrob += xy.y - BorderRows;
			if(YStrob < 0) YStrob = 0;
			if(YStrob > 287) YStrob = 287;
			if((YStrob + RowStrob) >= 288) YStrob = 288 - RowStrob - 1;
		}
		if(coordsEt.cols <= ColMin + 1 && coordsEt.rows <= RowMin + 1)  params_track->track_OK = 0;//Capture = 0;
		if(freeze > 25) params_track->track_OK = 0;//Capture = 0;
	}



//	if(Sync < 2)//начало работы, первы кадр
//	{
//		if((CadrN & 3) == 0) Sync ++;
//		if(Sync > 1)
//		{
////			Haf_Circle(1);
//			haf(strobAddr, ColStrob, RowStrob, 255);
//			findObj((Uint8*)strobAddr, RowStrob, ColStrob, (Uint32)&coordsEt);
//
//			a1 = Altera_regs_read->_04_1st_zone_count_even;
//			a2 = Altera_regs_read->_06_2nd_zone_count_even;
//			a3 = Altera_regs_read->_0A_contour_zone_count;
//
//			colsStEt = ColStrob;
//			memcpy((void*)Etalon, (void*)(CorGray == 0 ? strobAddr : strobAddrG), ColStrob * RowStrob);
//
//			if(coordsEt.cols > ColMax) coordsEt.cols = ColMax;
//			if(coordsEt.rows > RowMax) coordsEt.rows = RowMax;
//			if(coordsEt.cols < ColMin) coordsEt.cols = ColMin;
//			if(coordsEt.rows < RowMin) coordsEt.rows = RowMin;
//
//			XStrob += coordsEt.x - BorderCols;
//			if(XStrob < 0) XStrob = 0;
//
//			YStrob += coordsEt.y - BorderRows;
//			if(YStrob < 0) YStrob = 0;
//
//			RowStrob = coordsEt.rows + (BorderRows << 1);
//			ColStrob = coordsEt.cols + (BorderCols << 1);
//		}
//	}
//	else
//	{
////		Haf_Circle(0);
//		haf(strobAddr, ColStrob, RowStrob, 255);
//		xy = CorrParts(coordsEt.x, coordsEt.y, (Uint32)&Etalon, (CorGray == 0 ? strobAddr : strobAddrG), (float*)&Kkor, colsStEt);
//
//		if (Kkor < porog_0_90)
//		{
//			freeze = 0;
//			coordsEt.x = xy.x;
//			coordsEt.y = xy.y;
////			if(field_index)
//				EtalonCorrect((Uint8*)strobAddr,  (Uint32)&coordsEt);
//			TakeEt();
//			Mxy.x = (float)(xy.x + XStrob) + (float)coordsEt.cols / 2;
//			Mxy.y = (float)(xy.y + YStrob) + (float)coordsEt.rows / 2;
//			//обновление координат
//			XStrob += xy.x - BorderCols;
//			if(XStrob < 0) XStrob = 0;
//			if(XStrob > 719) XStrob = 719;
//			if((XStrob + ColStrob) >= 720) XStrob = 720 - ColStrob - 1;
//
//			YStrob += xy.y - BorderRows;
//			if(YStrob < 0) YStrob = 0;
//			if(YStrob > 287) YStrob = 287;
//			if((YStrob + RowStrob) >= 288) YStrob = 288 - RowStrob - 1;
//		}
//		else if(freeze++ > 20) Capture = 0;
//		if(coordsEt.cols <= ColMin + 1 && coordsEt.rows <= RowMin + 1)  Capture = 0;
//
//
//	}
}

void TakeEt()
{
	colsStEt = ColStrob;
	memcpy((void*)Etalon, (void*)strobAddrG, ColStrob * RowStrob);
	RowStrob = coordsEt.rows + (BorderRows << 1) + 1;
	ColStrob = coordsEt.cols + (BorderCols << 1) + 1;
	xy.x = coordsEt.x;
	xy.y = coordsEt.y;
}

XY CorrParts(Uint32 smx, Uint32 smy, Uint32 et, Uint32 st, float* RezKkor, Uint32 colsStEt)
{
	XY xyrez;
	Uint32 stepx = 1;
	Uint32 stepy = 1;
	if(ColStrob > 128) stepx = 2;
//	if(ColStrob > 192) stepx = 5;
//	if(RowStrob > 64) stepy = 2;
//	if(RowStrob > 96) stepy = 2;
	*RezKkor = PartFastCorrFullFromPrevSt0(et, smy, smx, 0, 0, st, coordsEt.cols, coordsEt.rows,  colsStEt, ColStrob, RowStrob, (Uint32)&xyrez, BorderCols, BorderRows, stepx, stepy);
	return xyrez;
}

int EtalonCorrect(Uint8* st, Uint32 rezult, Uint32 colsStEt)
{
	int cordone = 0;
	Uint16* rez = (Uint16*)rezult;
	Uint16 smy = rez[1];
	Uint16 smx = rez[0];
	Uint16 colsEt = rez[2];
	Uint16 rowsEt = rez[3];
	Uint32 cnt;

	Uint32 NPixRowFirst = 0; //число ненулевых пикселей на 1-й строке
	Uint32 NPixRowFirstNext = 0; //число ненулевых пикселей на 1-й строке
	Uint32 NPixRowFirstPrev = 0; //число ненулевых пикселей на 1-й строке
	Uint32 NPixRowFirstMax = 0; //число ненулевых пикселей на 1-й строке
	Uint32 NPixRowFirstMin = 0; //число ненулевых пикселей на 1-й строке

	Uint32 NPixRowLast = 0; //число ненулевых пикселей на послдней строке
	Uint32 NPixRowLastPrev = 0; //число ненулевых пикселей на послдней строке
	Uint32 NPixRowLastNext = 0; //число ненулевых пикселей на послдней строке
	Uint32 NPixRowLastMin = 0; //число ненулевых пикселей на послдней строке
	Uint32 NPixRowLastMax = 0; //число ненулевых пикселей на послдней строке

	Uint8* RowFirst = st + smy * colsStEt + smx;
	Uint8* RowFirstPrev = st + (smy - 1) * colsStEt + smx;
	Uint8* RowFirstNext = st + (smy + 1) * colsStEt + smx;

	Uint8* RowLast = RowFirst + (rowsEt - 1)* colsStEt;
	Uint8* RowLastPrev = RowFirst + (rowsEt - 2)* colsStEt;
	Uint8* RowLastNext = RowFirst + (rowsEt)* colsStEt;


	Uint8* ColFirst = RowFirst;
	Uint8* ColFirstNext = RowFirst + 1;
	Uint8* ColFirstPrev = RowFirst - 1;

	Uint8* ColLast = ColFirst + colsEt - 1;
	Uint8* ColLastNext = ColFirst + colsEt;
	Uint8* ColLastPrev = ColFirst + colsEt - 2;

	Uint32 NPixColFirst = 0; //число ненулевых пикселей на 1-м столбце
	Uint32 NPixColFirstMin = 0; //число ненулевых пикселей на 1-м столбце
	Uint32 NPixColFirstMax = 0; //число ненулевых пикселей на 1-м столбце
	Uint32 NPixColFirstPrev = 0; //число ненулевых пикселей на 1-м столбце
	Uint32 NPixColFirstNext = 0; //число ненулевых пикселей на 1-м столбце

	Uint32 NPixColLast = 0; //число ненулевых пикселей на послднем столбце
	Uint32 NPixColLastMin = 0; //число ненулевых пикселей на послднем столбце
	Uint32 NPixColLastMax = 0; //число ненулевых пикселей на послднем столбце
	Uint32 NPixColLastNext = 0; //число ненулевых пикселей на послднем столбце
	Uint32 NPixColLastPrev = 0; //число ненулевых пикселей на послднем столбце

	for(cnt = 0; cnt < colsEt; cnt++)//строки
	{
		NPixRowLast += *RowLast++;
		NPixRowLastNext += *RowLastNext++;
		NPixRowLastPrev += *RowLastPrev++;

		NPixRowFirst += *RowFirst++;
		NPixRowFirstPrev += *RowFirstPrev++;
		NPixRowFirstNext += *RowFirstNext++;
	}

	NPixRowFirstMin = NPixRowFirst;
	if(NPixRowFirstMin < NPixRowFirstPrev) NPixRowFirstMin = NPixRowFirstPrev;
	if(NPixRowFirstMin < NPixRowFirstNext) NPixRowFirstMin = NPixRowFirstNext;

	NPixRowFirstMax = NPixRowFirst;
	if(NPixRowFirstMax > NPixRowFirstPrev) NPixRowFirstMax = NPixRowFirstPrev;
	if(NPixRowFirstMax > NPixRowFirstNext) NPixRowFirstMax = NPixRowFirstNext;

	NPixRowLastMin = NPixRowLast;
	if(NPixRowLastMin < NPixRowLastPrev) NPixRowLastMin = NPixRowLastPrev;
	if(NPixRowLastMin < NPixRowLastNext) NPixRowLastMin = NPixRowLastNext;

	NPixRowLastMax = NPixRowLast;
	if(NPixRowLastMax > NPixRowLastPrev) NPixRowLastMax = NPixRowLastPrev;
	if(NPixRowLastMax > NPixRowLastNext) NPixRowLastMax = NPixRowLastNext;

	for(cnt = 0; cnt < rowsEt; cnt++)//столбцы
	{
		NPixColLast += *ColLast;
		NPixColLastNext += *ColLastNext;
		NPixColLastPrev += *ColLastPrev;

		NPixColFirst += *ColFirst;
		NPixColFirstNext += *ColFirstNext;
		NPixColFirstPrev += *ColFirstPrev;

		ColLast += colsStEt;
		ColLastNext += colsStEt;
		ColLastPrev += colsStEt;

		ColFirst += colsStEt;
		ColFirstNext += colsStEt;
		ColFirstPrev += colsStEt;
	}

	NPixColLastMin = NPixColLast;
	if(NPixColLastMin < NPixColLastPrev) NPixColLastMin = NPixColLastPrev;
	if(NPixColLastMin < NPixColLastNext) NPixColLastMin = NPixColLastNext;

	NPixColLastMax = NPixColLast;
	if(NPixColLastMax > NPixColLastPrev) NPixColLastMax = NPixColLastPrev;
	if(NPixColLastMax > NPixColLastNext) NPixColLastMax = NPixColLastNext;

	NPixColFirstMin = NPixColFirst;
	if(NPixColFirstMin < NPixColFirstPrev) NPixColFirstMin = NPixColFirstPrev;
	if(NPixColFirstMin < NPixColFirstNext) NPixColFirstMin = NPixColFirstNext;

	NPixColFirstMax = NPixColFirst;
	if(NPixColFirstMax > NPixColFirstPrev) NPixColFirstMax = NPixColFirstPrev;
	if(NPixColFirstMax > NPixColFirstNext) NPixColFirstMax = NPixColFirstNext;

	if(rowsEt < RowMax)//расширение по вертикали 255*3 = 765
		if((NPixRowLastMin > 510) && ((smy + rowsEt) < RowStrob)) rowsEt++;	//вниз
	if(rowsEt < RowMax)//расширение по вертикали 255*3 = 765
		if((NPixRowFirstMin > 510) && (smy > 0)) {rowsEt++; smy--; cordone = 1;}		//вверх

	if(colsEt < ColMax)//расширение по горизонтали
		if((NPixColLastMin > 510) && ((smx + colsEt) < ColStrob)) colsEt++;	//вниз
	if(colsEt < ColMax)//расширение по горизонтали
		if((NPixColFirstMin > 510) && (smx > 0)) {colsEt++; smx--;cordone = 1;}		//вверх

	if(rowsEt > RowMin)//сужение по вертикали
		if(NPixRowLastMax <= 510) {rowsEt--;cordone = 1;}				//вниз
	if(rowsEt > RowMin)//сужение по вертикали
		if(NPixRowFirstMax <= 510) {rowsEt--; smy++;cordone = 1;}	//вверх

	if(colsEt > ColMin)//сужение по горизонтали
		if(NPixColLastMax <= 510) {colsEt--;cordone = 1;}				//вниз
	if(colsEt > ColMin)//сужение по горизонтали
		if(NPixColFirstMax <= 510) {colsEt--; smx++;cordone = 1;}	//вверх


	rez[1] = smy;
	rez[0] = smx;
	rez[2] = colsEt;
	rez[3] = rowsEt;
	return cordone;
}

//int EtalonCorrect(Uint8* st, Uint32 rezult)
//{
//	int cordone = 0;
//	Uint16* rez = (Uint16*)rezult;
//	Uint16 smy = rez[1];
//	Uint16 smx = rez[0];
//	Uint16 colsEt = rez[2];
//	Uint16 rowsEt = rez[3];
//	Uint32 cnt;
//
//#define PorogInc (3 * 255)
//#define PorogDec (3 * 255)
//
//	Uint32 NPixRowFirst = 0; //число ненулевых пикселей на 1-й строке
//	Uint32 NPixRowLast = 0; //число ненулевых пикселей на послдней строке
//
//	Uint8* RowFirst = st + smy * ColStrob + smx;
//	volatile Uint8* RowLast = RowFirst + (rowsEt - 1)* ColStrob;
//
//	Uint8* ColFirst = RowFirst;
//	Uint8* ColLast = RowFirst + colsEt - 1;
//
//	Uint32 NPixColFirst = 0; //число ненулевых пикселей на 1-м столбце
//	Uint32 NPixColLast = 0; //число ненулевых пикселей на послднем столбце
//
//	for(cnt = 0; cnt < colsEt; cnt++)//строки
//	{
//		NPixRowLast += RowLast[cnt];
//		NPixRowFirst += RowFirst[cnt];
//	}
//
//	for(cnt = 0; cnt < rowsEt; cnt++)//столбцы
//	{
//		NPixColLast += *ColLast;
//		NPixColFirst += *ColFirst;
//		ColLast += ColStrob;
//		ColFirst += ColStrob;
//	}
//
//	if(rowsEt < RowMax - 1){
//		if(NPixRowLast > PorogInc)  rowsEt++;
//	}
//	if(rowsEt > RowMin + 1){
//		if(NPixRowLast < PorogDec)  rowsEt--;
//	}
//	if(rowsEt < RowMax - 1){
//		if(NPixRowFirst > PorogInc) {rowsEt++; smy--;}
//	}
//	if(rowsEt > RowMin + 1){
//		if(NPixRowFirst < PorogDec) {rowsEt--; smy++;}
//	}
//
//	if(colsEt < ColMax - 1){
//		if(NPixColFirst > PorogInc)  {colsEt++; smx--;}
//	}
//	if(colsEt > ColMin + 1){
//		if(NPixColFirst < PorogDec) {colsEt--; smx++;}
//	}
//	if(colsEt < ColMax - 1){
//		if(NPixColLast > PorogInc)  {colsEt++;}
//	}
//	if(colsEt > ColMin + 1){
//		if(NPixColLast < PorogDec)  {colsEt--;}
//	}
//
//	rez[1] = smy;
//	rez[0] = smx;
//	rez[2] = colsEt;
//	rez[3] = rowsEt;
//	return cordone;
//}
