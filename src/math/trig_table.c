// trig_table.c

#include "trig_table.h"

static float sin_table [256] = {
 0.0,					 0.024541228522912288,	 0.049067674327418015,	 0.07356456359966743,
 0.0980171403295606,	 0.1224106751992162,	 0.14673047445536175,	 0.17096188876030122,
 0.19509032201612825,	 0.2191012401568698,	 0.24298017990326387,	 0.26671275747489837,
 0.29028467725446233,	 0.3136817403988915,	 0.33688985339222005,	 0.3598950365349881,
 0.3826834323650898,	 0.40524131400498986,	 0.4275550934302821,	 0.44961132965460654,
 0.47139673682599764,	 0.49289819222978404,	 0.5141027441932217,	 0.5349976198870972,
 0.5555702330196022,	 0.5758081914178453,	 0.5956993044924334,	 0.6152315905806268,
 0.6343932841636455,	 0.6531728429537768,	 0.6715589548470183,	 0.6895405447370668,
 0.7071067811865475,	 0.7242470829514669,	 0.7409511253549591,	 0.7572088465064845,
 0.773010453362737,		 0.7883464276266062,	 0.8032075314806448,	 0.8175848131515837,
 0.8314696123025452,	 0.844853565249707,		 0.8577286100002721,	 0.8700869911087113,
 0.8819212643483549,	 0.8932243011955153,	 0.9039892931234433,	 0.9142097557035307,
 0.9238795325112867,	 0.9329927988347388,	 0.9415440651830208,	 0.9495281805930367,
 0.9569403357322089,	 0.9637760657954398,	 0.970031253194544,		 0.9757021300385286,
 0.9807852804032304,	 0.9852776423889412,	 0.989176509964781,		 0.99247953459871,
 0.9951847266721968,	 0.9972904566786902,	 0.9987954562051724,	 0.9996988186962042,
 1.0,					 0.9996988186962042,	 0.9987954562051724,	 0.9972904566786902,
 0.9951847266721969,	 0.99247953459871,		 0.989176509964781,		 0.9852776423889412,
 0.9807852804032304,	 0.9757021300385286,	 0.970031253194544,		 0.9637760657954398,
 0.9569403357322089,	 0.9495281805930367,	 0.9415440651830208,	 0.9329927988347388,
 0.9238795325112867,	 0.9142097557035307,	 0.9039892931234434,	 0.8932243011955152,
 0.881921264348355,		 0.8700869911087115,	 0.8577286100002721,	 0.8448535652497072,
 0.8314696123025455,	 0.8175848131515837,	 0.8032075314806449,	 0.7883464276266063,
 0.7730104533627371,	 0.7572088465064847,	 0.740951125354959,		 0.7242470829514669,
 0.7071067811865476,	 0.689540544737067,		 0.6715589548470186,	 0.6531728429537766,
 0.6343932841636455,	 0.6152315905806269,	 0.5956993044924335,	 0.5758081914178454,
 0.5555702330196022,	 0.5349976198870972,	 0.5141027441932218,	 0.49289819222978415,
 0.47139673682599786,	 0.4496113296546069,	 0.42755509343028203,	 0.4052413140049899,
 0.3826834323650899,	 0.35989503653498833,	 0.33688985339222033,	 0.3136817403988914,
 0.2902846772544624,	 0.2667127574748985,	 0.24298017990326407,	 0.21910124015687005,
 0.1950903220161286,	 0.17096188876030122,	 0.1467304744553618,	 0.12241067519921635,
 0.09801714032956083,	 0.07356456359966773,	 0.049067674327417966,	 0.024541228522912326,
 0.0,					-0.02454122852291208,	-0.049067674327417724,	-0.0735645635996675,
-0.09801714032956059,	-0.1224106751992161,	-0.14673047445536158,	-0.17096188876030097,
-0.19509032201612836,	-0.2191012401568698,	-0.24298017990326382,	-0.26671275747489825,
-0.2902846772544621,	-0.3136817403988912,	-0.3368898533922201,	-0.3598950365349881,
-0.38268343236508967,	-0.4052413140049897,	-0.4275550934302818,	-0.44961132965460665,
-0.47139673682599764,	-0.4928981922297839,	-0.5141027441932216,	-0.5349976198870969,
-0.555570233019602,		-0.5758081914178453,	-0.5956993044924332,	-0.6152315905806267,
-0.6343932841636453,	-0.6531728429537765,	-0.6715589548470184,	-0.6895405447370668,
-0.7071067811865475,	-0.7242470829514668,	-0.7409511253549589,	-0.7572088465064842,
-0.7730104533627367,	-0.7883464276266059,	-0.803207531480645,		-0.8175848131515838,
-0.8314696123025452,	-0.844853565249707,		-0.857728610000272,		-0.8700869911087113,
-0.8819212643483549,	-0.8932243011955152,	-0.9039892931234431,	-0.9142097557035305,
-0.9238795325112865,	-0.932992798834739,		-0.9415440651830208,	-0.9495281805930367,
-0.9569403357322088,	-0.9637760657954398,	-0.970031253194544,		-0.9757021300385285,
-0.9807852804032303,	-0.9852776423889411,	-0.9891765099647809,	-0.9924795345987101,
-0.9951847266721969,	-0.9972904566786902,	-0.9987954562051724,	-0.9996988186962042,
-1.0,					-0.9996988186962042,	-0.9987954562051724,	-0.9972904566786902,
-0.9951847266721969,	-0.9924795345987101,	-0.9891765099647809,	-0.9852776423889412,
-0.9807852804032304,	-0.9757021300385286,	-0.970031253194544,		-0.96377606579544,
-0.9569403357322089,	-0.9495281805930368,	-0.9415440651830209,	-0.9329927988347391,
-0.9238795325112866,	-0.9142097557035306,	-0.9039892931234433,	-0.8932243011955153,
-0.881921264348355,		-0.8700869911087115,	-0.8577286100002722,	-0.8448535652497072,
-0.8314696123025455,	-0.817584813151584,		-0.8032075314806453,	-0.7883464276266061,
-0.7730104533627369,	-0.7572088465064846,	-0.7409511253549591,	-0.724247082951467,
-0.7071067811865477,	-0.6895405447370672,	-0.6715589548470187,	-0.6531728429537771,
-0.6343932841636459,	-0.6152315905806274,	-0.5956993044924332,	-0.5758081914178452,
-0.5555702330196022,	-0.5349976198870973,	-0.5141027441932219,	-0.49289819222978426,
-0.4713967368259979,	-0.449611329654607,		-0.42755509343028253,	-0.4052413140049904,
-0.3826834323650904,	-0.359895036534988,		-0.33688985339222,		-0.3136817403988915,
-0.2902846772544625,	-0.2667127574748986,	-0.24298017990326418,	-0.21910124015687016,
-0.19509032201612872,	-0.17096188876030177,	-0.1467304744553624,	-0.12241067519921603,
-0.0980171403295605,	-0.07356456359966741,	-0.04906767432741809,	-0.024541228522912448
};

float rad_sin(float f){

	return sin_table[((int)(f * (255/(2*PI)))) % 255];

}

float rad_cos(float f){

	// Subtract 90 degrees from phase
	return sin_table[(((int)((f-(PI/2)) * (255/(2*PI)))) % 255)];

}
