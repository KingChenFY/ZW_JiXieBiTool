#ifndef __HARD_DEF_H_
#define __HARD_DEF_H_

#define NO_MOVE 0x7FFFFFFF
#define PRINT_PORT 8889
#define PC_PORT 8888

#define WK_LEN_QRCODE_CONTENT           128             //二维码长度

//测距外触发切换模块 D层轴类型
typedef enum
{
    emAxis_X,           //X轴
    emAxis_Y,           //Y轴
    emAxis_Z,           //Z轴
    emAxis_End
}emAxisType;

//入舱皮带模块任务类型
typedef enum
{
    emTskDBeltInType_Start = 0,

    emTskDBeltInType_InSideMoving,      //入舱皮带向里转动(自动使能)
    emTskDBeltInType_OutSideMoving,     //入舱皮带向外转动(自动使能)
    emTskDBeltInType_Stop,              //入舱皮带停止(自动使能)
    emTskDBeltInType_PowerOff,          //入舱皮带失能(允许手推动)

    emTskDBeltInType_End
}emTaskDBeltInType;

//出舱皮带模块任务类型
typedef enum
{
    emTskDBeltOutType_Start = 0,

    emTskDBeltOutType_InSideMoving,     // 出舱皮带向里转动(自动使能)
    emTskDBeltOutType_OutSideMoving,    // 出舱皮带向外转动(自动使能)
    emTskDBeltOutType_Stop,             // 出舱皮带停止(自动使能)
    emTskDBeltOutType_PowerOff,         // 出舱皮带失能(允许手推动)

    emTskDBeltOutType_End
}emTaskDBeltOutType;

// 入舱推杆功能D层任务类型
typedef enum
{
    emTskDPoleInType_Start,

    emTskDPoleInType_In,            // 入舱推杆进位(到5)
    emTskDPoleInType_Out,           // 入舱推杆出位(到1)
    emTskDPoleInType_Stop,          // 停止入舱推杆移动(自动使能)
    emTskDPoleInType_PowerOff,      // 入舱推杆失能(允许手推动)

    emTskDPoleInType_End
}emTaskDPoleInType;

// 出舱拉杆功能D层任务类型
typedef enum
{
    emTskDPoleOutType_Start,

    emTskDPoleOutType_In,           // 出舱拉杆进位(到5)
    emTskDPoleOutType_Out,          // 出舱拉杆出位(到1)
    emTskDPoleOutType_Stop,         // 停止出舱拉杆移动(自动使能)
    emTskDPoleOutType_PowerOff,     // 出舱拉杆失能(允许手推动)

    emTskDPoleOutType_End
}emTaskDPoleOutType;

//玻片盒翻转装置模块任务类型
typedef enum
{
    emTskDUpenderType_Start,

    emTskDUpenderType_LieDown,      // 翻倒（出）
    emTskDUpenderType_StandUp,      // 立起(入)
    emTskDUpenderType_Stop,         // 停止
    emTskDUpenderType_PowerOff,     // 不支持失能，该任务不使用

    emTskDUpenderType_End
}emTaskDUpenderType;

//玻片盒垂直扫描任务类型
typedef enum
{
    emTaskDMoveBoxVType_start = 0,

    emTaskDMoveBoxVType_minPosSet,      //下边界定位
    emTaskDMoveBoxVType_maxPosSet,      //上边界定位
    emTaskDMoveBoxVType_slideScan,      //玻片扫描
    emTaskDMoveBoxVType_move,           //移动
    emTaskDMoveBoxVType_stop,           //停止
    emTaskDMoveBoxVType_zeroSFCheck,    //较零

    emTaskDMoveBoxVType_end
}emTaskDBoxVType;

// XYZ平台移动任务类型
typedef enum
{
    emTskDXYZType_start = 0,

    emTskDXYZType_zeroXCheck,       //X轴原点校验1
    emTskDXYZType_setXPosZero,      //设置X轴原点2
    emTskDXYZType_minPosYSet,       //里边界定位3
    emTskDXYZType_maxPosYSet,       //外边界定位4
    emTskDXYZType_minPosZSet,       //底边界定位5
    emTskDXYZType_maxPosZSet,       //顶边界定位6
    emTskDXYZType_stop,             //停止7
    emTskDXYZType_move,             //移动到指定点8
    emTskDXYZType_moveInTime,       //一定时间内移动到指定点9
    emTskDXYZType_moveTogether,     //连轴移动10
    emTskDXYZType_moveTwoPos,       //两点移动(移动到起点，定时移动到终点)11
    emTskDXYZType_enableAxis,       //使能轴12
    emTskDXYZType_disableAxis,      //失能轴13
    emTskDXYZType_adaptive,         //自适应轴参数14
    emTskDXYZType_zeroXMoveCheck,   //设置不同的移动速度进行移动测试15

    emTskDXYZType_end
}emTaskDXYZType;

//除油任务类型
typedef enum
{
    emTaskDCleanOilType_Start = 0,

    emTaskDCleanOilType_Clean,          // 去油
    emTaskDCleanOilType_Stop,           // 任务停止
    emTaskDCleanOilType_PowerOff,       //轴失能
    emTaskDCleanOilType_Frap,           //收纸

    emTaskDCleanOilType_End
}emTaskDCleanOilType;

//滴油任务类型
typedef enum
{
    emTaskDDripOil_Start = 0,

    emTaskDDripOil_Drop,        //滴油
    emTaskDDripOil_Draw,        //加油
    emTaskDDripOil_Stop,
    emTaskDDripOil_DrawBack,    //油壶放油
    emTaskDDripOil_DripOut,     //滴头放油

    emTaskDDripOil_End
}emTaskDDripOilType;

