#include "main.h"
#include <stdlib.h>
#include <string.h>


inline void maximum8(Uint8* max, Uint8 var){
	int d = *max - var;
	int m = d >> 31;
	*max -= (Uint8)(d & m);
}
inline void minimum8(Uint8* min, Uint8 var){
	int d = var - *min;
	int m = d >> 31;
	*min += (Uint8)(d & m);
}
inline void maximum16(Uint16* max, Uint16 var){
	int d = *max - var;
	int m = d >> 31;
	*max -= (Uint16)(d & m);
}
//inline void minimum16(Uint16* min, Uint16 var){
//	int d = var - *min;
//	int m = d >> 31;
//	*min += (Uint16)(d & m);
//}

#define CADR_WIDTH   720
#define CADR_HEIGHT  288
#define PSI_WIDTH  	 36
#define PSI_HEIGHT 	 18
#define DROP_PIXELS_BORDER 4
#define HIST_LEVELS_N 16
#define PSI_FIELD_WIDTH  (720 / PSI_WIDTH)
#define PSI_FIELD_HEIGHT (288  / PSI_HEIGHT)
#define MIN_ZONE_PIX 5

void Calc_Ranges(Uint8* Cadr_field, Uint8* Ranges_field_ptr)
{
	int psi_row, psi_col, cadr_row, cadr_col, ranges_cnt;
	Uint8 *psi_min_index, *psi_max_index;

	Uint8 Psi_min[PSI_FIELD_WIDTH * PSI_FIELD_HEIGHT];
	Uint8 Psi_max[PSI_FIELD_WIDTH * PSI_FIELD_HEIGHT];

	memset(Psi_min, 0xff, sizeof(Psi_min));
	memset(Psi_max, 0	, sizeof(Psi_max));

	for(cadr_row = DROP_PIXELS_BORDER; cadr_row < CADR_HEIGHT - DROP_PIXELS_BORDER; cadr_row++)
	{
		psi_row = cadr_row / PSI_HEIGHT;
		psi_min_index = Psi_min + psi_row * PSI_FIELD_WIDTH;
		psi_max_index = Psi_max + psi_row * PSI_FIELD_WIDTH;
//		Uint8* cadr_index = Cadr_field + cadr_row * CADR_WIDTH + DROP_PIXELS_BORDER;
		Uint32 *cadr_index = (Uint32*)(Cadr_field + cadr_row * CADR_WIDTH + DROP_PIXELS_BORDER);
		for(psi_col = 0; psi_col < PSI_FIELD_WIDTH; psi_col++)
		{
			int left_border, right_border;
			if(psi_col == 0) left_border = DROP_PIXELS_BORDER;
			else left_border = 0;
			if(psi_col == PSI_FIELD_WIDTH - 1) right_border = DROP_PIXELS_BORDER;
			else right_border = 0;
//			for(cadr_col = left_border; cadr_col < PSI_WIDTH - right_border; cadr_col++)
//			{
//				maximum8(psi_max_index, *cadr_index);
//				minimum8(psi_min_index, *cadr_index);
//				cadr_index++;
//			}
			Uint32 psi_min32 = 0xffffffff, psi_max32 = 0;
			for(cadr_col = left_border; cadr_col < PSI_WIDTH - right_border; cadr_col+=4)
			{
				psi_min32 = _minu4(psi_min32, *cadr_index);
				psi_max32 = _maxu4(psi_max32, *cadr_index);
				cadr_index++;
			}
			int i;
			for(i = 0; i < 4; i++)
			{
				maximum8(psi_max_index, psi_max32 >> (i * 8));
				minimum8(psi_min_index, psi_min32 >> (i * 8));
			}
			psi_min_index++;
			psi_max_index++;
		}
	}
	psi_min_index = Psi_min;
	psi_max_index = Psi_max;
	for(ranges_cnt = 0; ranges_cnt < PSI_FIELD_HEIGHT * PSI_FIELD_WIDTH; ranges_cnt++)
		*Ranges_field_ptr++ = *psi_max_index++ - *psi_min_index++;
}

void Calc_Ranges_medians(Uint8* Ranges_field, Uint8* Medians_array)
{
	int psi_row, psi_col, hist_cnt, median_sum;
	Uint8 hist[256];
	for(psi_row = 0; psi_row < PSI_FIELD_HEIGHT; psi_row++)
	{
		memset(hist, 0, sizeof(hist));
		for(psi_col = 0; psi_col < PSI_FIELD_WIDTH; psi_col++) hist[*Ranges_field++]++;
		for(hist_cnt = 0, median_sum = 0; median_sum < (PSI_FIELD_WIDTH >> 1); hist_cnt++) median_sum += hist[hist_cnt];
		Medians_array[psi_row] = --hist_cnt;
	}
}

