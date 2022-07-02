#pragma once

#include "main.h"
#include "GlobalDateTime.h"


void setRTCTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void setRTCDate(int64_t year, Month month, uint8_t day, DayOfWeek dayOfWeek);

void setRTCDateTime(DateTime *dateTime);

void setRTCAlarm_A(uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds, uint32_t alarmMask);
void setRTCAlarm_B(uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds, uint32_t alarmMask);

static inline bool isRTCTimeSet() {
    return LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR0) != 0x32F2;
}
