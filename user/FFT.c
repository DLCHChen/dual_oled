/*
CFP
2022 - 
*/

#include "board_cfg.h"
#include "debug.h"
#include "tim.h"
#include "fft.h"
#include "disp_show.h"

const s16 x512_COS_TABLE[512]=
{
  1024,1024,1024,1024,1024,1024,1023,1023,1023,1022,
  1022,1022,1021,1021,1020,1020,1019,1018,1018,1017,
  1016,1016,1015,1014,1013,1012,1011,1010,1009,1008,
  1007,1006,1004,1003,1002,1000,999,998,996,995,
  993,992,990,989,987,985,983,982,980,978,
  976,974,972,970,968,966,964,962,960,958,
  955,953,951,948,946,944,941,939,936,934,
  931,928,926,923,920,917,915,912,909,906,
  903,900,897,894,891,888,885,882,878,875,
  872,868,865,862,858,855,851,848,844,841,
  837,834,830,826,822,819,815,811,807,803,
  799,796,792,788,784,779,775,771,767,763,
  759,755,750,746,742,737,733,729,724,720,
  715,711,706,702,697,692,688,683,678,674,
  669,664,659,654,650,645,640,635,630,625,
  620,615,610,605,600,595,590,584,579,574,
  569,564,558,553,548,543,537,532,526,521,
  516,510,505,499,494,488,483,477,472,466,
  460,455,449,443,438,432,426,421,415,409,
  403,398,392,386,380,374,369,363,357,351,
  345,339,333,327,321,315,309,303,297,291,
  285,279,273,267,261,255,249,243,237,230,
  224,218,212,206,200,194,187,181,175,169,
  163,156,150,144,138,132,125,119,113,107,
  100,94,88,82,75,69,63,57,50,44,
  38,31,25,19,13,6,0,-6,-13,-19,
  -25,-31,-38,-44,-50,-57,-63,-69,-75,-82,
  -88,-94,-100,-107,-113,-119,-125,-132,-138,-144,
  -150,-156,-163,-169,-175,-181,-187,-194,-200,-206,
  -212,-218,-224,-230,-237,-243,-249,-255,-261,-267,
  -273,-279,-285,-291,-297,-303,-309,-315,-321,-327,
  -333,-339,-345,-351,-357,-363,-369,-374,-380,-386,
  -392,-398,-403,-409,-415,-421,-426,-432,-438,-443,
  -449,-455,-460,-466,-472,-477,-483,-488,-494,-499,
  -505,-510,-516,-521,-526,-532,-537,-543,-548,-553,
  -558,-564,-569,-574,-579,-584,-590,-595,-600,-605,
  -610,-615,-620,-625,-630,-635,-640,-645,-650,-654,
  -659,-664,-669,-674,-678,-683,-688,-692,-697,-702,
  -706,-711,-715,-720,-724,-729,-733,-737,-742,-746,
  -750,-755,-759,-763,-767,-771,-775,-779,-784,-788,
  -792,-796,-799,-803,-807,-811,-815,-819,-822,-826,
  -830,-834,-837,-841,-844,-848,-851,-855,-858,-862,
  -865,-868,-872,-875,-878,-882,-885,-888,-891,-894,
  -897,-900,-903,-906,-909,-912,-915,-917,-920,-923,
  -926,-928,-931,-934,-936,-939,-941,-944,-946,-948,
  -951,-953,-955,-958,-960,-962,-964,-966,-968,-970,
  -972,-974,-976,-978,-980,-982,-983,-985,-987,-989,
  -990,-992,-993,-995,-996,-998,-999,-1000,-1002,-1003,
  -1004,-1006,-1007,-1008,-1009,-1010,-1011,-1012,-1013,-1014,
  -1015,-1016,-1016,-1017,-1018,-1018,-1019,-1020,-1020,-1021,
  -1021,-1022,-1022,-1022,-1023,-1023,-1023,-1024,-1024,-1024,
  -1024,-1024
};

