#include <math.h>
#include "sensor_static_data.h"
#include "sensor.h"
#include "utils.h"

////------------------------------------------
////Polyfit params define for Wheatstone bridge leg=300ohm
////------------------------------------------
//############################
//# SensorName:RTD_PT100_3851
//############################
const float COFF_RTD_PT100_3851[3][5] =
{
    {583.0886,	1.9644228E-4,	1.920789E-11,	3.0531867E-18,	2.1660222E-25,	},//0.010437012
    {557.685,	1.655523E-4,	5.3009533E-12,	3.0903527E-19,	1.68617E-26,	},//0.0093688965
    {556.9608,	1.6751903E-4,	3.2995859E-12,	1.1907909E-18,	-1.1623763E-25,	},//1.8310547E-4
};

const int32_t SEGMENT_RTD_PT100_3851[2] = {-3155657, 1340818, };
//{ -5282221.0 , 1666448.0 },//RTD_PT100_3851


//############################
//# SensorName:RTD_PT200_3851
//############################
const float COFF_RTD_PT200_3851[3][5] =
{
    {130.40186,	0.0013066423,	3.4185807E-10,	4.572508E-16,	1.4600823E-21,	},//0.010314941
    {130.44365,	0.0013072764,	3.2904632E-10,	1.09074074E-16,	1.3706364E-22,	},//0.009185791
    {113.391914,	0.0014549596,	-1.4172348E-10,	7.5723147E-16,	-1.8261886E-22,	},//9.765625E-4
};

const int32_t SEGMENT_RTD_PT200_3851[2] = {-42862, 428948, };
//{ -271156.0 , 478143.0 },//RTD_PT200_3851

//############################
//# SensorName:RTD_PT500_3851
//############################
const float COFF_RTD_PT500_3851[3][5] =
{
    {-100.62759,	4.846489E-4,	7.824277E-11,	-3.7562963E-17,	3.7207672E-23,	},//0.010223389
    {-100.4082,	4.8523568E-4,	6.5374435E-11,	6.5181765E-19,	5.9821E-24,	},//0.009567261
    {-87.398766,	4.3844202E-4,	1.2755481E-10,	-3.5695133E-17,	1.3890293E-23,	},//5.493164E-4
};

const int32_t SEGMENT_RTD_PT500_3851[2] = {335802, 1383910, };
//{ -213273.0 , 1555116.0 },//RTD_PT500_3851

//############################
//# SensorName:RTD_PT1000_3851
//############################
const float COFF_RTD_PT1000_3851[3][5] =
{
    {-173.1623,	2.3210664E-4,	3.157011E-11,	-6.711697E-18,	2.3269886E-24,	},//0.010360718
    {-173.00978,	2.3428911E-4,	2.497443E-11,	-9.150627E-19,	8.2542604E-25,	},//0.009777069
    {-96.91556,	1.1587506E-4,	9.467141E-11,	-1.9308768E-17,	2.661859E-24,	},//3.6621094E-4
};

const int32_t SEGMENT_RTD_PT1000_3851[2] = {925180, 2657510, };
//{ -117509.0 , 3115524.0 },//RTD_PT1000_3851


//############################
//# SensorName:RTD_PT100_3916
//############################
const float COFF_RTD_PT100_3916[2][6] =
{
    {582.10834,	2.0438546E-4,	2.4559113E-11,	4.4151774E-18,	3.870135E-25,	8.5401046E-33,	},//0.008361816
    {547.57416,	1.6222255E-4,	5.068822E-12,	2.9232405E-19,	2.1733226E-26,	1.4236266E-33,	},//0.0012207031
};

const int32_t SEGMENT_RTD_PT100_3916[1] = {-2748105, };
//{ -5308904.0 , 1756102.0 },//RTD_PT100_3916

//############################
//# SensorName:RTD_PT200_3916
//############################
const float COFF_RTD_PT200_3916[2][6] =
{
    {128.25424,	0.0012859161,	3.5529898E-10,	6.0824483E-16,	1.9737814E-21,	7.1153005E-28,	},//0.008361816
    {128.2492,	0.0012849807,	3.1358657E-10,	1.4310769E-16,	1.27250695E-23,	1.1485099E-28,	},//0.0021362305
};

const int32_t SEGMENT_RTD_PT200_3916[1] = {5246, };
//{ -274086.0 , 487635.0 },//RTD_PT200_3916

//############################
//# SensorName:RTD_PT500_3916
//############################
const float COFF_RTD_PT500_3916[3][5] =
{
    {-98.99176,	4.7689813E-4,	7.6280864E-11,	-3.6490232E-17,	3.565273E-23,	},//0.0107421875
    {-98.77374,	4.7747718E-4,	6.3659196E-11,	6.8156694E-19,	5.6618338E-24,	},//0.009666443
    {-67.00033,	3.8090706E-4,	1.7492771E-10,	-5.691742E-17,	1.6959126E-23,	},//4.8828125E-4
};

const int32_t SEGMENT_RTD_PT500_3916[2] = {338040, 1402634, };
//{ -220559.0 , 1576369.0 },//RTD_PT500_3916

//############################
//# SensorName:RTD_PT1000_3916
//############################
const float COFF_RTD_PT1000_3916[3][5] =
{
    {-170.38506,	2.2853236E-4,	3.0739217E-11,	-6.326602E-18,	2.1198438E-24,	},//0.010040283
    {-170.31725,	2.3086792E-4,	2.4184132E-11,	-7.907525E-19,	7.7824427E-25,	},//0.010070801
    {-99.03559,	1.2019452E-4,	8.917381E-11,	-1.7900915E-17,	2.4822252E-24,	},//3.6621094E-4
};

const int32_t SEGMENT_RTD_PT1000_3916[2] = {910981, 2674926, };
//{ -131952.0 , 3151337.0 },//RTD_PT1000_3916

//############################
//# SensorName:RTD_PT100_3920
//############################
const float COFF_RTD_PT100_3920[3][5] =
{
    {569.86237,	1.9007591E-4,	1.7742717E-11,	2.7949368E-18,	1.982289E-25,	},//0.010131836
    {546.7705,	1.619405E-4,	5.054311E-12,	2.880179E-19,	1.5641337E-26,	},//0.009216309
    {547.3117,	1.605493E-4,	6.3922036E-12,	-3.030308E-19,	1.2339546E-25,	},//1.8310547E-4
};

const int32_t SEGMENT_RTD_PT100_3920[2] = {-3145172, 1432250, };
//{ -5309656.0 , 1763767.0 },//RTD_PT100_3920

