#ifndef MODBUS_H_
#define MODBUS_H_

#include "usart.h"

int16_t validateFactoryConfigSensorType(uint16_t value);
int16_t validateFactoryConfigTempSensor(uint16_t value);

int16_t validateUserCfgTempUnit(uint16_t value);
int16_t validateUserCfgTempOffset(uint16_t value);
int16_t validateUserCfgFloatByteOrder(uint16_t value);
int16_t validateUserCfgFilterWindow(uint16_t value);
int16_t validateUserCfgAsynAcqInterval(uint16_t value);
int16_t validateUserCfgReserved(uint16_t value);

int16_t validateSensorCfgTempSource(uint16_t value);
int16_t validateSensorCfgTempManualInput(uint16_t value);
int16_t validateSensorCfgCellConstantCalibMode(uint16_t value);
int16_t validateSensorCfgCellConstant(uint16_t value);
int16_t validateSensorCfgCellConstantValue(uint16_t value);
int16_t validateSensorCfgEcRefTemperature(uint16_t value);
int16_t validateSensorCfgEcCompensationMode(uint16_t value);
int16_t validateSensorCfgCompensationCoefEC(uint16_t value);
int16_t validateSensorCfgCompensationCoefSalinity(uint16_t value);
int16_t validateSensorCfgCompensationCoefTDS(uint16_t value);
int16_t validateSensorCfgDefault(uint16_t value);

int16_t validateCalibUserCalibECFor1Pt(int32_t value);
int16_t validateCalibUserCalibECFor2Pt(uint16_t calibPointIndex, int32_t value);

int16_t validateCommParamProtocol(uint8_t value);
int16_t validateCommParamSlaveAddress(uint8_t value);
int16_t validateCommParamBaudrate(uint8_t value);
int16_t validateCommParamDataBits(uint8_t value);
int16_t validateCommParamParity(uint8_t value);
int16_t validateCommParamStopBits(uint8_t value);
int16_t validateCommParamRespDelay(uint8_t value);
int16_t validateCommParamActvOtptItvl(uint8_t value);

//modbus trans
uint16_t modbusRtuTransValidate(STRUCT_USART_INFO* pCommInfo);
void modbusRtuRequest2Respond(STRUCT_USART_INFO* pCommInfo);

//modbus
void modbusRtu_3(STRUCT_USART_INFO* pCommInfo);
void modbusRtu_4(STRUCT_USART_INFO* pCommInfo);
void modbusRtu_6(STRUCT_USART_INFO* pCommInfo);
void modbusRtu_16(STRUCT_USART_INFO* pCommInfo);

#endif /* MODBUS_H_ */
