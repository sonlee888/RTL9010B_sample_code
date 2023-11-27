//All functions can refer to RTL9010Bx_Sample_Code_Note_v0.3.pdf.

#include "mdio.h"
#include "mdio2.h"
#include "ht32f165x_it.h"

#define DEBUG		1
#if(DEBUG)
	#define DBGMSG(x)		{printf x;}
#else
	#define DBGMSG(x)
#endif



#define LinkUp			1
#define LinkDown		0

#define Falling_edge_detection		1
#define Rising_edge_detection		0

#define Lwake_to_rwake_is_enabled		1
#define Lwake_to_rwake_is_disabled		2



#define BIT_TST(x,n)			(x&(1<<n))
#define BIT_SET(x,n)			(x|=(1<<n))
#define BIT_CLR(x,n)			(x&=(~(1<<n)))
#define BIT_SHIFT(x,n)			(x&(0xFFFF<<n))

#define PARAM_RESET(x)		(x=0)


#define RX_LPI_INDICATION_REAL_TIME	 			0
#define TX_LPI_INDICATION_REAL_TIME				1
#define RX_LPI_STATUS_LATCHED_HIGH			2
#define TX_LPI_STATUS_LATCHED_HIGH			3

#define GENERAL_INTERRUPT_STATUS_PHY_STATUS_CHANGED			0
#define GENERAL_INTERRUPT_STATUS_GENERAL_PURPOSE				1
#define GENERAL_INTERRUPT_STATUS_LINK_STATUS_CHANGED			2
#define GENERAL_INTERRUPT_STATUS_PTP_EVENT						3
#define GENERAL_INTERRUPT_STATUS_PHY_FATAL_ERROR				4
#define GENERAL_INTERRUPT_STATUS_MACSEC_EVENT				5

#define GENERAL_INTERRUPT_SUB_STATUS_SLEEP_ACK				0
#define GENERAL_INTERRUPT_SUB_STATUS_SLEEP_FAIL			1
#define GENERAL_INTERRUPT_SUB_STATUS_LPS_WUR				2

#define OP_INTERRUPT_STATUS_TRANS_TO_SLEEP_BY_UV_DVDD_MII		0
#define OP_INTERRUPT_STATUS_TRANS_TO_SLEEP_BY_UV_DVDD09			1
#define OP_INTERRUPT_STATUS_TRANS_TO_SLEEP_BY_UV_AVDD09			2
#define OP_INTERRUPT_STATUS_TRANS_TO_SLEEP_BY_UV_DVDD33			3
#define OP_INTERRUPT_STATUS_TRANS_TO_SLEEP_BY_UV_AVDD33			4
#define OP_INTERRUPT_STATUS_TRANS_TO_SLEEP_BY_HOST_CMD			5
#define OP_INTERRUPT_STATUS_REC_UV_AVDD33							6
#define OP_INTERRUPT_STATUS_REC_UV_DVDD33							7
#define OP_INTERRUPT_STATUS_REC_UV_AVDD09							8
#define OP_INTERRUPT_STATUS_REC_UV_DVDD09							9
#define OP_INTERRUPT_STATUS_REC_UV_DVDD_MII						10
#define OP_INTERRUPT_STATUS_OT_EVENT								11
#define OP_INTERRUPT_STATUS_WOKEUP_BY_REM_WAKE					12
#define OP_INTERRUPT_STATUS_RECEIVE_WUR							13
#define OP_INTERRUPT_STATUS_WOKEUP_BY_LOC_WAKE					14
#define OP_INTERRUPT_STATUS_PHY_PWR_ON							15
#define OP_INTERRUPT_STATUS_RESET_PHYRSTB							16
#define OP_INTERRUPT_STATUS_RESET_MDIO								17

#define MACSEC_GLB_ISR_EGRESS_EVENTS				0
#define MACSEC_GLB_ISR_INGRESS_EVENTS				1

#define AIC_INTERRUPT_DROP_CLASS_EVENTS				0
#define AIC_INTERRUPT_DROP_PP_EVENTS				1
#define AIC_INTERRUPT_ENG_IRQ_EVENTS				2
#define AIC_INTERRUPT_DROP_CC_EVENTS				3
#define AIC_INTERRUPT_SA_PN_THR_EVENTS				4
#define AIC_INTERRUPT_SA_EXPIRED_EVENTS				5