//############################
//# SensorName:RTD_PT200_3920
//############################
const float COFF_RTD_PT200_3920[3][5] =
{
    {128.06155,	0.0012829413,	3.3288292E-10,	4.3865033E-16,	1.364394E-21,	},//0.010406494
    {128.08615,	0.0012830922,	3.1785136E-10,	1.01892485E-16,	1.2841021E-22,	},//0.00919342
    {247.71118,	2.3763675E-4,	3.750529E-9,	-4.9209412E-15,	2.8947333E-21,	},//0.0010986328
};

const int32_t SEGMENT_RTD_PT200_3920[2] = {-37822, 438387, };
//{ -274169.0 , 488447.0 },//RTD_PT200_3920

//############################
//# SensorName:RTD_PT500_3920
//############################
const float COFF_RTD_PT500_3920[2][6] =
{
    {-98.88823,	4.7655648E-4,	7.5070804E-11,	-3.4578863E-17,	3.2469755E-23,	9.433436E-30,	},//0.008178711
    {-99.569595,	4.8302763E-4,	4.798213E-11,	1.9539378E-17,	-5.3691695E-24,	2.4674794E-30,	},//0.0021972656
};

const int32_t SEGMENT_RTD_PT500_3920[1] = {470228, };
//{ -220764.0 , 1578184.0 },//RTD_PT500_3920

//############################
//# SensorName:RTD_PT1000_3920
//############################
const float COFF_RTD_PT1000_3920[3][5] =
{
    {-170.26826,	2.2864982E-4,	3.012435E-11,	-6.0447758E-18,	2.125472E-24,	},//0.010284424
    {-170.03113,	2.3040317E-4,	2.4319874E-11,	-8.5727866E-19,	7.8376797E-25,	},//0.009986877
    {-96.421,	1.16788084E-4,	9.0636235E-11,	-1.8209817E-17,	2.501108E-24,	},//3.6621094E-4
};

const int32_t SEGMENT_RTD_PT1000_3920[2] = {929393, 2689761, };
//{ -132359.0 , 3154393.0 },//RTD_PT1000_3920


//############################
//# SensorName:RTD_PT100_3911
//############################
const float COFF_RTD_PT100_3911[3][5] =
{
    {571.8008,	1.9120068E-4,	1.808169E-11,	2.8604158E-18,	2.0309247E-25,	},//0.010375977
    {548.1595,	1.6238038E-4,	5.0780036E-12,	2.8981242E-19,	1.5732154E-26,	},//0.009223938
    {548.06305,	1.6258843E-4,	4.9168473E-12,	3.2094866E-19,	2.723271E-26,	},//1.8310547E-4
};

const int32_t SEGMENT_RTD_PT100_3911[2] = {-3146579, 1420656, };
//{ -5306196.0 , 1751494.0 },//RTD_PT100_3911

//############################
//# SensorName:RTD_PT200_3911
//############################
const float COFF_RTD_PT200_3911[3][5] =
{
    {128.35709,	0.0012856547,	3.3012426E-10,	4.287747E-16,	1.3711744E-21,	},//0.010391235
    {128.39607,	0.0012862597,	3.189161E-10,	1.0277135E-16,	1.2906985E-22,	},//0.00983429
    {288.71793,	-1.1684638E-4,	4.9292996E-9,	-6.642329E-15,	3.8391964E-21,	},//0.0012207031
};

const int32_t SEGMENT_RTD_PT200_3911[2] = {-41937, 440183, };
//{ -273789.0 , 487148.0 },//RTD_PT200_3911

//############################
//# SensorName:RTD_PT500_3911
//############################
const float COFF_RTD_PT500_3911[3][5] =
{
    {-99.117744,	4.775954E-4,	7.5691654E-11,	-3.5185454E-17,	3.4857312E-23,	},//0.010284424
    {-98.90642,	4.7814404E-4,	6.3515596E-11,	8.387229E-19,	5.606707E-24,	},//0.009414673
    {-63.973957,	3.730925E-4,	1.8335755E-10,	-6.063823E-17,	1.7568838E-23,	},//4.272461E-4
};

const int32_t SEGMENT_RTD_PT500_3911[2] = {337883, 1394685, };
//{ -219819.0 , 1575277.0 },//RTD_PT500_3911

//############################
//# SensorName:RTD_PT1000_3911
//############################
const float COFF_RTD_PT1000_3911[3][5] =
{
    {-170.63786,	2.290288E-4,	3.0462865E-11,	-6.2361514E-18,	2.1722131E-24,	},//0.010467529
    {-170.43744,	2.309494E-4,	2.4380713E-11,	-8.570393E-19,	7.8692006E-25,	},//0.009883881
    {-96.53809,	1.1686609E-4,	9.09932E-11,	-1.8295654E-17,	2.5138806E-24,	},//3.0517578E-4
};

const int32_t SEGMENT_RTD_PT1000_3911[2] = {928827, 2685626, };
//{ -130485.0 , 3149499.0 },//RTD_PT1000_3911


//############################
//# SensorName:RTD_PT100_3928
//############################
const float COFF_RTD_PT100_3928[3][5] =
{
    {568.9922,	1.8959158E-4,	1.7455178E-11,	2.7029018E-18,	1.8936711E-25,	},//0.010253906
    {545.5332,	1.6162438E-4,	5.0630975E-12,	2.9129044E-19,	1.6305302E-26,	},//0.009284973
    {545.2196,	1.6236003E-4,	4.4248017E-12,	5.1319774E-19,	-1.9460397E-27,	},//1.2207031E-4
};

const int32_t SEGMENT_RTD_PT100_3928[2] = {-3034077, 1497556, };
//{ -5312406.0 , 1773358.0 },//RTD_PT100_3928

//############################
//# SensorName:RTD_PT200_3928
//############################
const float COFF_RTD_PT200_3928[3][5] =
{
    {127.76518,	0.001280446,	3.3767486E-10,	4.40578E-16,	1.2971416E-21,	},//0.010391235
    {127.76979,	0.0012799146,	3.1875966E-10,	9.853641E-17,	1.3377198E-22,	},//0.009780884
    {231.20468,	3.8080695E-4,	3.2563163E-9,	-4.180581E-15,	2.4810804E-21,	},//0.0010375977
};

const int32_t SEGMENT_RTD_PT200_3928[2] = {-29722, 448355, };
//{ -274471.0 , 489462.0 },//RTD_PT200_3928

//############################
//# SensorName:RTD_PT500_3928
//############################
const float COFF_RTD_PT500_3928[4][5] =
{
    {-98.64988,	4.7559084E-4,	7.328663E-11,	-3.0800216E-17,	3.315068E-23,	},//0.010314941
    {-98.34048,	4.7535883E-4,	6.404028E-11,	1.9913845E-19,	5.7775826E-24,	},//0.009712219
    {-56934.12,	0.13158529,	-1.0751237E-7,	3.5849355E-14,	-3.7006094E-21,	},//0.017944336
    {-60.987213,	3.6347494E-4,	1.9092226E-10,	-6.438235E-17,	1.82238E-23,	},//4.8828125E-4
};

