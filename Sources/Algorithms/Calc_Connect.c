#include "main.h"
#include <string.h>

void fill_border(Uint8* Cadr, short cols, short rows, Uint8 Color)
{
	memset(Cadr, Color, cols);
	memset(Cadr + (rows - 1) * cols, Color, cols);
	int i;
	for(i = 0; i < rows; i++)
	{
		Cadr[i * cols] = Color;
		Cadr[(i + 1) * cols - 1] = Color;
	}
}

void fill_Zones(Uint8* Cadr, short cols, short rows, int zone_amount)
{
	int i, j;
	memset(Zones, 0, sizeof(Zones));
	for(i = 1; i <= zone_amount; i++)
	{
		Zones[i].left = cols;
		Zones[i].top = rows;
	}
	for(i = 0; i < rows; i++)
	{
		int row_idx = i * cols;
		for(j = 0; j < cols; j++)
			if(Cadr[row_idx + j])
			{
				int num = Cadr[row_idx + j];
				Zones[num].NumPix++;
				if(j < Zones[num].left)  	Zones[num].left = j;
				if(j > Zones[num].right)  	Zones[num].right = j;
				if(i < Zones[num].top)  	Zones[num].top = i;
				if(i > Zones[num].bottom)  	Zones[num].bottom = i;
			}
	}
}

