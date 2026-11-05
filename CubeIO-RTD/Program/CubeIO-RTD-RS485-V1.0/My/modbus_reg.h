#ifndef MODBUS_MBR_H_
#define MODBUS_MBR_H_
#include "sensor_static_data.h"

//------------------------------------------
//公开寄存器
//------------------------------------------
//----------------------
//数据寄存器-16位整型
//----------------------
#define MBR_MSV_NUM		    					SENSOR_CHANNEL_SIZE_MAXSUPPORT
#define MBR_MSV_BASE               	0x0000
#define MBR_MSV_CHN(n)              (MBR_MSV_BASE + (n))
#define MBR_MSV_CHN00               MBR_MSV_CHN(0x0000)
#define MBR_MSV_CHN01               MBR_MSV_CHN(0x0001)
#define MBR_MSV_CHN02               MBR_MSV_CHN(0x0002)
#define MBR_MSV_CHN03               MBR_MSV_CHN(0x0003)
#define MBR_MSV_CHN04               MBR_MSV_CHN(0x0004)
#define MBR_MSV_CHN05               MBR_MSV_CHN(0x0005)
#define MBR_MSV_CHN06               MBR_MSV_CHN(0x0006)
#define MBR_MSV_CHN07               MBR_MSV_CHN(0x0007)
#define MBR_MSV_CHN08               MBR_MSV_CHN(0x0008)
#define MBR_MSV_CHN09               MBR_MSV_CHN(0x0009)
#define MBR_MSV_CHN10               MBR_MSV_CHN(0x000A)
#define MBR_MSV_CHN11               MBR_MSV_CHN(0x000B)
#define MBR_MSV_CHN12               MBR_MSV_CHN(0x000C)
#define MBR_MSV_CHN13               MBR_MSV_CHN(0x000D)
#define MBR_MSV_CHN14               MBR_MSV_CHN(0x000E)
#define MBR_MSV_CHN15               MBR_MSV_CHN(0x000F)

//----------------------
// 数据寄存器-浮点（每个float占用2个modbus寄存器，步长*2）
//----------------------
#define MBR_MSV_FLOAT_NUM		    				(SENSOR_CHANNEL_SIZE_MAXSUPPORT*2)
#define MBR_MSV_FLOAT_BASE              0x1000
#define MBR_MSV_FLOAT_CHN(n)            (MBR_MSV_FLOAT_BASE + (n)*2)
#define MBR_MSV_FLOAT_CHN00            	MBR_MSV_FLOAT_CHN(0x0000)
#define MBR_MSV_FLOAT_CHN01            	MBR_MSV_FLOAT_CHN(0x0001)
#define MBR_MSV_FLOAT_CHN02            	MBR_MSV_FLOAT_CHN(0x0002)
#define MBR_MSV_FLOAT_CHN03            	MBR_MSV_FLOAT_CHN(0x0003)
#define MBR_MSV_FLOAT_CHN04            	MBR_MSV_FLOAT_CHN(0x0004)
#define MBR_MSV_FLOAT_CHN05            	MBR_MSV_FLOAT_CHN(0x0005)
#define MBR_MSV_FLOAT_CHN06            	MBR_MSV_FLOAT_CHN(0x0006)
#define MBR_MSV_FLOAT_CHN07            	MBR_MSV_FLOAT_CHN(0x0007)
#define MBR_MSV_FLOAT_CHN08            	MBR_MSV_FLOAT_CHN(0x0008)
#define MBR_MSV_FLOAT_CHN09            	MBR_MSV_FLOAT_CHN(0x0009)
#define MBR_MSV_FLOAT_CHN10            	MBR_MSV_FLOAT_CHN(0x000A)
#define MBR_MSV_FLOAT_CHN11            	MBR_MSV_FLOAT_CHN(0x000B)
#define MBR_MSV_FLOAT_CHN12            	MBR_MSV_FLOAT_CHN(0x000C)
#define MBR_MSV_FLOAT_CHN13            	MBR_MSV_FLOAT_CHN(0x000D)
#define MBR_MSV_FLOAT_CHN14            	MBR_MSV_FLOAT_CHN(0x000E)
#define MBR_MSV_FLOAT_CHN15            	MBR_MSV_FLOAT_CHN(0x000F)