const int32_t SEGMENT_RTD_PT500_3928[3] = {357727, 1413269, 1420175, };
//{ -221515.0 , 1580455.0 },//RTD_PT500_3928

//############################
//# SensorName:RTD_PT1000_3928
//############################
const float COFF_RTD_PT1000_3928[3][5] =
{
    {-169.9356,	2.285571E-4,	2.926421E-11,	-5.627291E-18,	2.132778E-24,	},//0.010726929
    {-169.38458,	2.2929934E-4,	2.4704665E-11,	-1.0157003E-18,	8.0659287E-25,	},//0.009597778
    {-92.437744,	1.1121586E-4,	9.3218905E-11,	-1.8832146E-17,	2.5584313E-24,	},//4.272461E-4
};

const int32_t SEGMENT_RTD_PT1000_3928[2] = {966175, 2705678, };
//{ -133849.0 , 3158215.0 },//RTD_PT1000_3928


//############################
//# SensorName:RTD_PT100_3750
//############################
const float COFF_RTD_PT100_3750[2][6] =
{
    {616.72217,	2.2245695E-4,	2.9250165E-11,	5.3183586E-18,	4.7040727E-25,	1.0893752E-32,	},//0.008178711
    {575.059,	1.7177484E-4,	5.8743934E-12,	3.7382505E-19,	3.0111298E-26,	2.0004104E-33,	},//0.0014038086
};

const int32_t SEGMENT_RTD_PT100_3750[1] = {-2789332, };
//{ -5244812.0 , 1513148.0 },//RTD_PT100_3750

//############################
//# SensorName:RTD_PT200_3750
//############################
const float COFF_RTD_PT200_3750[3][5] =
{
    {133.88397,	0.0013426648,	3.6348732E-10,	4.940967E-16,	1.6066992E-21,	},//0.010375977
    {133.94931,	0.0013439186,	3.533418E-10,	1.2505929E-16,	1.6222627E-22,	},//0.009963989
    {161.56705,	0.0010837404,	1.2812932E-9,	-1.3649038E-15,	1.0741183E-21,	},//0.0010986328
};

const int32_t SEGMENT_RTD_PT200_3750[2] = {-48179, 408525, };
//{ -267049.0 , 461901.0 },//RTD_PT200_3750

//############################
//# SensorName:RTD_PT500_3750
//############################
const float COFF_RTD_PT500_3750[3][5] =
{
    {-103.12635,	4.960935E-4,	8.3708825E-11,	-4.2200627E-17,	4.1837692E-23,	},//0.010696411
    {-102.90342,	4.9682014E-4,	6.937354E-11,	2.96926E-19,	6.963079E-24,	},//0.009635925
    {-69.80401,	3.9144597E-4,	1.9646773E-10,	-6.8535666E-17,	2.1081555E-23,	},//5.493164E-4
};

const int32_t SEGMENT_RTD_PT500_3750[2] = {332412, 1339231, };
//{ -203065.0 , 1518689.0 },//RTD_PT500_3750

//############################
//# SensorName:RTD_PT1000_3750
//############################
const float COFF_RTD_PT1000_3750[2][6] =
{
    {-177.27113,	2.3687199E-4,	3.352282E-11,	-6.3653136E-18,	6.9283854E-25,	9.937816E-31,	},//0.00680542
    {-181.95903,	2.5382842E-4,	9.450469E-12,	8.602534E-18,	-1.7989843E-24,	3.0106444E-31,	},//0.0038452148
};

const int32_t SEGMENT_RTD_PT1000_3750[1] = {1156675, };
//{ -97290.0 , 3053986.0 },//RTD_PT1000_3750


//Ni6180
//############################
//# SensorName:RTD_NI100_6180
//############################
const float COFF_RTD_NI100_6180[2][5] =
{
    {152.2762,	-8.3258274E-5,	-6.5809025E-11,	-1.25888866E-17,	-1.032012E-24,	},//0.01121521
    {259.177,	4.8968603E-5,	-4.164732E-12,	2.7304415E-19,	-1.6461353E-26,	},//0.0052490234
};

const int32_t SEGMENT_RTD_NI100_6180[1] = {-2891630, };
//{ -4313708.0 , -184540.0 },//RTD_NI100_6180


//############################
//# SensorName:RTD_NI120_6180
//############################
const float COFF_RTD_NI120_6180[2][5] =
{
    {182.42105,	8.4803E-6,	-2.504666E-11,	-5.079584E-18,	-5.0037595E-25,	},//0.011169434
    {209.94191,	4.8113427E-5,	-3.492277E-12,	1.931959E-19,	-7.79045E-27,	},//0.0052337646
};

const int32_t SEGMENT_RTD_NI120_6180[1] = {-2347690, };
//{ -4050552.0 , 886903.0 },//RTD_NI120_6180

//############################
//# SensorName:RTD_NI500_6180
//############################
const float COFF_RTD_NI500_6180[2][5] =
{
    {-81.06883,	4.4784334E-4,	-2.7929134E-10,	3.0217059E-16,	-1.9981998E-22,	},//0.010215759
    {-76.63645,	4.0626287E-4,	-1.2250169E-10,	2.1648127E-17,	-1.6835538E-24,	},//0.004951477
};

const int32_t SEGMENT_RTD_NI500_6180[1] = {423393, };
//{ 48458.0 , 1107881.0 },//RTD_NI500_6180


//############################
//# SensorName:RTD_NI1000_6180
//############################
const float COFF_RTD_NI1000_6180[2][5] =
{
    {-157.87207,	2.841575E-4,	-1.11547625E-10,	5.5445235E-17,	-1.3476038E-23,	},//0.010772705
    {-141.9127,	2.2568917E-4,	-2.8477698E-11,	1.3956316E-18,	3.7718108E-26,	},//0.0038757324
};

const int32_t SEGMENT_RTD_NI1000_6180[1] = {1137035, };
//{ 394798.0 , 2346120.0 },//RTD_NI1000_6180


//############################
//# SensorName:RTD_NI100_6720
//############################
const float COFF_RTD_NI100_6720[2][5] =
{
    {235.31311,	3.4098008E-5,	-1.2045135E-11,	-2.1935399E-18,	-2.7720767E-25,	},//0.010498047
    {246.6609,	5.0855142E-5,	-2.6751015E-12,	1.7618762E-19,	-4.6347393E-26,	},//0.0015487671
};

const int32_t SEGMENT_RTD_NI100_6720[1] = {-2183153, };
//{ -4353326.0 , 65857.0 },//RTD_NI100_6720