void Calc_MPKR_medians(Uint8* Ranges_field_ptr, Uint8* Medians_array)
{
	int psi_row, psi_col, hist_cnt, median_sum, psi_row_index;
	short pkr_left, pkr_right;
	Uint16 hist[512];
	for(psi_row = 0; psi_row < PSI_FIELD_HEIGHT; psi_row++)
	{
		memset(hist, 0, sizeof(hist));
		psi_row_index = psi_row * PSI_FIELD_WIDTH;
		pkr_left = abs(Ranges_field_ptr[psi_row_index + 1] - Ranges_field_ptr[psi_row_index]);
		for(psi_col = 1; psi_col < PSI_FIELD_WIDTH - 1; psi_col++)
		{
			pkr_right = abs(Ranges_field_ptr[psi_row_index  + psi_col] - Ranges_field_ptr[psi_row_index + psi_col + 1]);
			hist[pkr_right + pkr_left]++;
			pkr_left = pkr_right;
		}
		for(hist_cnt = 0, median_sum = 0; median_sum < (PSI_FIELD_WIDTH >> 1); hist_cnt++) median_sum += hist[hist_cnt];
		Medians_array[psi_row] = --hist_cnt;
	}
}
int hist_porog = 10;
int Calc_porog(Uint16 *hist, Uint16 size)
{
//	int porog;
	int i;
//	Uint16 hist_moda, moda_value = 0;
//	//find moda
//	for(i = 0; i < size; i++)
//		if(hist[i] > moda_value)
//		{
//			moda_value = hist[i];
//			hist_moda = i;
//		}
//	int hist_incline = hist[hist_moda] / (size - hist_moda);
//	int current_altitude = hist[hist_moda];
//	int max_delta = 0;
//	for (i = hist_moda + 1; i < size; i++){
//		current_altitude -= hist_incline;
//		int delta = current_altitude - hist[i];
//		if (max_delta <= delta){
//			max_delta = delta;
//			porog = i;
//		}
//	}
//	return porog;
	for(i = size - 1; i >= 0; i--)
		if(hist[i] > hist_porog) break;
	return i;
}
int add_toPorog_psi = 1;
int add_toPorog_pix = 20;
#define pix_porog 20
Uint32 Porog_pix32 = pix_porog | (pix_porog  << 8) | (pix_porog  << 16) | (pix_porog  << 24);
void Calc_RCN(Uint8* Ranges_field, Uint8* Medians_range, Uint8* Medians_mpkr)
{
	int psi_row, psi_col, i;
	Uint16 Hist[HIST_LEVELS_N];
	Uint16 max_rcn = 0;
	Uint8* Ranges_field_ptr = Ranges_field;
	Uint8* Ranges_field_ptr_left = Ranges_field;
	Uint8* Ranges_field_ptr_right = Ranges_field + PSI_FIELD_WIDTH - 1;
	Uint16 Ranges_field16[PSI_FIELD_HEIGHT * PSI_FIELD_WIDTH];
	Uint16 *Ranges_field16_ptr = Ranges_field16;

	//set to zero left & right borders ranges field
	for(psi_row = 0; psi_row < PSI_FIELD_HEIGHT; psi_row++)
	{
		*Ranges_field_ptr_right = Medians_range[psi_row];
		*Ranges_field_ptr_left = Medians_range[psi_row];
		Ranges_field_ptr_right += PSI_FIELD_WIDTH;
		Ranges_field_ptr_left += PSI_FIELD_WIDTH;
	}

	memset(Hist, 0, sizeof(Hist));

	for(psi_row = 0; psi_row < PSI_FIELD_HEIGHT; psi_row++)
		for(psi_col = 0; psi_col < PSI_FIELD_WIDTH; psi_col++){
			int val = *Ranges_field_ptr++ - Medians_range[psi_row];
			*Ranges_field16_ptr =  (abs(val) << 8) / Medians_mpkr[psi_row];
			maximum16((Uint16*)&max_rcn, *Ranges_field16_ptr++);
		}

	int diveder = max_rcn / HIST_LEVELS_N + 1;

	for(i = 0; i < PSI_FIELD_HEIGHT * PSI_FIELD_WIDTH; i++)
		Hist[Ranges_field16[i] / diveder]++;

	Hist[0] -= (PSI_FIELD_HEIGHT << 1);

//	for(i = HIST_LEVELS_N - 1; i > 0; i--)
//		if(Hist[i - 1] == 0) Hist[i - 1] = Hist[i];

	Uint16 porog = (Calc_porog(Hist, HIST_LEVELS_N) + add_toPorog_psi) * diveder;

	for(i = 0; i < PSI_FIELD_HEIGHT * PSI_FIELD_WIDTH; i++)
		Ranges_field[i] = Ranges_field16[i] > porog ?  0xff : 0;
}