void sFast_FFT(s32 *x, s16 n)
{
	u16 i, j, k, m, i1, i2, i3, i4, n1, n2, n4;
	u16 a, e;
	s32 xt;

	s32 t1, t2, cc, ss;
	m = 0;
	for(j = 1, i = 1; i < 16; i ++)
	{
		m = i;
		j = (j << 1);
		if(j == n)
			break;
	}
	n1 = n - 1;
	for(j = 0, i = 0; i < n1; i ++)
	{
		if(i < j)
		{
			xt = x[j];
			x[j] = x[i];
			x[i] = xt;
		}
		k = (n>> 1);
		while(k < (j + 1))
		{
			j -= k;
			k = (k>> 1);
		}
		j += k;
	}
	for(i = 0; i < n; i += 2)
	{
		xt = x[i];
		x[i] += x[i + 1];
		x[i + 1] = xt - x[i + 1];
	}

	n2 = 1;
	for(k = 2; k <= m; k++)
	{
		n4 = n2;
		n2 = (n4 << 1);
		n1 = (n2 << 1);
		e = 1024 >> k;
		for(i = 0; i < n; i += n1)
		{
			xt = x[i];
			x[i] += x[i + n2];
			x[i + n2] = xt - x[i + n2];
			x[i + n2 + n4] = -x[i + n2 + n4];
			a = e;
			for(j = 1; j < n4; j ++)
			{
				i1 = i + j;
				i2 = i - j + n2;
				i3 = i + j + n2;
				i4 = i - j + n1;
				cc = (s32)x512_COS_TABLE[a];
				ss = (s32)x512_COS_TABLE[(1024/4)-a];
				a += e;
				t1 = cc * x[i3] + ss * x[i4];
				t2 = ss * x[i3] - cc * x[i4];
				t1 = (t1>> 10);
				t2 = (t2>> 10);
				x[i4] = x[i2] - t2;
				x[i3] = -x[i2] - t2;
				x[i2] = x[i1] - t1;
				x[i1] = x[i1] + t1;
			}
		}
	}
}

void xFast_FFT(s16 *x, s16 n)
{
	u16 i, j, k, m, i1, i2, i3, i4, n1, n2, n4;
	u16 a, e;
	s16 xt;
	s32 t1, t2, cc, ss;
	m = 0;
	for(j = 1, i = 1; i < 16; i ++)
	{
		m = i;
		j = (j << 1);
		if(j == n)
			break;
	}
	n1 = n - 1;
	for(j = 0, i = 0; i < n1; i ++)
	{
		if(i < j)
		{
			xt = x[j];
			x[j] = x[i];
			x[i] = xt;
		}
		k = (n>> 1);
		while(k < (j + 1))
		{
			j -= k;
			k = (k>> 1);
		}
		j += k;
	}
	for(i = 0; i < n; i += 2)
	{
		xt = x[i];
		x[i] += x[i + 1];
		x[i + 1] = xt - x[i + 1];
	}

	n2 = 1;
	for(k = 2; k <= m; k++)
	{
		n4 = n2;
		n2 = (n4 << 1);
		n1 = (n2 << 1);
		e = 1024 >> k;
		for(i = 0; i < n; i += n1)
		{
			xt = x[i];
			x[i] += x[i + n2];
			x[i + n2] = xt - x[i + n2];
			x[i + n2 + n4] = -x[i + n2 + n4];
			a = e;
			for(j = 1; j < n4; j ++)
			{
				i1 = i + j;
				i2 = i - j + n2;
				i3 = i + j + n2;
				i4 = i - j + n1;
				cc = (s32)x512_COS_TABLE[a];
				ss = (s32)x512_COS_TABLE[(1024/4)-a];
				a += e;
				t1 = cc * x[i3] + ss * x[i4];
				t2 = ss * x[i3] - cc * x[i4];
				t1 = (t1>> 10);
				t2 = (t2>> 10);
				x[i4] = x[i2] - t2;
				x[i3] = -x[i2] - t2;
				x[i2] = x[i1] - t1;
				x[i1] = x[i1] + t1;
			}
		}
	}
}