//夹爪模块任务类型
typedef enum
{
    emTaskDGripperType_Start = 0,

    emTaskDGripperType_Control,       // 夹爪控制
    emTaskDGripperType_SynSlide,      // 同步玻片状态
    emTaskDGripperType_End
}emTaskDGripperType;

// 各路传感信号状态枚举
typedef enum
{
    emSensorStatus_Start,       // 信号未同步
    emSensorStatus_On,          //信号有效
    emSensorStatus_Off,         //信号无效
    emSensorStatus_Err,         //信号功能异常
    emSensorStatus_End
}emSensorStatus;

// 各路传感信号编号枚举
typedef enum
{
    emSensorId_Start,
    emSensorId_DuiShe1,         // 入舱对射1(里)								1
    emSensorId_DuiShe2,         // 入舱对射2(外)								2
    emSensorId_Touch1,          // 推杆起点盒到位信号1						3
    emSensorId_Touch2,          // 推杆起点盒到位信号2(靠翻转装置侧)			4
    emSensorId_PushLmt1,        // 上层推杆顶住信号							5
    emSensorId_PullLmt2,        // 下层拉杆顶住信号                           6
    emSensorId_InPos1,          // 入舱盒位1(手动放盒位)						7
    emSensorId_InPos2,          // 入舱盒位2(皮带中段)						8
    emSensorId_InPos3,          // 入舱盒位3(皮带最里端)						9
    emSensorId_InPos4,          // 入舱盒位4(靠皮带侧)						10
    emSensorId_InPos5,          // 入舱盒位5(推杆中段靠皮带侧)					11
    emSensorId_InPos6,          // 入舱盒位6(推杆中段靠翻转装置侧)				12
    emSensorId_InPos7,          // 入舱盒位7(靠翻转装置侧)						13
    emSensorId_HatchTouch1,     // 玻片盒存在上信号							14
    emSensorId_HatchTouch2,     // 玻片盒存在下信号							15
    emSensorId_HatchExist,      //旋转仓内玻片盒存在信号						16
    emSensorId_OutPos1,         // 出舱盒位1(靠翻转装置侧)                    17
    emSensorId_OutPos2,         // 出舱盒位2(拉杆中段靠翻转装置侧)             18
    emSensorId_OutPos3,         // 出舱盒位3(拉杆中段靠皮带侧)
    emSensorId_OutPos4,         // 出舱盒位4(靠皮带侧)
    emSensorId_OutPos5,         // 出舱盒位5(皮带最里端)
    emSensorId_OutPos6,         // 出舱盒位6(皮带中段)
    emSensorId_OutPos7,         // 出舱盒位7(手动拿盒位)
    emSensorId_Gripper,         // 夹爪上玻片信号
    emSensorId_Door,            // 舱门信号
    emSensorId_BoxVUp,          // 垂直盒在上
    emSensorId_BoxVDown,        // 垂直盒在下
    emSensorId_CleanOilEmpty,   // 除油没纸信号
    emSensorId_CleanOilRelease, // 除油纸松信号

    emSensorId_End
}emSensorId;

// 打印对象bitmap
typedef enum
{
    pActionThread_START 		= 0,
    pActionThread_TCP 			= 0x00000001,
    pActionThread_UsageCPU 	= 0x00000002,
    pActionThread_X 			= 0x00000004,
    pActionThread_Y 			= 0x00000008,
    pActionThread_Z 			= 0x00000010,
    pActionThread_SFScan 		= 0x00000020,
    pActionThread_GripRole 	= 0x00000040,
    pActionThread_YZmtData	= 0x00000080,
    pActionThread_Pos         = 0x00000100,
    pActionThread_End
}emNetPrintfEnableBYTE;

/*Board ONE(RT1061) TAST START*/

// 二维码扫码D层任务类型
typedef enum 
{
    emTaskDQRScanner_Start,
        
    emTaskDQRScanner_Scan,
    
    emTaskDQRScanner_End
}emTaskDQRScannerType;

//夹爪旋转模块任务类型
typedef enum 
{
	emTskDGripRoleType_Start = 0,

	emTskDGripRoleType_Control, //旋转控制

	emTskDGripRoleType_End
}emTskDGripRoleType;
		
//振镜模块任务类型
typedef enum
{
	emTskDGalvoType_Start = 0,

	emTskDGalvoType_CycleMove,	//设置周期运动
	emTskDGalvoType_Point,		//设置定点
	emTskDGalvoType_Stop,		//停止振镜

	emTskDGalvoType_End
}emTskDGalvoType;
/*Board ONE(RT1061) TAST END*/
/*Board TWO(STM32F407) TAST START*/

//光源任务类型
typedef enum 
{
    emTaskDSetLED_Start,
        
    emTaskDSetLED_Control,//控制光源
    
    emTaskDSetLED_End
}emTaskDSetLEDType;

//硬件独立小功能控制
typedef enum
{
 emHardControl_Start,

 emHardControl_BeltLed,//入舱皮带激光
 emHardControl_BeltQR,//入舱皮带二维码

 emHardControl_End
}emHardControlType;

/*Board TWO(STM32F407) TAST END*/

// 执行状态
typedef enum 
{
    emWorkStatus_start,
        
    emWorkStatus_ask,//已接受
    emWorkStatus_run,//执行中
    emWorkStatus_finish,//动作已完成
	
    emWorkStatus_end
}emWorkStatus;



#endif /* ifndef __HARD_DEF_H_ */