//############################
//# SensorName:RTD_NI120_6720
//############################
const float COFF_RTD_NI120_6720[2][5] =
{
    {194.89737,	4.3329423E-5,	-5.1037967E-12,	-6.7078775E-19,	-1.339351E-25,	},//0.010467529
    {197.01274,	4.7249636E-5,	-2.3671117E-12,	2.0230672E-19,	-2.2497048E-26,	},//0.0015411377
};

const int32_t SEGMENT_RTD_NI120_6720[1] = {-1500274, };
//{ -4098028.0 , 1185630.0 },//RTD_NI120_6720

//############################
//# SensorName:RTD_NI500_6720
//############################
const float COFF_RTD_NI500_6720[2][5] =
{
    {-75.22171,	4.1007198E-4,	-2.076202E-10,	1.3157005E-16,	-4.7124764E-23,	},//0.01108551
    {-71.23734,	3.8405406E-4,	-1.4032069E-10,	5.0133538E-17,	-8.6344526E-24,	},//8.31604E-4
};

const int32_t SEGMENT_RTD_NI500_6720[1] = {644771, };
//{ 37855.0 , 1169340.0 },//RTD_NI500_6720

//############################
//# SensorName:RTD_NI1000_6720
//############################
const float COFF_RTD_NI1000_6720[2][5] =
{
    {-143.78574,	2.461478E-4,	-6.6447146E-11,	1.9684794E-17,	-2.8949598E-24,	},//0.011100769
    {-135.14468,	2.2231204E-4,	-4.1030793E-11,	7.315973E-18,	-5.8996176E-25,	},//3.9672852E-4
};

const int32_t SEGMENT_RTD_NI1000_6720[1] = {1588268, };
//{ 374288.0 , 2453669.0 },//RTD_NI1000_6720

//############################
//# SensorName:RTD_CU50_428
//############################
const float COFF_RTD_CU50_428[2][5] =
{
    {764.4549,	-2.593505E-5,	-6.45788E-11,	-5.4779627E-18,	-6.6496894E-26,	},//4.7683716E-4
    {764.4549,	-2.593505E-5,	-6.45788E-11,	-5.4779627E-18,	-6.6496894E-26,	},//4.7683716E-4
};

const int32_t SEGMENT_RTD_CU50_428[1] = {-4500000};
//{ -4888274.0 , -4075469.0 },//RTD_CU50_428


//############################
//# SensorName:RTD_CU100_428
//############################
const float COFF_RTD_CU100_428[2][5] =
{
    {447.67142,	1.07854736E-4,	-5.4587645E-12,	-6.327737E-19,	-6.613254E-27,	},//3.8146973E-4
    {447.67142,	1.07854736E-4,	-5.4587645E-12,	-6.327737E-19,	-6.613254E-27,	},//3.8146973E-4
};

const int32_t SEGMENT_RTD_CU100_428[1] = {-3000000};
//{ -4144486.0 , -2528664.0 },//RTD_CU100_428


//############################
//# SensorName:RTD_CU1000_428
//############################
const float COFF_RTD_CU1000_428[2][5] =
{
    {-160.68225,	2.2035812E-4,	2.0079823E-11,	-1.3846823E-18,	-6.887329E-25,	},//3.6621094E-4
    {-160.68225,	2.2035812E-4,	2.0079823E-11,	-1.3846823E-18,	-6.887329E-25,	},//3.6621094E-4
};

const int32_t SEGMENT_RTD_CU1000_428[1] = {1000000};
//{ 481988.0 , 1281842.0 },//RTD_CU1000_428

//############################
//# SensorName:RTD_R600
//############################
const float COFF_RTD_R600[2][6] =
{
    {300.0,	5.4037457E-5,	1.4384454E-13,	3.8298117E-22,	1.026172E-30,	9.0937E-40,	},//6.1035156E-5
    {300.0,	5.4037457E-5,	1.4384454E-13,	3.8298117E-22,	1.026172E-30,	9.0937E-40,	},//6.1035156E-5
};

const int32_t SEGMENT_RTD_R600[1] = {0};
//{ -5634980.0 , 5469057.0 },//RTD_R600


//############################
//# SensorName:RTD_R4000
//############################
const float COFF_RTD_R4000[3][5] =
{
    {299.99622,	9.824899E-4,	4.7628127E-11,	2.2040658E-18,	1.542912E-25,	},//0.010156244
    {-14969.357,	0.021815464,	-1.0608732E-8,	2.4242212E-15,	-2.0622547E-22,	},//0.017578125
    {256.50327,	0.001033468,	2.5614365E-11,	6.312214E-18,	-1.1983973E-25,	},//9.765625E-4
};

const int32_t SEGMENT_RTD_R4000[2] = {2821085, 3049570, };
//{ -309924.0 , 3185329.0 },//RTD_R4000



//############################
//# SensorName:RTD_PT50_3851
//############################
const float COFF_RTD_PT50_3851[3][5] =
{
    {2992.8396,	0.0016717594,	4.3673326E-10,	6.2079124E-17,	3.3976547E-24,	},//0.0105896
    {1688.6249,	5.108837E-4,	5.0084582E-11,	4.9723117E-18,	2.422039E-25,	},//0.009475708
    {1636.1445,	4.013527E-4,	-3.4395726E-11,	-2.367842E-17,	-3.372588E-24,	},//2.4414062E-4
};

const int32_t SEGMENT_RTD_PT50_3851[2] = {-4409359, -2100879, };
//{ -5458519.0 , -1948975.0 },//RTD_PT50_3851

//------------------------------------------
//ADC works in two ranges, one is +/-100mV
//the other is +/-1000mV, For two ranges, we have to
//calibrate seperately to ensure the precision
//This is for bridge leg=300ohm
//------------------------------------------
const int32_t PROGMEM_CALIBPOINT_IDEAL_VALUE[SENSORTYPE_NUM][2] =
{
    { -3737963, -927568}, //ADCSPAN_100MV
    { -205588, 2425492 }, //ADCSPAN_1000MV
};