volatile int max_stack_cnt = 0;
Zone_type fill8sv(Uint8* Cadr, Uint8* CAD, short cols, short rows, int j1, int i1, Uint8 Color, Uint8 cnt, Uint8 porog)
{
	//fill border with opposite color
	Uint8 num;
	if(cnt > 128) num = 0;//white
	else num = 255;
	fill_border(Cadr, cols, rows, num);

	memset(CAD, 0, MaxCadr);

	int ist = 0;
	int def = 0;
	if(cnt > 128)//white
	{
		num = Color - porog;
		while(ist >= 0)
		{
        	if(max_stack_cnt < ist) max_stack_cnt = ist;
			CAD[i1 + j1] = cnt;
			switch (def)
			{
				case 0:
					if (Cadr[i1 + j1 - 1] > num && CAD[i1 + j1 - 1] == 0){//left
							stack32[ist].def = 4;
							stack32[ist].i = i1;
							stack32[ist].j = j1;
							ist++;

							j1 -= 1;
							def = 0;
//							Cadr[i1 + j1] = cnt;

							break;
					}
				case 4:
					if (Cadr[i1 - cols + j1 - 1] > num && CAD[i1 - cols + j1 - 1] == 0){//left//up
							stack32[ist].def = 5;
							stack32[ist].i = i1;
							stack32[ist].j = j1;
							ist++;

							i1 -= cols;
							j1 -= 1;
							def = 0;
//							Cadr[i1 + j1] = cnt;

							break;
					}
				case 5:
					if (Cadr[i1 + cols + j1 - 1] > num && CAD[i1 + cols + j1 - 1] == 0){//left//down
							stack32[ist].def = 6;
							stack32[ist].i = i1;
							stack32[ist].j = j1;
							ist++;

							i1 += cols;
							j1 -= 1;
							def = 0;
//							Cadr[i1 + j1] = cnt;

							break;
					}
				case 6:
					if (Cadr[i1 + cols + j1 + 1] > num && CAD[i1 + cols + j1 + 1] == 0){//right//down
							stack32[ist].def = 7;
							stack32[ist].i = i1;
							stack32[ist].j = j1;
							ist++;

							i1 += cols;
							j1 += 1;
							def = 0;
//							Cadr[i1 + j1] = cnt;

							break;
					}
				case 7:
					if (Cadr[i1 - cols + j1 + 1] > num && CAD[i1 - cols + j1 + 1] == 0){//right//up
							stack32[ist].def = 1;
							stack32[ist].i = i1;
							stack32[ist].j = j1;
							ist++;

							i1 -= cols;
							j1 += 1;
							def = 0;
//							Cadr[i1 + j1] = cnt;

							break;
					}
				case 1:
					if (Cadr[i1 + j1 + 1] > num && CAD[i1 + j1 + 1] == 0){//right
							stack32[ist].def = 2;
							stack32[ist].i = i1;
							stack32[ist].j = j1;
							ist++;

							j1 += 1;
							def = 0;
//							Cadr[i1 + j1] = cnt;

							break;
					}
				case 2:
					if (Cadr[i1 - cols + j1] > num && CAD[i1 - cols + j1] == 0){//up
							stack32[ist].def = 3;
							stack32[ist].i = i1;
							stack32[ist].j = j1;
							ist++;

							i1 -= cols;
							def = 0;
//							Cadr[i1 + j1] = cnt;

							break;
					}
				case 3:
					if (Cadr[i1 + cols + j1] > num && CAD[i1 + cols + j1] == 0){//down
							i1 += cols;
							def = 0;
//							Cadr[i1 + j1] = cnt;

							break;
					}
					ist--;
					j1 = stack32[ist].j;
					i1 = stack32[ist].i;
					def = stack32[ist].def;
			}
			if(ist >= (int)MaxStack)
			{
				ist--;
				j1 = stack32[ist].j;
				i1 = stack32[ist].i;
				def = stack32[ist].def;
			}
		}
	}
	else//black
	{
		num = Color + porog;
		while(ist >=0 )
		{
        	if(max_stack_cnt < ist) max_stack_cnt = ist;
			CAD[i1 + j1] = cnt;
			switch (def)
			{
				case 0:
					if (Cadr[i1 + j1 - 1] < num && CAD[i1 + j1 - 1] == 0){//left
							stack32[ist].def = 4;
							stack32[ist].i = i1;
							stack32[ist].j = j1;
							ist++;

							j1 -= 1;
							def = 0;
//							Cadr[i1 + j1] = cnt;

							break;
					}
				case 4:
					if (Cadr[i1 - cols + j1 - 1] < num && CAD[i1 - cols + j1 - 1] == 0){//left//up
							stack32[ist].def = 5;
							stack32[ist].i = i1;
							stack32[ist].j = j1;
							ist++;

							i1 -= cols;
							j1 -= 1;
							def = 0;
//							Cadr[i1 + j1] = cnt;

							break;
					}
				case 5:
					if (Cadr[i1 + cols + j1 - 1] < num && CAD[i1 + cols + j1 - 1] == 0){//left//down
							stack32[ist].def = 6;
							stack32[ist].i = i1;
							stack32[ist].j = j1;
							ist++;

							i1 += cols;
							j1 -= 1;
							def = 0;
//							Cadr[i1 + j1] = cnt;

							break;
					}
				case 6:
					if (Cadr[i1 + cols + j1 + 1] < num && CAD[i1 + cols + j1 + 1] == 0){//right//down
							stack32[ist].def = 7;
							stack32[ist].i = i1;
							stack32[ist].j = j1;
							ist++;

							i1 += cols;
							j1 += 1;
							def = 0;
//							Cadr[i1 + j1] = cnt;

							break;
					}
				case 7:
					if (Cadr[i1 - cols + j1 + 1] < num && CAD[i1 - cols + j1 + 1] == 0){//right//up
							stack32[ist].def = 1;
							stack32[ist].i = i1;
							stack32[ist].j = j1;
							ist++;

							i1 -= cols;
							j1 += 1;
							def = 0;
//							Cadr[i1 + j1] = cnt;

							break;
					}
				case 1:
					if (Cadr[i1 + j1 + 1] < num && CAD[i1 + j1 + 1] == 0){//right
							stack32[ist].def = 2;
							stack32[ist].i = i1;
							stack32[ist].j = j1;
							ist++;

							j1 += 1;
							def = 0;
//							Cadr[i1 + j1] = cnt;

							break;
					}
				case 2:
					if (Cadr[i1 - cols + j1] < num && CAD[i1 - cols + j1] == 0){//up
							stack32[ist].def = 3;
							stack32[ist].i = i1;
							stack32[ist].j = j1;
							ist++;

							i1 -= cols;
							def = 0;
//							Cadr[i1 + j1] = cnt;

							break;
					}
				case 3:
					if (Cadr[i1 + cols + j1] < num && CAD[i1 + cols + j1] == 0){//down
							i1 += cols;
							def = 0;
//							Cadr[i1 + j1] = cnt;

							break;
					}
					ist--;
					j1 = stack32[ist].j;
					i1 = stack32[ist].i;
					def = stack32[ist].def;
			}
			if(ist >= (int)MaxStack)
			{
				ist--;
				j1 = stack32[ist].j;
				i1 = stack32[ist].i;
				def = stack32[ist].def;
			}
		}
	}
	Zone_type Zone;
	memset(&Zone, 0, sizeof(Zone_type));
	Zone.left = cols;
	Zone.top = rows;
	for(i1 = 0; i1 < rows; i1 ++)//check zone size
	{
		int row_idx = i1 * cols;
		for(j1 = 0; j1 < cols; j1++)
			if(CAD[row_idx + j1] == cnt)
			{
				Zone.NumPix++;
				Zone.SumBright += Cadr[row_idx + j1];
				if(Zone.left > j1) Zone.left = j1;
				if(Zone.right < j1) Zone.right = j1;
				if(Zone.top > i1) Zone.top = i1;
				if(Zone.bottom < i1) Zone.bottom = i1;
				Zone.SumBrightX += j1 * Cadr[row_idx + j1];
				Zone.SumBrightY += i1 * Cadr[row_idx + j1];
				Zone.InvSumBrightX += j1 * (255 - Cadr[row_idx + j1]);
				Zone.InvSumBrightY += i1 * (255 - Cadr[row_idx + j1]);
			}
	}
	return Zone;
}