//----------------------
//数据寄存器（主动输出）-16位整型
//----------------------
#define MBR_INFO_MSV_NUM        					SENSOR_CHANNEL_SIZE_MAXSUPPORT
#define MBR_INFO_MSV_BASE       					0x8000
#define MBR_INFO_MSV_CHN(n)     					(MBR_INFO_MSV_BASE + (n))
#define MBR_INFO_MSV_00          					MBR_INFO_MSV_CHN(0x0000)
#define MBR_INFO_MSV_01          					MBR_INFO_MSV_CHN(0x0001)
#define MBR_INFO_MSV_02          					MBR_INFO_MSV_CHN(0x0002)
#define MBR_INFO_MSV_03          					MBR_INFO_MSV_CHN(0x0003)
#define MBR_INFO_MSV_04          					MBR_INFO_MSV_CHN(0x0004)
#define MBR_INFO_MSV_05          					MBR_INFO_MSV_CHN(0x0005)
#define MBR_INFO_MSV_06          					MBR_INFO_MSV_CHN(0x0006)
#define MBR_INFO_MSV_07          					MBR_INFO_MSV_CHN(0x0007)
#define MBR_INFO_MSV_08          					MBR_INFO_MSV_CHN(0x0008)
#define MBR_INFO_MSV_09          					MBR_INFO_MSV_CHN(0x0009)
#define MBR_INFO_MSV_10        						MBR_INFO_MSV_CHN(0x000A)
#define MBR_INFO_MSV_11        						MBR_INFO_MSV_CHN(0x000B)
#define MBR_INFO_MSV_12        						MBR_INFO_MSV_CHN(0x000C)
#define MBR_INFO_MSV_13        						MBR_INFO_MSV_CHN(0x000D)
#define MBR_INFO_MSV_14        						MBR_INFO_MSV_CHN(0x000E)
#define MBR_INFO_MSV_15        						MBR_INFO_MSV_CHN(0x000F)

//----------------------
//用户配置
//----------------------
#define MBR_USERCONFIG_NUM	    		      8
#define MBR_USERCONFIG_BASE               0x0020
#define MBR_USERCONFIG(n)	                (MBR_USERCONFIG_BASE + (n))
#define MBR_USERCONFIG_FLOATBYTEORDER	    MBR_USERCONFIG(0x0000)
#define MBR_USERCONFIG_RESERVED1  				MBR_USERCONFIG(0x0001)
#define MBR_USERCONFIG_RESERVED2 	  			MBR_USERCONFIG(0x0002)
#define MBR_USERCONFIG_RESERVED3     			MBR_USERCONFIG(0x0003)
#define MBR_USERCONFIG_RESERVED4   				MBR_USERCONFIG(0x0004)
#define MBR_USERCONFIG_RESERVED5 	        MBR_USERCONFIG(0x0005)
#define MBR_USERCONFIG_RESERVED6 	        MBR_USERCONFIG(0x0006)
#define MBR_USERCONFIG_RESERVED7 	        MBR_USERCONFIG(0x0007)

#define MBR_TOFFSET_NUM	            		SENSOR_CHANNEL_SIZE_MAXSUPPORT
#define MBR_TOFFSET_BASE        				0x0030
#define MBR_TOFFSET_CHN(n)	            (MBR_TOFFSET_BASE + (n))
#define MBR_TOFFSET_CHN00	            	MBR_TOFFSET_CHN(0x0000)
#define MBR_TOFFSET_CHN01	            	MBR_TOFFSET_CHN(0x0001)
#define MBR_TOFFSET_CHN02	            	MBR_TOFFSET_CHN(0x0002)
#define MBR_TOFFSET_CHN03	            	MBR_TOFFSET_CHN(0x0003)
#define MBR_TOFFSET_CHN04	            	MBR_TOFFSET_CHN(0x0004)
#define MBR_TOFFSET_CHN05	            	MBR_TOFFSET_CHN(0x0005)
#define MBR_TOFFSET_CHN06	            	MBR_TOFFSET_CHN(0x0006)
#define MBR_TOFFSET_CHN07	            	MBR_TOFFSET_CHN(0x0007)
#define MBR_TOFFSET_CHN08	            	MBR_TOFFSET_CHN(0x0008)
#define MBR_TOFFSET_CHN09	            	MBR_TOFFSET_CHN(0x0009)
#define MBR_TOFFSET_CHN10	           	 	MBR_TOFFSET_CHN(0x000A)
#define MBR_TOFFSET_CHN11	           	 	MBR_TOFFSET_CHN(0x000B)
#define MBR_TOFFSET_CHN12	            	MBR_TOFFSET_CHN(0x000C)
#define MBR_TOFFSET_CHN13	            	MBR_TOFFSET_CHN(0x000D)
#define MBR_TOFFSET_CHN14	            	MBR_TOFFSET_CHN(0x000E)
#define MBR_TOFFSET_CHN15	            	MBR_TOFFSET_CHN(0x000F)