////------------------------------------------
////ADC code range for wheatstone bridge one leg=300ohm, this is ideal value, for use of OVER RANGE detection
////------------------------------------------
const int32_t PROGMEM_SENSOR_ADC_RANGE[SENSORTYPE_NUM][2] =
{
    { -5282221.0, 1666448.0 }, //RTD_PT100_3851
    { -271156.0, 478143.0 }, //RTD_PT200_3851
    { -213273.0, 1555116.0 }, //RTD_PT500_3851
    { -117509.0, 3115524.0 }, //RTD_PT1000_3851

    { -5308904.0, 1756102.0 }, //RTD_PT100_3916
    { -274086.0, 487635.0 }, //RTD_PT200_3916
    { -220559.0, 1576369.0 }, //RTD_PT500_3916
    { -131952.0, 3151337.0 }, //RTD_PT1000_3916

    { -5309656.0, 1763767.0 }, //RTD_PT100_3920
    { -274169.0, 488447.0 }, //RTD_PT200_3920
    { -220764.0, 1578184.0 }, //RTD_PT500_3920
    { -132359.0, 3154393.0 }, //RTD_PT1000_3920

    { -5306196.0, 1751494.0 }, //RTD_PT100_3911
    { -273789.0, 487148.0 }, //RTD_PT200_3911
    { -219819.0, 1575277.0 }, //RTD_PT500_3911
    { -130485.0, 3149499.0 }, //RTD_PT1000_3911

    { -5312406.0, 1773358.0 }, //RTD_PT100_3928
    { -274471.0, 489462.0 }, //RTD_PT200_3928
    { -221515.0, 1580455.0 }, //RTD_PT500_3928
    { -133849.0, 3158215.0 }, //RTD_PT1000_3928

    { -5244812.0, 1513148.0 }, //RTD_PT100_3750
    { -267049.0, 461901.0 }, //RTD_PT200_3750
    { -203065.0, 1518689.0 }, //RTD_PT500_3750
    { -97290.0, 3053986.0 }, //RTD_PT1000_3750

    { -4313708.0, -184540.0 }, //RTD_NI100_6180
    { -4050552.0, 886903.0 }, //RTD_NI120_6180
    { 48458.0, 1107881.0 }, //RTD_NI500_6180
    { 394798.0, 2346120.0 }, //RTD_NI1000_6180

    { -4353326.0, 65857.0 }, //RTD_NI100_6720
    { -4098028.0, 1185630.0 }, //RTD_NI120_6720
    { 37855.0, 1169340.0 }, //RTD_NI500_6720
    { 374288.0, 2453669.0 }, //RTD_NI1000_6720

    { -4888274.0, -4075469.0 }, //RTD_CU50_428
    { -4144486.0, -2528664.0 }, //RTD_CU100_428
    { 481988.0, 1281842.0 }, //RTD_CU1000_428

    { -5634980.0, 5469057.0 }, //RTD_R600
    { -309924.0, 3185329.0 }, //RTD_R4000

    { -5458519.0, -1948975.0 }, //RTD_PT50_3851
};

//------------------------------------------
//Sensor range, for use of OVER RANGE detection
//The value is (original_value * 10)
//------------------------------------------
const int16_t PROGMEM_SENSOR_RESULT_RANGE[SENSORTYPE_NUM][2] =
{
    { -2000, 8500 },//SENSORTYPE_PT100_3851
    { -2000, 8500 },//SENSORTYPE_PT200_3851
    { -2000, 8500 },//SENSORTYPE_PT500_3851
    { -2000, 8500 }, //SENSORTYPE_PT1000_3851

    { -2000, 8500 },//SENSORTYPE_PT100_3916
    { -2000, 8500 },//SENSORTYPE_PT200_3916
    { -2000, 8500 },//SENSORTYPE_PT500_3916
    { -2000, 8500 }, //SENSORTYPE_PT1000_3916

    { -2000, 8500 },//SENSORTYPE_PT100_3920
    { -2000, 8500 },//SENSORTYPE_PT200_3920
    { -2000, 8500 },//SENSORTYPE_PT500_3920
    { -2000, 8500 }, //SENSORTYPE_PT1000_3920

    { -2000, 8500 },//SENSORTYPE_PT100_3911
    { -2000, 8500 },//SENSORTYPE_PT200_3911
    { -2000, 8500 },//SENSORTYPE_PT500_3911
    { -2000, 8500 }, //SENSORTYPE_PT1000_3911

    { -2000, 8500 },//SENSORTYPE_PT100_3928
    { -2000, 8500 },//SENSORTYPE_PT200_3928
    { -2000, 8500 },//SENSORTYPE_PT500_3928
    { -2000, 8500 }, //SENSORTYPE_PT1000_3928

    { -2000, 8500 },//SENSORTYPE_PT100_3750
    { -2000, 8500 },//SENSORTYPE_PT200_3750
    { -2000, 8500 },//SENSORTYPE_PT500_3750
    { -2000, 8500 }, //SENSORTYPE_PT1000_3750

    { -600, 2500 },//SENSORTYPE_NI100_6180
    { -600, 2500 },//SENSORTYPE_NI120_6180
    { -600, 2500 },//SENSORTYPE_NI500_6180
    { -600, 2500 }, //SENSORTYPE_NI1000_6180

    { -600, 2500 },//SENSORTYPE_NI100_6720
    { -600, 2500 },//SENSORTYPE_NI120_6720
    { -600, 2500 },//SENSORTYPE_NI500_6720
    { -600, 2500 }, //SENSORTYPE_NI1000_6720

    { -500, 1500 }, //SENSORTYPE_CU50_428
    { -500, 1500 },//SENSORTYPE_CU100_428
    { -500, 1500 }, //SENSORTYPE_CU1000_428

    { 0, 6000 }, //SENSORTYPE_R600
    { 0, (int16_t)40000 }, //SENSORTYPE_R40000

    { -2000, 8500 }, //SENSORTYPE_PT50_3851
};

//------------------------------------------
// Calib Circuit Zero Value
//------------------------------------------
int32_t adcCalibSystemOffset(STRUCT_CHANNEL_ITEM_CONFIG* pChannelItemConfig, int32_t systemOffset, int32_t adcValue)
{
    uint8_t RtdType = pChannelItemConfig->RtdType;

    switch(RtdType)
    {
    case SENSORTYPE_PT200_3851:
    case SENSORTYPE_PT500_3851:
    case SENSORTYPE_PT1000_3851:
    case SENSORTYPE_PT200_3916:
    case SENSORTYPE_PT500_3916:
    case SENSORTYPE_PT1000_3916:
    case SENSORTYPE_PT200_3920:
    case SENSORTYPE_PT500_3920:
    case SENSORTYPE_PT1000_3920:
    case SENSORTYPE_PT200_3911:
    case SENSORTYPE_PT500_3911:
    case SENSORTYPE_PT1000_3911:
    case SENSORTYPE_PT200_3928:
    case SENSORTYPE_PT500_3928:
    case SENSORTYPE_PT1000_3928:
    case SENSORTYPE_PT200_3750:
    case SENSORTYPE_PT500_3750:
    case SENSORTYPE_PT1000_3750:
    case SENSORTYPE_NI500_6180:
    case SENSORTYPE_NI1000_6180:
    case SENSORTYPE_NI500_6720:
    case SENSORTYPE_NI1000_6720:
    case SENSORTYPE_CU1000_428:
    case SENSORTYPE_R4000:
    {
        systemOffset = systemOffset / 10;//circuitZero is measured at 100mV mode
        break;
    }
    //	case SENSORTYPE_PT100_3851:
    //	case SENSORTYPE_PT100_3916:
    //	case SENSORTYPE_PT100_3920:
    //	case SENSORTYPE_PT100_3911:
    //	case SENSORTYPE_PT100_3928:
    //	case SENSORTYPE_PT100_3750:
    //	case SENSORTYPE_NI100_6180:
    //	case SENSORTYPE_NI120_6180:
    //	case SENSORTYPE_NI100_6720:
    //	case SENSORTYPE_NI120_6720:
    //	case SENSORTYPE_CU50_428:
    //	case SENSORTYPE_CU100_428:
    //	case SENSORTYPE_R400: {
    //		break;
    //	}
    default:
        break;
    }
    return (adcValue - systemOffset);
}