void Segmentation(Uint8* Ranges_field)
{
	Uint8 Medians_array[PSI_FIELD_HEIGHT];
	Uint8 MPKR_Medians_array[PSI_FIELD_HEIGHT];

	Calc_Ranges((Uint8*)strobAddrG, Ranges_field);
	Calc_Ranges_medians(Ranges_field, Medians_array);
	Calc_MPKR_medians(Ranges_field, MPKR_Medians_array);
	Calc_RCN(Ranges_field, Medians_array, MPKR_Medians_array);
}




Uint8 Calc_Uzmed_and_Pseg(Uint8* UzMe, Uint8* PSI_Sld, Uint8* Cadr, int k, int l)
{
	int i, j;
//	Uint16 hist[256];
	Uint16 max = 0;
//	memset(hist, 0, 256 * 2);
	for(i = 0; i < PSI_HEIGHT; i++)
	{
		int sum = 0;
		int pixel_idx = (i + l * PSI_HEIGHT) * CADR_WIDTH + k * PSI_WIDTH;
//		for(j = 0; j < PSI_WIDTH; j++) sum += Cadr[pixel_idx++];
//		UzMe[i] = sum / PSI_WIDTH;

		Uint32 mult = 0x01010101;
		Uint32 *cadr_index = (Uint32*)(Cadr + pixel_idx);
		for(j = 0; j < PSI_WIDTH; j += 4) sum += _dotpu4(mult, *cadr_index++);
			UzMe[i] = sum / PSI_WIDTH;

//		pixel_idx = (i + l * PSI_HEIGHT) * CADR_WIDTH + k * PSI_WIDTH;
//		for(j = 0; j < PSI_WIDTH; j++)
//		{
//			Uint16 val = abs(Cadr[pixel_idx++] - UzMe[i]);
////			hist[val]++;
//			maximum16((Uint16*)&max, val);
//		}
	}
//	return (Uint8)Calc_porog(hist, max);
	return max;
}

void Interpolation(Uint8* PSI_Sld, Uint8* Cadr)
{
	int k, l, i, j, psi_left, psi_right;
	int psi_row_idx, pixel_idx;
	Uint8 UzMe_left[PSI_HEIGHT], UzMe_right[PSI_HEIGHT];
	for(l = 0; l < PSI_FIELD_HEIGHT; l++)
	{
		psi_row_idx = l * PSI_FIELD_WIDTH;
		for(k = 0; k < PSI_FIELD_WIDTH; k++)
		{
			//look for nonzero psi
			if(PSI_Sld[psi_row_idx + k] > 0)
			{
				psi_left = k;
				//look for end of nonzero psi
				while(PSI_Sld[psi_row_idx + k] > 0 && k < PSI_FIELD_WIDTH) k++;
				if(k < PSI_FIELD_WIDTH)
				{
					psi_right = k - 1;
					Uint8 porog_left = Calc_Uzmed_and_Pseg(UzMe_left, PSI_Sld, Cadr, psi_left - 1, l) + add_toPorog_pix;
					Uint8 porog_right = Calc_Uzmed_and_Pseg(UzMe_right, PSI_Sld, Cadr, psi_right + 1, l) + add_toPorog_pix;
					volatile int pix_width = (psi_right - psi_left + 1) * PSI_WIDTH;
					for(i = 0; i < PSI_HEIGHT; i++)
					{
//						float interp_pix = (float)UzMe_left[i];
//						float interp_inc = (float)(UzMe_right[i] - UzMe_left[i]) / pix_width;
////						float porog_pix = (float)porog_left;
////						float porog_inc = (float)(porog_right - porog_left) / pix_width;
//						pixel_idx = (i + l * PSI_HEIGHT) * CADR_WIDTH + psi_left * PSI_WIDTH;
//						for(j = 0; j < pix_width; j++)
//						{
////							Cadr[pixel_idx++] = abs(Cadr[pixel_idx] - (Uint8)interp_pix);// > porog_pix ? 0xff : 0;
//							Cadr[pixel_idx++] = abs(Cadr[pixel_idx] - (Uint8)interp_pix) > add_toPorog_pix ? 0xff : 0;
//							interp_pix += interp_inc;
////							porog_pix += porog_inc;
//						}
						float interp_pix = (float)UzMe_left[i];
						float interp_inc = 4 * (float)(UzMe_right[i] - UzMe_left[i]) / pix_width;
//						float porog_pix = (float)porog_left;
//						float porog_inc = (float)(porog_right - porog_left) / pix_width;
						pixel_idx = (i + l * PSI_HEIGHT) * CADR_WIDTH + psi_left * PSI_WIDTH;
						Uint32 *cadr_index = (Uint32*)(Cadr + pixel_idx);
						for(j = 0; j < pix_width; j+=4)
						{
							Uint32 sub = _subabs4(*cadr_index, (Uint8)interp_pix | ((Uint8)interp_pix  << 8) | ((Uint8)interp_pix  << 16) | ((Uint8)interp_pix  << 24));
							Uint32 cmp = _cmpgtu4(sub, Porog_pix32);
							*cadr_index++ = _xpnd4(cmp);
//							Cadr[pixel_idx++] = abs(Cadr[pixel_idx] - (Uint8)interp_pix);// > porog_pix ? 0xff : 0;
//							Cadr[pixel_idx++] = abs(Cadr[pixel_idx] - (Uint8)interp_pix) > add_toPorog_pix ? 0xff : 0;
							interp_pix += interp_inc;
//							porog_pix += porog_inc;
						}
					}
				}
			}
		}
		//clearing zero psi
		for(k = 0; k < PSI_FIELD_WIDTH; k++)
			if(PSI_Sld[psi_row_idx + k] == 0)
			{
				pixel_idx = l * PSI_HEIGHT * CADR_WIDTH + k * PSI_WIDTH;
				for(i = 0; i < PSI_HEIGHT; i++)
				{
					memset(Cadr + pixel_idx, 0, PSI_WIDTH);
					pixel_idx += CADR_WIDTH;
				}
			}
	}
}