#define MBR_RTDTYPE_NUM	            	SENSOR_CHANNEL_SIZE_MAXSUPPORT
#define MBR_RTDTYPE_BASE        			0x0060
#define MBR_RTDTYPE_CHN(n)	          (MBR_RTDTYPE_BASE + (n))
#define MBR_RTDTYPE_CHN00	            MBR_RTDTYPE_CHN(0x0000)
#define MBR_RTDTYPE_CHN01	            MBR_RTDTYPE_CHN(0x0001)
#define MBR_RTDTYPE_CHN02	            MBR_RTDTYPE_CHN(0x0002)
#define MBR_RTDTYPE_CHN03	            MBR_RTDTYPE_CHN(0x0003)
#define MBR_RTDTYPE_CHN04	            MBR_RTDTYPE_CHN(0x0004)
#define MBR_RTDTYPE_CHN05	            MBR_RTDTYPE_CHN(0x0005)
#define MBR_RTDTYPE_CHN06	            MBR_RTDTYPE_CHN(0x0006)
#define MBR_RTDTYPE_CHN07	            MBR_RTDTYPE_CHN(0x0007)
#define MBR_RTDTYPE_CHN08	            MBR_RTDTYPE_CHN(0x0008)
#define MBR_RTDTYPE_CHN09	            MBR_RTDTYPE_CHN(0x0009)
#define MBR_RTDTYPE_CHN10	            MBR_RTDTYPE_CHN(0x000A)
#define MBR_RTDTYPE_CHN11	            MBR_RTDTYPE_CHN(0x000B)
#define MBR_RTDTYPE_CHN12	            MBR_RTDTYPE_CHN(0x000C)
#define MBR_RTDTYPE_CHN13	            MBR_RTDTYPE_CHN(0x000D)
#define MBR_RTDTYPE_CHN14	            MBR_RTDTYPE_CHN(0x000E)
#define MBR_RTDTYPE_CHN15	            MBR_RTDTYPE_CHN(0x000F)

#define MBR_FILTERORDER_NUM	        	SENSOR_CHANNEL_SIZE_MAXSUPPORT
#define MBR_FILTERORDER_BASE        	0x0070
#define MBR_FILTERORDER_CHN(n)	      (MBR_FILTERORDER_BASE + (n))
#define MBR_FILTERORDER_CHN00	        MBR_FILTERORDER_CHN(0x0000)
#define MBR_FILTERORDER_CHN01	        MBR_FILTERORDER_CHN(0x0001)
#define MBR_FILTERORDER_CHN02	        MBR_FILTERORDER_CHN(0x0002)
#define MBR_FILTERORDER_CHN03	        MBR_FILTERORDER_CHN(0x0003)
#define MBR_FILTERORDER_CHN04	        MBR_FILTERORDER_CHN(0x0004)
#define MBR_FILTERORDER_CHN05	        MBR_FILTERORDER_CHN(0x0005)
#define MBR_FILTERORDER_CHN06	        MBR_FILTERORDER_CHN(0x0006)
#define MBR_FILTERORDER_CHN07	        MBR_FILTERORDER_CHN(0x0007)
#define MBR_FILTERORDER_CHN08	        MBR_FILTERORDER_CHN(0x0008)
#define MBR_FILTERORDER_CHN09	        MBR_FILTERORDER_CHN(0x0009)
#define MBR_FILTERORDER_CHN10	        MBR_FILTERORDER_CHN(0x000A)
#define MBR_FILTERORDER_CHN11	        MBR_FILTERORDER_CHN(0x000B)
#define MBR_FILTERORDER_CHN12	        MBR_FILTERORDER_CHN(0x000C)
#define MBR_FILTERORDER_CHN13	        MBR_FILTERORDER_CHN(0x000D)
#define MBR_FILTERORDER_CHN14	        MBR_FILTERORDER_CHN(0x000E)
#define MBR_FILTERORDER_CHN15	        MBR_FILTERORDER_CHN(0x000F)