//------------------------------------------
// Calib Full and Zero Value
//------------------------------------------
int32_t adcTwoPointCalib(STRUCT_CHANNEL_ITEM_CONFIG* pChannelItemConfig, int32_t value)
{
    int32_t idealZero = 0, idealFull = 0, realZero = 0, realFull = 0;
    uint8_t RtdType = pChannelItemConfig->RtdType;

    switch(RtdType)
    {
    case SENSORTYPE_PT200_3851:
    case SENSORTYPE_PT500_3851:
    case SENSORTYPE_PT1000_3851:
    case SENSORTYPE_PT200_3916:
    case SENSORTYPE_PT500_3916:
    case SENSORTYPE_PT1000_3916:
    case SENSORTYPE_PT200_3920:
    case SENSORTYPE_PT500_3920:
    case SENSORTYPE_PT1000_3920:
    case SENSORTYPE_PT200_3911:
    case SENSORTYPE_PT500_3911:
    case SENSORTYPE_PT1000_3911:
    case SENSORTYPE_PT200_3928:
    case SENSORTYPE_PT500_3928:
    case SENSORTYPE_PT1000_3928:
    case SENSORTYPE_PT200_3750:
    case SENSORTYPE_PT500_3750:
    case SENSORTYPE_PT1000_3750:
    case SENSORTYPE_NI500_6180:
    case SENSORTYPE_NI1000_6180:
    case SENSORTYPE_NI500_6720:
    case SENSORTYPE_NI1000_6720:
    case SENSORTYPE_CU1000_428:
    case SENSORTYPE_R4000:
    {
        realZero = pChannelItemConfig->calibPoint1000mv_1;
        realFull = pChannelItemConfig->calibPoint1000mv_2;
        idealZero = PROGMEM_CALIBPOINT_IDEAL_VALUE[ADCSPAN_1000MV][0];
        idealFull = PROGMEM_CALIBPOINT_IDEAL_VALUE[ADCSPAN_1000MV][1];
        break;
    }
    //	case SENSORTYPE_PT100_3851:
    //	case SENSORTYPE_PT100_3916:
    //	case SENSORTYPE_PT100_3920:
    //	case SENSORTYPE_PT100_3911:
    //	case SENSORTYPE_PT100_3928:
    //	case SENSORTYPE_PT100_3750:
    //	case SENSORTYPE_NI100_6180:
    //	case SENSORTYPE_NI120_6180:
    //	case SENSORTYPE_NI100_6720:
    //	case SENSORTYPE_NI120_6720:
    //	case SENSORTYPE_CU50_428:
    //	case SENSORTYPE_CU100_428:
    //	case SENSORTYPE_R400:
    default:
    {
        realZero = pChannelItemConfig->calibPoint100mv_1;
        realFull = pChannelItemConfig->calibPoint100mv_2;
        idealZero = PROGMEM_CALIBPOINT_IDEAL_VALUE[ADCSPAN_100MV][0];
        idealFull = PROGMEM_CALIBPOINT_IDEAL_VALUE[ADCSPAN_100MV][1];
        break;
    }
    }

    float valueCalibed = ((float) value - (float) realZero) / ((float) realFull - (float) realZero)
                         * (idealFull - idealZero) + idealZero;
    return (int32_t) valueCalibed;
}

//------------------------------------------
// Search Segment Index
//------------------------------------------
uint8_t adcSearchPloyfitSegmentIdx(const int32_t* segment, uint8_t segmentSize, int32_t n)
{
    uint8_t i;
    int32_t seperator;

    seperator = segment[0];
    if(n <= seperator)
    {
        return 0;
    }
    seperator = segment[segmentSize - 1];
    if(n >= seperator)
    {
        return segmentSize;
    }

    for(i = 0; i < segmentSize - 1; i++)
    {
        int32_t low = segment[i];
        int32_t high = segment[i + 1];
        if(n >= low && n <= high)
        {
            return i + 1;
        }
    }
    return 0;
}