typedef unsigned long		u32;
typedef unsigned short		u16;
typedef unsigned char		u8;
typedef short				s16;

typedef enum {
	E_NOERR	= 0x00,

	E_INVALD	= 0xFC,	/* Invalid argument or value */
	E_NOTRDY	= 0xFD,	/* Not Ready to Access */
	E_TIMOUT	= 0xFE,	/* Time out */
	E_FAILED	= 0xFF	/* Unexpected Result */
} Error_Code;

typedef enum {
	OP_STATE_STANDBY	 = 0x01,
	OP_STATE_NORMAL,	// 2
	OP_STATE_SAFETY,	// 3
	OP_STATE_UNKNOWN = 0xFF
} OP_State;

typedef enum {
	OP_CMD_GOTO_STANDBY = 1,
	OP_CMD_GOTO_NORMAL = 2,
	OP_CMD_GOTO_SLEEP = 3
} OP_CMD;

typedef enum {
	Speed_LinkDown = 0,
	Speed_100BaseT1 = 1,
	Speed_1000BaseT1,		// 2
} Link_Speed;

typedef enum {
	PCS_STATE_DISABLE_TRANSMITTER	= 0x01,
	PCS_STATE_SLAVE_SILENT,			// 0x02,
	PCS_STATE_TRAINING_STATE,		// 0x03,
	PCS_STATE_SEND_IDLE,			// 0x04,
	PCS_STATE_SEND_IDLE_OR_DATA,	// 0x05,
	PCS_STATE_LINK_SYNCHRONIZATION,	// 0x06,
	PCS_STATE_SILENT_WAIT,			// 0x07,
	PCS_STATE_LINK_UP,				// 0x08,
	PCS_STATE_UNKNOWN = 0xFF
} PCS_State;

typedef enum {
	CABLE_NORMAL = 0x01,
	CABLE_OPEN = 0x02,
	CABLE_SHORT = 0x03,
	CABLE_UNKNOWN = 0xFF
} Cable_Type;

typedef enum {
	Typical_xMII_3V3 =1, 
	Typical_xMII_2V5 =2, 
	Typical_xMII_1V8 =3
} RGMII_Voltage;

typedef enum {
	TurnOn =1,
	TurnOnWithFlowCtrlOn =2,
	TurnOff=3,
} Control;

typedef enum {
	Egress = 1, 
	Ingress =2, 
} Egress_Ingress;

typedef enum {
	SAK1 = 1,
	SAK2 = 2
} SAK_n;

typedef enum {
	SAK1_to_SAK2 = 1,
	SAK2_to_SAK1 = 2
} Switch_SAK;



void wait_x_ms( u32 count ); 