#define MBR_TCCOLDCOMPENSATION_NUM	  SENSOR_CHANNEL_SIZE_MAXSUPPORT
#define MBR_CJC_ENABLE_BASE	        	0x0080
#define MBR_CJC_ENABLE_CHN(n)	        (MBR_CJC_ENABLE_BASE + (n))
#define MBR_CJC_ENABLE_CHNL00	        MBR_CJC_ENABLE_CHN(0x0000)
#define MBR_CJC_ENABLE_CHNL01	        MBR_CJC_ENABLE_CHN(0x0001)
#define MBR_CJC_ENABLE_CHNL02	        MBR_CJC_ENABLE_CHN(0x0002)
#define MBR_CJC_ENABLE_CHNL03	        MBR_CJC_ENABLE_CHN(0x0003)
#define MBR_CJC_ENABLE_CHNL04	        MBR_CJC_ENABLE_CHN(0x0004)
#define MBR_CJC_ENABLE_CHNL05	       	MBR_CJC_ENABLE_CHN(0x0005)
#define MBR_CJC_ENABLE_CHNL06	       	MBR_CJC_ENABLE_CHN(0x0006)
#define MBR_CJC_ENABLE_CHNL07	       	MBR_CJC_ENABLE_CHN(0x0007)
#define MBR_CJC_ENABLE_CHNL08	      	MBR_CJC_ENABLE_CHN(0x0008)
#define MBR_CJC_ENABLE_CHNL09	      	MBR_CJC_ENABLE_CHN(0x0009)
#define MBR_CJC_ENABLE_CHNL10	      	MBR_CJC_ENABLE_CHN(0x000A)
#define MBR_CJC_ENABLE_CHNL11	      	MBR_CJC_ENABLE_CHN(0x000B)
#define MBR_CJC_ENABLE_CHNL12	      	MBR_CJC_ENABLE_CHN(0x000C)
#define MBR_CJC_ENABLE_CHNL13	      	MBR_CJC_ENABLE_CHN(0x000D)
#define MBR_CJC_ENABLE_CHNL14	        MBR_CJC_ENABLE_CHN(0x000E)
#define MBR_CJC_ENABLE_CHNL15	       	MBR_CJC_ENABLE_CHN(0x000F)

#define MBR_CHNL_ENABLE_NUM      			SENSOR_CHANNEL_SIZE_MAXSUPPORT
#define MBR_CHNL_ENABLE_BASE          0x0090
#define MBR_CHNL_ENABLE_CHN(n)        (MBR_CHNL_ENABLE_BASE + (n))
#define MBR_CHNL_ENABLE_CHNL00        MBR_CHNL_ENABLE_CHN(0x0000)
#define MBR_CHNL_ENABLE_CHNL01        MBR_CHNL_ENABLE_CHN(0x0001)
#define MBR_CHNL_ENABLE_CHNL02        MBR_CHNL_ENABLE_CHN(0x0002)
#define MBR_CHNL_ENABLE_CHNL03        MBR_CHNL_ENABLE_CHN(0x0003)
#define MBR_CHNL_ENABLE_CHNL04        MBR_CHNL_ENABLE_CHN(0x0004)
#define MBR_CHNL_ENABLE_CHNL05        MBR_CHNL_ENABLE_CHN(0x0005)
#define MBR_CHNL_ENABLE_CHNL06        MBR_CHNL_ENABLE_CHN(0x0006)
#define MBR_CHNL_ENABLE_CHNL07        MBR_CHNL_ENABLE_CHN(0x0007)
#define MBR_CHNL_ENABLE_CHNL08        MBR_CHNL_ENABLE_CHN(0x0008)
#define MBR_CHNL_ENABLE_CHNL09        MBR_CHNL_ENABLE_CHN(0x0009)
#define MBR_CHNL_ENABLE_CHNL10        MBR_CHNL_ENABLE_CHN(0x000A)
#define MBR_CHNL_ENABLE_CHNL11        MBR_CHNL_ENABLE_CHN(0x000B)
#define MBR_CHNL_ENABLE_CHNL12        MBR_CHNL_ENABLE_CHN(0x000C)
#define MBR_CHNL_ENABLE_CHNL13        MBR_CHNL_ENABLE_CHN(0x000D)
#define MBR_CHNL_ENABLE_CHNL14        MBR_CHNL_ENABLE_CHN(0x000E)
#define MBR_CHNL_ENABLE_CHNL15        MBR_CHNL_ENABLE_CHN(0x000F)