//------------------------------------------
// Caculate Channel Value
//------------------------------------------
float adcCalcSensorValue(STRUCT_CHANNEL_ITEM_CONFIG* pChannelItemConfig, int32_t adcVaule)
{
    uint8_t RtdType = pChannelItemConfig->RtdType;
    uint8_t segmentIdx = 0;
    float dataValue = 0;

    switch(RtdType)
    {
    case SENSORTYPE_PT100_3851:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT100_3851,
                                                sizeof(SEGMENT_RTD_PT100_3851) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT100_3851[segmentIdx],
                                            sizeof(COFF_RTD_PT100_3851[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT200_3851:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT200_3851,
                                                sizeof(SEGMENT_RTD_PT200_3851) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT200_3851[segmentIdx],
                                            sizeof(COFF_RTD_PT200_3851[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT500_3851:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT500_3851,
                                                sizeof(SEGMENT_RTD_PT500_3851) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT500_3851[segmentIdx],
                                            sizeof(COFF_RTD_PT500_3851[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT1000_3851:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT1000_3851,
                                                sizeof(SEGMENT_RTD_PT1000_3851) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT1000_3851[segmentIdx],
                                            sizeof(COFF_RTD_PT1000_3851[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }

    case SENSORTYPE_PT100_3916:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT100_3916,
                                                sizeof(SEGMENT_RTD_PT100_3916) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT100_3916[segmentIdx],
                                            sizeof(COFF_RTD_PT100_3916[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT200_3916:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT200_3916,
                                                sizeof(SEGMENT_RTD_PT200_3916) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT200_3916[segmentIdx],
                                            sizeof(COFF_RTD_PT200_3916[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT500_3916:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT500_3916,
                                                sizeof(SEGMENT_RTD_PT500_3916) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT500_3916[segmentIdx],
                                            sizeof(COFF_RTD_PT500_3916[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT1000_3916:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT1000_3916,
                                                sizeof(SEGMENT_RTD_PT1000_3916) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT1000_3916[segmentIdx],
                                            sizeof(COFF_RTD_PT1000_3916[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }

    case SENSORTYPE_PT100_3920:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT100_3920,
                                                sizeof(SEGMENT_RTD_PT100_3920) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT100_3920[segmentIdx],
                                            sizeof(COFF_RTD_PT100_3920[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT200_3920:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT200_3920,
                                                sizeof(SEGMENT_RTD_PT200_3920) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT200_3920[segmentIdx],
                                            sizeof(COFF_RTD_PT200_3920[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT500_3920:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT500_3920,
                                                sizeof(SEGMENT_RTD_PT500_3920) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT500_3920[segmentIdx],
                                            sizeof(COFF_RTD_PT500_3920[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT1000_3920:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT1000_3920,
                                                sizeof(SEGMENT_RTD_PT1000_3920) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT1000_3920[segmentIdx],
                                            sizeof(COFF_RTD_PT1000_3920[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }

    case SENSORTYPE_PT100_3911:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT100_3911,
                                                sizeof(SEGMENT_RTD_PT100_3911) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT100_3911[segmentIdx],
                                            sizeof(COFF_RTD_PT100_3911[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT200_3911:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT200_3911,
                                                sizeof(SEGMENT_RTD_PT200_3911) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT200_3911[segmentIdx],
                                            sizeof(COFF_RTD_PT200_3911[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT500_3911:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT500_3911,
                                                sizeof(SEGMENT_RTD_PT500_3911) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT500_3911[segmentIdx],
                                            sizeof(COFF_RTD_PT500_3911[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT1000_3911:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT1000_3911,
                                                sizeof(SEGMENT_RTD_PT1000_3911) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT1000_3911[segmentIdx],
                                            sizeof(COFF_RTD_PT1000_3911[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }

    case SENSORTYPE_PT100_3928:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT100_3928,
                                                sizeof(SEGMENT_RTD_PT100_3928) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT100_3928[segmentIdx],
                                            sizeof(COFF_RTD_PT100_3928[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT200_3928:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT200_3928,
                                                sizeof(SEGMENT_RTD_PT200_3928) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT200_3928[segmentIdx],
                                            sizeof(COFF_RTD_PT200_3928[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT500_3928:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT500_3928,
                                                sizeof(SEGMENT_RTD_PT500_3928) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT500_3928[segmentIdx],
                                            sizeof(COFF_RTD_PT500_3928[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT1000_3928:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT1000_3928,
                                                sizeof(SEGMENT_RTD_PT1000_3928) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT1000_3928[segmentIdx],
                                            sizeof(COFF_RTD_PT1000_3928[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }

    case SENSORTYPE_PT100_3750:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT100_3750,
                                                sizeof(SEGMENT_RTD_PT100_3750) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT100_3750[segmentIdx],
                                            sizeof(COFF_RTD_PT100_3750[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT200_3750:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT200_3750,
                                                sizeof(SEGMENT_RTD_PT200_3750) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT200_3750[segmentIdx],
                                            sizeof(COFF_RTD_PT200_3750[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT500_3750:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT500_3750,
                                                sizeof(SEGMENT_RTD_PT500_3750) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT500_3750[segmentIdx],
                                            sizeof(COFF_RTD_PT500_3750[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT1000_3750:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT1000_3750,
                                                sizeof(SEGMENT_RTD_PT1000_3750) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT1000_3750[segmentIdx],
                                            sizeof(COFF_RTD_PT1000_3750[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }

    case SENSORTYPE_NI100_6180:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_NI100_6180,
                                                sizeof(SEGMENT_RTD_NI100_6180) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_NI100_6180[segmentIdx],
                                            sizeof(COFF_RTD_NI100_6180[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_NI120_6180:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_NI120_6180,
                                                sizeof(SEGMENT_RTD_NI120_6180) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_NI120_6180[segmentIdx],
                                            sizeof(COFF_RTD_NI120_6180[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_NI500_6180:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_NI500_6180,
                                                sizeof(SEGMENT_RTD_NI500_6180) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_NI500_6180[segmentIdx],
                                            sizeof(COFF_RTD_NI500_6180[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_NI1000_6180:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_NI1000_6180,
                                                sizeof(SEGMENT_RTD_NI1000_6180) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_NI1000_6180[segmentIdx],
                                            sizeof(COFF_RTD_NI1000_6180[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }

    case SENSORTYPE_NI100_6720:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_NI100_6720,
                                                sizeof(SEGMENT_RTD_NI100_6720) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_NI100_6720[segmentIdx],
                                            sizeof(COFF_RTD_NI100_6720[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_NI120_6720:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_NI120_6720,
                                                sizeof(SEGMENT_RTD_NI120_6720) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_NI120_6720[segmentIdx],
                                            sizeof(COFF_RTD_NI120_6720[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_NI500_6720:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_NI500_6720,
                                                sizeof(SEGMENT_RTD_NI500_6720) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_NI500_6720[segmentIdx],
                                            sizeof(COFF_RTD_NI500_6720[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_NI1000_6720:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_NI1000_6720,
                                                sizeof(SEGMENT_RTD_NI1000_6720) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_NI1000_6720[segmentIdx],
                                            sizeof(COFF_RTD_NI1000_6720[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }

    case SENSORTYPE_CU50_428:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_CU50_428, sizeof(SEGMENT_RTD_CU50_428)
                                                / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_CU50_428[segmentIdx],
                                            sizeof(COFF_RTD_CU50_428[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_CU100_428:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_CU100_428,
                                                sizeof(SEGMENT_RTD_CU100_428) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_CU100_428[segmentIdx],
                                            sizeof(COFF_RTD_CU100_428[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_CU1000_428:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_CU1000_428,
                                                sizeof(SEGMENT_RTD_CU1000_428) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_CU1000_428[segmentIdx],
                                            sizeof(COFF_RTD_CU1000_428[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_R600:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_R600, sizeof(SEGMENT_RTD_R600)
                                                / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_R600[segmentIdx], sizeof(COFF_RTD_R600[segmentIdx])
                                            / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_R4000:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_R4000, sizeof(SEGMENT_RTD_R4000)
                                                / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_R4000[segmentIdx], sizeof(COFF_RTD_R4000[segmentIdx])
                                            / sizeof(float), adcVaule);
        break;
    }
    case SENSORTYPE_PT50_3851:
    {
        //check ADC value over range
        segmentIdx = adcSearchPloyfitSegmentIdx(SEGMENT_RTD_PT50_3851,
                                                sizeof(SEGMENT_RTD_PT50_3851) / sizeof(int32_t), adcVaule);
        dataValue = adcCalcPolynominalValue(COFF_RTD_PT50_3851[segmentIdx],
                                            sizeof(COFF_RTD_PT50_3851[segmentIdx]) / sizeof(float), adcVaule);
        break;
    }
    default:
        break;
    }
    adcVaule = dataValue * 10 + 0.5;//round up
    return adcVaule;
}

//------------------------------------------
// Channel value after calibration should be inside the sensor range..
// return calibed value
//------------------------------------------
int16_t adcCalcEngCalibedSensorValue(STRUCT_CHANNEL_ITEM_CONFIG* pChannelItemConfig, int16_t chnlValue)
{
    //must use long value to hold values, cause it's two int's sum or sub,maybe overflow
    uint8_t RtdType = pChannelItemConfig->RtdType;
    int16_t calibValue = pChannelItemConfig->TOffset;
    int16_t calibedValue = chnlValue + calibValue;
    int16_t valueMin = (int16_t) PROGMEM_SENSOR_RESULT_RANGE[RtdType][0];
    int16_t valueMax = (int16_t) PROGMEM_SENSOR_RESULT_RANGE[RtdType][1];

    if(calibedValue < valueMin)
    {
        calibedValue = valueMin;
    }
    if(calibedValue > valueMax)
    {
        calibedValue = valueMax;
    }
    return calibedValue;
}

//------------------------------------------
// Check Channel Over Range
// Return: -1:Low, 0:Normal, +1:High
//------------------------------------------
int16_t adcCheckAdcOverRange(STRUCT_CHANNEL_ITEM_CONFIG* pChannelItemConfig, int32_t adcVaule)
{
    int32_t buffer = 0;
    uint8_t RtdType = pChannelItemConfig->RtdType;
    switch(RtdType)
    {
    case SENSORTYPE_PT100_3851:
    case SENSORTYPE_PT100_3916:
    case SENSORTYPE_PT100_3920:
    case SENSORTYPE_PT100_3911:
    case SENSORTYPE_PT100_3928:
    case SENSORTYPE_PT100_3750:
    case SENSORTYPE_NI100_6180:
    case SENSORTYPE_NI120_6180:
    case SENSORTYPE_NI100_6720:
    case SENSORTYPE_NI120_6720:
    case SENSORTYPE_CU50_428:
    case SENSORTYPE_CU100_428:
    case SENSORTYPE_R600:
    case SENSORTYPE_PT50_3851:
    {
        //buffer = 1000000;
        break;
    }
    case SENSORTYPE_PT200_3851:
    case SENSORTYPE_PT500_3851:
    case SENSORTYPE_PT1000_3851:
    case SENSORTYPE_PT200_3916:
    case SENSORTYPE_PT500_3916:
    case SENSORTYPE_PT1000_3916:
    case SENSORTYPE_PT200_3920:
    case SENSORTYPE_PT500_3920:
    case SENSORTYPE_PT1000_3920:
    case SENSORTYPE_PT200_3911:
    case SENSORTYPE_PT500_3911:
    case SENSORTYPE_PT1000_3911:
    case SENSORTYPE_PT200_3928:
    case SENSORTYPE_PT500_3928:
    case SENSORTYPE_PT1000_3928:
    case SENSORTYPE_PT200_3750:
    case SENSORTYPE_PT500_3750:
    case SENSORTYPE_PT1000_3750:
    case SENSORTYPE_NI500_6180:
    case SENSORTYPE_NI1000_6180:
    case SENSORTYPE_NI500_6720:
    case SENSORTYPE_NI1000_6720:
    case SENSORTYPE_CU1000_428:
    case SENSORTYPE_R4000:
    {
        //buffer = 100000;
        break;
    }
    default:
        break;
    }

    int32_t valMin = PROGMEM_SENSOR_ADC_RANGE[RtdType][0] - buffer;
    int32_t valMax = PROGMEM_SENSOR_ADC_RANGE[RtdType][1] + buffer;

    if(adcVaule < valMin)
    {
        return -1;
    }
    if(adcVaule > valMax)
    {
        return 1;
    }
    return 0;
}

//------------------------------------------
// Check Channel Value Over Range
// Return: -1:Low, 0:Normal, +1:High
//------------------------------------------
int16_t adcCheckSensorValueOverRange(STRUCT_CHANNEL_ITEM_CONFIG* pChannelItemConfig, int16_t value)
{
    uint8_t RtdType = pChannelItemConfig->RtdType;

    switch(RtdType)
    {
    case SENSORTYPE_R4000:
    {
        uint16_t valueMin = PROGMEM_SENSOR_RESULT_RANGE[RtdType][0];
        uint16_t valueMax = PROGMEM_SENSOR_RESULT_RANGE[RtdType][1];
        uint16_t valueUnit16 = (uint16_t) value;
        if(valueUnit16 < valueMin)
        {
            return -1;
        }
        if(valueUnit16 > valueMax)
        {
            return 1;
        }
        return 0;
        //break;
    }
    default:
    {
        int16_t valueMin = PROGMEM_SENSOR_RESULT_RANGE[RtdType][0];
        int16_t valueMax = PROGMEM_SENSOR_RESULT_RANGE[RtdType][1];

        if(value < valueMin)
        {
            return -1;
        }
        if(value > valueMax)
        {
            return 1;
        }
        return 0;
        //break;
    }
    }

}

const STRUCT_SENSORTYPE_ADCCONFIG PROGMEM_STRUCT_SENSORTYPE_ADCCONFIG[SENSORTYPE_NUM] =
{
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
    {ADS1220_GAIN_1, ADS1220_BYPASSPGA_ENABLE},
};

STRUCT_SENSORTYPE_CHANNEL_ADCCONFIG GetSensorTypeChannelAdcConfig(int16_t RtdType, int16_t ChnlIndex)
{
    STRUCT_SENSORTYPE_ADCCONFIG* pSensorTypeAdcConfig = (STRUCT_SENSORTYPE_ADCCONFIG*)&PROGMEM_STRUCT_SENSORTYPE_ADCCONFIG[RtdType];

    STRUCT_SENSORTYPE_CHANNEL_ADCCONFIG SensorTypeChannelAdcConfig;
    SensorTypeChannelAdcConfig.Gain = pSensorTypeAdcConfig->Gain;
    SensorTypeChannelAdcConfig.BypassPGA = pSensorTypeAdcConfig->BypassPGA;
    switch(ChnlIndex)
    {
    case 0:
    {
        SensorTypeChannelAdcConfig.Mux = ADS1220_MUX_0_1;
        break;
    }
    case 1:
    {
        SensorTypeChannelAdcConfig.Mux = ADS1220_MUX_2_3;
        break;
    }
    default:
    {
        SensorTypeChannelAdcConfig.Mux = ADS1220_MUX_0_1;
        break;
    }
    }
    return SensorTypeChannelAdcConfig;
}