void Detect_zones(Uint8* Cadr, int zone_amount)
{
	int i, j, k;

	fill_Zones(Cadr, CADR_WIDTH, CADR_HEIGHT, zone_amount);
	//filter
	for(i = 1; i <= zone_amount; i++)
	{
		if(Zones[i].NumPix < MIN_ZONE_PIX)
			while(--zone_amount > 0)
			{
				if(Zones[zone_amount + 1].NumPix > MIN_ZONE_PIX)
				{
					Zones[i] = Zones[zone_amount + 1];
					break;
				}
			}
	}
	//sort
	for(i = 2; i <= zone_amount; i++)
	{
		for(j = 1; j < i; j++)
		{
			if(Zones[i].NumPix > Zones[j].NumPix)
			{
				Zones[0] = Zones[i];
				for(k = i; k > j; k--)
					Zones[k] = Zones[k - 1];
				Zones[j] = Zones[0];
				break;
			}
		}
	}
	memset(Zones, 0, sizeof(Zone_type));
	Zones[0].Num = zone_amount;
}

void aru(Uint8* Cadr)
{
	int i, j;
	Uint8 min = 0xff, max = 0;
	int row_index;
//	int hist[256];
//	memset(hist, 0, sizeof(hist));
	int midle = 0;
	for(i = DROP_PIXELS_BORDER; i < CADR_HEIGHT - DROP_PIXELS_BORDER; i++)
	{
		row_index = i * CADR_WIDTH;
		for(j = DROP_PIXELS_BORDER; j < CADR_WIDTH - DROP_PIXELS_BORDER; j++)
		{
//			hist[Cadr[row_index + j]]++;
			midle += Cadr[row_index + j];
			maximum8((Uint8*)&max, Cadr[row_index + j]);
			minimum8((Uint8*)&min, Cadr[row_index + j]);
		}
	}
	midle /= (CADR_WIDTH - DROP_PIXELS_BORDER * 2) * (CADR_HEIGHT - DROP_PIXELS_BORDER * 2);
//	int moda, moda_val = 0;
//	for(i = min; i < max; i++)
//		if(hist[i] > moda_val)
//		{
//			moda = i;
//			moda_val = hist[i];
//		}
	int z, x;
	int Y = 8;
	int n = 255;
//	int c = (min + max) >> 1;
	int c = midle;
	int d = max - min;
	if (d < (n >> 1)) d = n >> 1;
	int k = (n << Y) / d;
	for(i = 0; i < CADR_HEIGHT * CADR_WIDTH; i++)
	{
		x = Cadr[i];
		z = ((c << Y) - k * (c - x)) >> Y;
		if(z > 255) z = 255;
		if(z < 0) z = 0;
		Cadr[i] = z;
	}
}

void Detection()
{
	Uint8 Ranges_field[PSI_FIELD_WIDTH * PSI_FIELD_HEIGHT];
//	aru((Uint8*)strobAddrG);
	Segmentation(Ranges_field);
	Interpolation(Ranges_field, (Uint8*)strobAddrG);
	int zone_amount = Fill_Connectivity((Uint8*)strobAddrG, CADR_HEIGHT, CADR_WIDTH);
	Detect_zones((Uint8*)strobAddrG, zone_amount);
}