//----------------------
//恢复出厂设置
//----------------------
#define MBR_FACTORYRESET_NUM	            	1
#define MBR_FACTORYRESET_BASE             	0x0050
#define MBR_FACTORYRESET                 		(MBR_FACTORYRESET_BASE + 0x0000)
#define MBR_SYSTEMRESET_NUM	              	1
#define MBR_SYSTEMRESET_BASE              	0x0051
#define MBR_SYSTEMRESET                 		(MBR_SYSTEMRESET_BASE + 0x0000)
#define MBR_CALIBRESET_NUM	            	  1
#define MBR_CALIBRESET_BASE             	  0x0052
#define MBR_CALIBRESET                 		  (MBR_CALIBRESET_BASE + 0x0000)
#define MBR_SELFCALIB_NUM	              	  1
#define MBR_SELFCALIB_BASE              	  0x0053
#define MBR_SELFCALIB                 		  (MBR_SELFCALIB_BASE + 0x0000)

//----------------------
//通信配置
//----------------------
#define MBR_USART_NUM		                	8
#define MBR_USART_BASE                  	0x0200
#define MBR_USART_CFG(n)                	(MBR_USART_BASE + (n))
#define MBR_USART_ADDRESS	              	MBR_USART_CFG(0x0000)
#define MBR_USART_BAUDRATE	            	MBR_USART_CFG(0x0001)
#define MBR_USART_PROTOCOL	            	MBR_USART_CFG(0x0002)
#define MBR_USART_PARITY	              	MBR_USART_CFG(0x0003)
#define MBR_USART_DATABITS	            	MBR_USART_CFG(0x0004)
#define MBR_USART_STOPBITS	            	MBR_USART_CFG(0x0005)
#define MBR_USART_RESPONSEDELAY	        	MBR_USART_CFG(0x0006)
#define MBR_USART_ACTIVEOUTPUTINTERVAL  	MBR_USART_CFG(0x0007)

#define MBR_DEVICE_NAMEVERSION_NUM        4
#define MBR_DEVICE_NAMEVERSION_BASE       0x0210
#define MBR_DEVICE_NAMEVERSION(n)         (MBR_DEVICE_NAMEVERSION_BASE + (n))
#define MBR_DEVICE_MODULENAME1          	MBR_DEVICE_NAMEVERSION(0x0000)
#define MBR_DEVICE_MODULENAME2          	MBR_DEVICE_NAMEVERSION(0x0001)
#define MBR_DEVICE_VERSION1             	MBR_DEVICE_NAMEVERSION(0x0002)
#define MBR_DEVICE_VERSION2             	MBR_DEVICE_NAMEVERSION(0x0003)

#define MBR_USERTAG_NUM         					4
#define MBR_USERTAG_BASE                  0x0220
#define MBR_USERTAG(n)                    (MBR_USERTAG_BASE + (n))
#define MBR_USERTAG_0           					MBR_USERTAG(0x0000)
#define MBR_USERTAG_1           					MBR_USERTAG(0x0001)
#define MBR_USERTAG_2           					MBR_USERTAG(0x0002)
#define MBR_USERTAG_3           					MBR_USERTAG(0x0003)