u8 RTL9010Bx_Initial_Configuration(void);
u8 RTL9010Bx_Initial_Configuration_Check(void);
u8 RTL9010Bx_Initial_With_AN_Configuration(void);
u8 RTL9010Bx_Initial_With_AN_Configuration_Check(void);
u8 RTL9010Bx_Initial_With_100M_TC10_Configuration(void);
u8 RTL9010Bx_Initial_With_100M_TC10_Configuration_Check(void);
u8 RTL9010Bx_Initial_With_AN_100M_TC10_Configuration(void);
u8 RTL9010Bx_Initial_With_AN_100M_TC10_Configuration_Check(void);
u8 RTL9010Bx_GetLinkStatus(u8 SpeedType);
u8 RTL9010Bx_Soft_Reset(void);
u8 RTL9010Bx_CableFaultLocationAndDiagnosis(u16* cable_length);
u8 RTL9010Bx_SQI(u8 SpeedType);
u8 RTL9010Bx_Check_Linkup(void);
u8 RTL9010Bx_Sleep_request(void);
u8 RTL9010Bx_Sleep_initial(void);
u8 RTL9010Bx_Sleep_reject(void);
u8 RTL9010Bx_EEE_function(void);
u8 RTL9010Bx_PHY_mode_EEE_function(void);
u8 RTL9010Bx_TRX_LPI_Indication(u16* trx_lpi_indication);
u8 RTL9010Bx_General_Interrupt_status(u16* general_int_status);
u8 RTL9010Bx_General_purpose_sub_status(u16* general_sub_status);
u8 RTL9010Bx_General_Interrupt_setting(void);
u8 RTL9010Bx_OP_Interrupt_setting(void);
u8 RTL9010Bx_OP_Interrupt_status(u32* op_status);
u8 RTL9010Bx_HOSTCMD_send_rwake(void);
u8 RTL9010Bx_Set_OP_state(u8 op_cmd);
u8 RTL9010Bx_Get_OP_state(void);
u8 RTL9010Bx_Disable_lwake_to_rwake(void);
u8 RTL9010Bx_Enable_lwake_to_rwake(void);
u8 RTL9010Bx_Check_lwake_to_rwake(void);
u8 RTL9010Bx_Check_rwake_en_dis(void);
u8 RTL9010Bx_Disable_rwake(void);
u8 RTL9010Bx_Enable_rwake(void);
u8 RTL9010Bx_Get_lwake_edge(void);
u8 RTL9010Bx_Set_lwake_edge(u8 edge_select);
u8 RTL9010Bx_Sleep_cap_check(void);
u8 RTL9000Cx_xMII_IO_Select(u8 RGMII_Voltage);
u8 RTL9010Bx_PHY_ready(void);
u8 RTL9010Bx_Get_PCS_status(u8 SpeedType);
u8 RTL9010Bx_PCS_loopback(void);
u8 RTL9010Bx_MDI_loopback(void);
u8 RTL9010Bx_Remote_loopback(void);
u8 RTL9010Bx_IOL_test(u8 TestMode_selection, u8 SpeedType);
u8 RTL9010Bx_MACSEC_Interrupt_setting(void);
u8 RTL9010Bx_MACSEC_GLB_Interrupt_Status(u32* MACSEC_GLB_ISR);
u8 RTL9010Bx_MACSEC_AIC_Interrupt_Status(u32* MACSEC_AIC_STATUS);


u8 RTL9010Bx_MACsec_Enable(u8 Control);
u8 RTL9010Bx_MACsec_Egress_Check_Access_Correct(void);
u8 RTL9010Bx_MACsec_Ingress_Check_Access_Correct(void);
u32 RTL9010Bx_MACsec_Egress_Read(u16 Address);
u32 RTL9010Bx_MACsec_Ingress_Read(u16 Address);
u8 RTL9010Bx_MACsec_Egress_Write(u16 Address, u32 reg_value);
u8 RTL9010Bx_MACsec_Ingress_Write(u16 Address, u32 reg_value);

u8 RTL9010Bx_Start_MACsec_Function(void);

u32 RTL9010Bx_Monitor_SAK_Packet_Number(u8 SAK_n);
u8 RTL9010Bx_Switch_Egress_SAK(u8 SW_SAK);
u8 RTL9010Bx_Update_SAK1(u8 Dir);
u8 RTL9010Bx_Update_SAK2(u8 Dir);

u32 RTL9010Bx_Egress_MIBCounter_Encrypted_OutPktBytes(u8 SAK_n);
u32 RTL9010Bx_Egress_MIBCounter_Encrypted_OutPkts(u8 SAK_n);
u32 RTL9010Bx_Egress_MIBCounter_Encrypted_OutPktsTooLong(u8 SAK_n);
u32 RTL9010Bx_Ingress_MIBCounter_Decrypted_InPktBytes(u8 SAK_n);
u32 RTL9010Bx_Ingress_MIBCounter_Decrypted_InPktsOK(u8 SAK_n);
u32 RTL9010Bx_Ingress_MIBCounter_Decrypted_InPktsNotValid(u8 SAK_n);
u32 RTL9010Bx_Ingress_MIBCounter_Decrypted_InPktsDelayed(u8 SAK_n);
u32 RTL9010Bx_Ingress_MIBCounter_Decrypted_InPktsLate(u8 SAK_n);
u32 RTL9010Bx_Ingress_MIBCounter_Decrypted_InPktsInvalid(u8 SAK_n);
u32 RTL9010Bx_Ingress_MIBCounter_Decrypted_InPktsNotUsingSA(u8 SAK_n);
u32 RTL9010Bx_Ingress_MIBCounter_Decrypted_InPktsUnusedSA(u8 SAK_n);
u32 RTL9010Bx_Ingress_MIBCounter_Decrypted_InPktsUntaggedHit(u8 SAK_n);

u8 RTL9010Bx_MACsec_Initial_Configuration_example_1(void);
u8 RTL9010Bx_MACsec_Initial_Configuration_example_2(void);