short Fill_Connectivity(Uint8* Cadr, Uint32 rows, Uint32 cols){

	//fill border with opposite color
	fill_border(Cadr, cols, rows, 0);

//	memset(stack32, 0, sizeof(stack32));

    int i ,j ,i1 , j1 ;
    int ist, def;
    Uint32 maxi = cols * rows;
    short cnt = 0, num = 255;
    for(i = cols; i < maxi - cols; i += cols){
        for(j = 1; j < cols - 1; j++){
            if (Cadr[i + j] == num){
                cnt++;
                if(cnt > 253) break;
                ist=0;
               	j1 = j;
               	i1 = i;
               	def = 0;

                while(ist>=0){
                	if(max_stack_cnt < ist) max_stack_cnt = ist;
                  	Cadr[i1 + j1] = cnt;
					switch (def){
						case 0:
							if (Cadr[i1 + j1 - 1] == num){//left
									stack32[ist].def = 4;
									stack32[ist].i = i1;
									stack32[ist].j = j1;
									ist++;

									j1 -= 1;
									def = 0;
//					               	Cadr[i1 + j1] = cnt;

									break;
							}
						case 4:
							if (Cadr[i1 - cols + j1 - 1] == num){//left//up
									stack32[ist].def = 5;
									stack32[ist].i = i1;
									stack32[ist].j = j1;
									ist++;

									i1 -= cols;
									j1 -= 1;
									def = 0;
//					               	Cadr[i1 + j1] = cnt;

									break;
							}
						case 5:
							if (Cadr[i1 + cols + j1 - 1] == num){//left//down
									stack32[ist].def = 6;
									stack32[ist].i = i1;
									stack32[ist].j = j1;
									ist++;

									i1 += cols;
									j1 -= 1;
									def = 0;
//					               	Cadr[i1 + j1] = cnt;

									break;
							}
						case 6:
							if (Cadr[i1 + cols + j1 + 1] == num){//right//down
									stack32[ist].def = 7;
									stack32[ist].i = i1;
									stack32[ist].j = j1;
									ist++;

									i1 += cols;
									j1 += 1;
									def = 0;
//					               	Cadr[i1 + j1] = cnt;

									break;
							}
						case 7:
							if (Cadr[i1 - cols + j1 + 1] == num){//right//up
									stack32[ist].def = 1;
									stack32[ist].i = i1;
									stack32[ist].j = j1;
									ist++;

									i1 -= cols;
									j1 += 1;
									def = 0;
//					               	Cadr[i1 + j1] = cnt;

									break;
							}
						case 1:
							if (Cadr[i1 + j1 + 1] == num){//right
									stack32[ist].def = 2;
									stack32[ist].i = i1;
									stack32[ist].j = j1;
									ist++;

									j1 += 1;
									def = 0;
//					               	Cadr[i1 + j1] = cnt;

									break;
							}
						case 2:
							if (Cadr[i1 - cols + j1] == num){//up
									stack32[ist].def = 3;
									stack32[ist].i = i1;
									stack32[ist].j = j1;
									ist++;

									i1 -= cols;
									def = 0;
//					               	Cadr[i1 + j1] = cnt;

									break;
							}
						case 3:
							if (Cadr[i1 + cols + j1] == num){//down
									i1 += cols;
									def = 0;
//					               	Cadr[i1 + j1] = cnt;

									break;
							}
							ist--;
							j1 = stack32[ist].j;
							i1 = stack32[ist].i;
							def = stack32[ist].def;
					}
                	if(ist >= (int)MaxStack)
                	{
						ist--;
						j1 = stack32[ist].j;
						i1 = stack32[ist].i;
						def = stack32[ist].def;
                	}
                }
    }}}
    return cnt;
}