//------------------------------------------
//私有寄存器
//------------------------------------------
//----------------------
//制造信息
//----------------------
#define MBR_D_MODINFO_NUM		        			0x0004
#define MBR_D_MODINFO_BASE       					0xF000
#define MBR_D_MODINFO(n)        					(MBR_D_MODINFO_BASE + (n))
#define MBR_D_MODINFO_YYYY	        			MBR_D_MODINFO(0x0000)
#define MBR_D_MODINFO_MMDD	        			MBR_D_MODINFO(0x0001)
#define MBR_D_MODINFO_HHMM	        			MBR_D_MODINFO(0x0002)
#define MBR_D_MODINFO_PRODUCTID	    			MBR_D_MODINFO(0x0003)

#define MBR_D_DEBUGENABLE_PASSWORD_NUM		0x0002
#define MBR_D_DEBUGENABLE_PASSWORD_BASE   0xF005
#define MBR_D_DEBUGENABLE_PASSWORD(n)     (MBR_D_DEBUGENABLE_PASSWORD_BASE + (n))
#define MBR_D_DEBUGENABLE_PASSWORD1     	MBR_D_DEBUGENABLE_PASSWORD(0x0000)
#define MBR_D_DEBUGENABLE_PASSWORD2     	MBR_D_DEBUGENABLE_PASSWORD(0x0001)

#define MBR_FACTORYCONFIG_NUM		        	4
#define MBR_FACTORYCONFIG_BASE            0xF010
#define MBR_FACTORYCONFIG(n)              (MBR_FACTORYCONFIG_BASE + (n))
#define MBR_FACTORYCONFIG_SENSORTYPE	  	MBR_FACTORYCONFIG(0x0000)
#define MBR_FACTORYCONFIG_TEMPSENSOR  		MBR_FACTORYCONFIG(0x0001)
#define MBR_FACTORYCONFIG_RESERVED1  			MBR_FACTORYCONFIG(0x0002)
#define MBR_FACTORYCONFIG_RESERVED2  	    MBR_FACTORYCONFIG(0x0003)

#define MBR_D_MEASUREMODE               0xF100
#define MBR_D_RESTART                   0xF101
#define MBR_D_SYSTEMOFFSET              0xF102

//Channel real time value for both temprature or adc value
#define MBR_D_MsvDebug_NUM          (SENSOR_CHANNEL_SIZE_MAXSUPPORT*2)
#define MBR_D_MsvDebug_BASE         0xF110
#define MBR_D_MsvDebug_CHN(n)       (MBR_D_MsvDebug_BASE + (n)*2)
#define MBR_D_MsvDebug_CHNL00       MBR_D_MsvDebug_CHN(0x0000)
#define MBR_D_MsvDebug_CHNL01       MBR_D_MsvDebug_CHN(0x0001)
#define MBR_D_MsvDebug_CHNL02       MBR_D_MsvDebug_CHN(0x0002)
#define MBR_D_MsvDebug_CHNL03       MBR_D_MsvDebug_CHN(0x0003)
#define MBR_D_MsvDebug_CHNL04       MBR_D_MsvDebug_CHN(0x0004)
#define MBR_D_MsvDebug_CHNL05       MBR_D_MsvDebug_CHN(0x0005)
#define MBR_D_MsvDebug_CHNL06       MBR_D_MsvDebug_CHN(0x0006)
#define MBR_D_MsvDebug_CHNL07       MBR_D_MsvDebug_CHN(0x0007)
#define MBR_D_MsvDebug_CHNL08       MBR_D_MsvDebug_CHN(0x0008)
#define MBR_D_MsvDebug_CHNL09       MBR_D_MsvDebug_CHN(0x0009)
#define MBR_D_MsvDebug_CHNL10       MBR_D_MsvDebug_CHN(0x000A)
#define MBR_D_MsvDebug_CHNL11       MBR_D_MsvDebug_CHN(0x000B)
#define MBR_D_MsvDebug_CHNL12       MBR_D_MsvDebug_CHN(0x000C)
#define MBR_D_MsvDebug_CHNL13       MBR_D_MsvDebug_CHN(0x000D)
#define MBR_D_MsvDebug_CHNL14       MBR_D_MsvDebug_CHN(0x000E)
#define MBR_D_MsvDebug_CHNL15       MBR_D_MsvDebug_CHN(0x000F)

//Calib Point, Resistor range 0-400ohm, Lower Calib(100ohm)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_NUM     (SENSOR_CHANNEL_SIZE_MAXSUPPORT*2)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_BASE    0xF130
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(n)  (MBR_D_CALIBPOINT_RangeSmall_Lower_BASE + (n)*2)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHNL00  MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(0x0000)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHNL01  MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(0x0001)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHNL02  MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(0x0002)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHNL03  MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(0x0003)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHNL04  MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(0x0004)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHNL05  MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(0x0005)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHNL06  MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(0x0006)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHNL07  MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(0x0007)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHNL08  MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(0x0008)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHNL09  MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(0x0009)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHNL10  MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(0x000A)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHNL11  MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(0x000B)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHNL12  MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(0x000C)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHNL13  MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(0x000D)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHNL14  MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(0x000E)
#define MBR_D_CALIBPOINT_RangeSmall_Lower_CHNL15  MBR_D_CALIBPOINT_RangeSmall_Lower_CHN(0x000F)

//Calib Point, Resistor range 0-400ohm, Upper Calib(250ohm)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_NUM     (SENSOR_CHANNEL_SIZE_MAXSUPPORT*2)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_BASE    0xF150
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(n)  (MBR_D_CALIBPOINT_RangeSmall_Upper_BASE + (n)*2)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHNL00  MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(0x0000)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHNL01  MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(0x0001)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHNL02  MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(0x0002)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHNL03  MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(0x0003)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHNL04  MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(0x0004)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHNL05  MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(0x0005)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHNL06  MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(0x0006)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHNL07  MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(0x0007)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHNL08  MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(0x0008)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHNL09  MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(0x0009)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHNL10  MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(0x000A)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHNL11  MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(0x000B)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHNL12  MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(0x000C)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHNL13  MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(0x000D)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHNL14  MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(0x000E)
#define MBR_D_CALIBPOINT_RangeSmall_Upper_CHNL15  MBR_D_CALIBPOINT_RangeSmall_Upper_CHN(0x000F)

//Calib Point, Resistor range 0-6000ohm, Lower Calib(1000ohm)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_NUM    (SENSOR_CHANNEL_SIZE_MAXSUPPORT*2)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_BASE   0xF170
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(n) (MBR_D_CALIBPOINT_RangeLarge_Lower_BASE + (n)*2)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHNL00 MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(0x0000)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHNL01 MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(0x0001)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHNL02 MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(0x0002)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHNL03 MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(0x0003)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHNL04 MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(0x0004)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHNL05 MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(0x0005)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHNL06 MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(0x0006)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHNL07 MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(0x0007)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHNL08 MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(0x0008)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHNL09 MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(0x0009)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHNL10 MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(0x000A)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHNL11 MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(0x000B)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHNL12 MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(0x000C)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHNL13 MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(0x000D)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHNL14 MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(0x000E)
#define MBR_D_CALIBPOINT_RangeLarge_Lower_CHNL15 MBR_D_CALIBPOINT_RangeLarge_Lower_CHN(0x000F)

//Calib Point, Resistor range 0-6000ohm, Upper Calib(3000ohm)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_NUM    (SENSOR_CHANNEL_SIZE_MAXSUPPORT*2)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_BASE   0xF190
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(n) (MBR_D_CALIBPOINT_RangeLarge_Upper_BASE + (n)*2)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHNL00 MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(0x0000)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHNL01 MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(0x0001)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHNL02 MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(0x0002)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHNL03 MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(0x0003)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHNL04 MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(0x0004)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHNL05 MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(0x0005)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHNL06 MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(0x0006)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHNL07 MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(0x0007)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHNL08 MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(0x0008)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHNL09 MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(0x0009)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHNL10 MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(0x000A)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHNL11 MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(0x000B)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHNL12 MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(0x000C)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHNL13 MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(0x000D)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHNL14 MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(0x000E)
#define MBR_D_CALIBPOINT_RangeLarge_Upper_CHNL15 MBR_D_CALIBPOINT_RangeLarge_Upper_CHN(0x000F)

#endif /* MODBUS_MBR_H_ */
