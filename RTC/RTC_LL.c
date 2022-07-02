#include "RTC_LL.h"

#define RTC_TIMEOUT_VALUE       1000

static const uint32_t LL_MONTH_ARRAY[] = {
        [JANUARY] = LL_RTC_MONTH_JANUARY,
        [FEBRUARY] = LL_RTC_MONTH_FEBRUARY,
        [MARCH] = LL_RTC_MONTH_MARCH,
        [APRIL] = LL_RTC_MONTH_APRIL,
        [MAY] = LL_RTC_MONTH_MAY,
        [JUNE] = LL_RTC_MONTH_JUNE,
        [JULY] = LL_RTC_MONTH_JULY,
        [AUGUST] = LL_RTC_MONTH_AUGUST,
        [SEPTEMBER] = LL_RTC_MONTH_SEPTEMBER,
        [OCTOBER] = LL_RTC_MONTH_OCTOBER,
        [NOVEMBER] = LL_RTC_MONTH_NOVEMBER,
        [DECEMBER] = LL_RTC_MONTH_DECEMBER
};

static const uint32_t LL_WEEKDAY_ARRAY[] = {
        [MONDAY] = LL_RTC_WEEKDAY_MONDAY,
        [TUESDAY] = LL_RTC_WEEKDAY_TUESDAY,
        [WEDNESDAY] = LL_RTC_WEEKDAY_WEDNESDAY,
        [THURSDAY] = LL_RTC_WEEKDAY_THURSDAY,
        [FRIDAY] = LL_RTC_WEEKDAY_FRIDAY,
        [SATURDAY] = LL_RTC_WEEKDAY_SATURDAY,
        [SUNDAY] = LL_RTC_WEEKDAY_SUNDAY,
};

static uint32_t convertToMonth(uint32_t month);
static void setRTCAlarm(LL_RTC_AlarmTypeDef *RTC_AlarmPointer, uint8_t hours, uint8_t minutes, uint8_t seconds);
static bool waitForFlagIsSet(uint8_t alarmFlag);


Date dateNow() {
    uint32_t year = 2000 + __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetYear(RTC));
    uint8_t month = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetMonth(RTC));
    uint32_t day = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetDay(RTC));
    return dateOf(year, convertToMonth(month), day);
}

Time timeNow() {
    uint8_t hours = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetHour(RTC));
    uint8_t minutes = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetMinute(RTC));
    uint8_t seconds = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC));
    return timeOf(hours, minutes, seconds);
}

DateTime dateTimeNow() {
    Time time = timeNow();
    Date date = dateNow();
    return dateTimeFrom(&date, &time);
}

ZonedDateTime zonedDateTimeNow(const TimeZone *zone) {
    DateTime dateTime = dateTimeNow();
    return zonedDateTimeOfDateTime(&dateTime, zone);
}

void setRTCTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    LL_RTC_TimeTypeDef RTC_TimeStruct = {0};
    RTC_TimeStruct.Hours = hours;
    RTC_TimeStruct.Minutes = minutes;
    RTC_TimeStruct.Seconds = seconds;
    LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BIN, &RTC_TimeStruct);
}

void setRTCDate(int64_t year, Month month, uint8_t day, DayOfWeek dayOfWeek) {
    LL_RTC_DateTypeDef RTC_DateStruct = {0};
    RTC_DateStruct.WeekDay = LL_WEEKDAY_ARRAY[dayOfWeek];
    RTC_DateStruct.Month = LL_MONTH_ARRAY[month];
    RTC_DateStruct.Year = year % 100;
    RTC_DateStruct.Day = day;
    LL_RTC_DATE_Init(RTC, LL_RTC_FORMAT_BIN, &RTC_DateStruct);
}

void setRTCDateTime(DateTime *dateTime) {
    if (!isDateTimeValid(dateTime)) return;
    Date date = dateTime->date;
    Time time = dateTime->time;
    setRTCTime(time.hours, time.minutes, time.seconds);
    setRTCDate(date.year, date.month, date.day, date.weekDay);
}

void setRTCAlarm_A(uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds, uint32_t alarmMask) {
    LL_RTC_AlarmTypeDef RTC_AlarmStruct = {0};

    setRTCAlarm(&RTC_AlarmStruct, hours, minutes, seconds);
    RTC_AlarmStruct.AlarmMask = alarmMask;
    RTC_AlarmStruct.AlarmDateWeekDaySel = LL_RTC_ALMA_DATEWEEKDAYSEL_DATE;
    RTC_AlarmStruct.AlarmDateWeekDay = day;
    LL_RTC_ALMA_Init(RTC, LL_RTC_FORMAT_BIN, &RTC_AlarmStruct);

    LL_RTC_DisableWriteProtection(RTC);
    LL_RTC_ALMA_Disable(RTC);
    LL_RTC_ClearFlag_ALRA(RTC);

    if (!waitForFlagIsSet(RTC_ISR_ALRAWF)) {
        LL_RTC_EnableWriteProtection(RTC);
        Error_Handler();
    }
    LL_RTC_ALMA_Enable(RTC);
    LL_RTC_EnableIT_ALRA(RTC);

    LL_RTC_SetAlarmOutEvent(RTC, LL_RTC_ALARMOUT_ALMA);

    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);
    LL_EXTI_EnableEvent_0_31(LL_EXTI_LINE_17);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_17);
    LL_RTC_EnableWriteProtection(RTC);
}

void setRTCAlarm_B(uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds, uint32_t alarmMask) {
    LL_RTC_AlarmTypeDef RTC_AlarmStruct = {0};

    setRTCAlarm(&RTC_AlarmStruct, hours, minutes, seconds);
    RTC_AlarmStruct.AlarmMask = alarmMask;
    RTC_AlarmStruct.AlarmDateWeekDaySel = LL_RTC_ALMA_DATEWEEKDAYSEL_DATE;
    RTC_AlarmStruct.AlarmDateWeekDay = day;
    LL_RTC_ALMB_Init(RTC, LL_RTC_FORMAT_BIN, &RTC_AlarmStruct);

    LL_RTC_DisableWriteProtection(RTC);
    LL_RTC_ALMB_Disable(RTC);
    LL_RTC_ClearFlag_ALRB(RTC);

    if (!waitForFlagIsSet(RTC_ISR_ALRBWF)) {
        LL_RTC_EnableWriteProtection(RTC);
        Error_Handler();
    }
    LL_RTC_ALMB_Enable(RTC);
    LL_RTC_EnableIT_ALRB(RTC);

    LL_RTC_SetAlarmOutEvent(RTC, LL_RTC_ALARMOUT_ALMB);

    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);
    LL_EXTI_EnableEvent_0_31(LL_EXTI_LINE_17);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_17);
    LL_RTC_EnableWriteProtection(RTC);
}

static uint32_t convertToMonth(uint32_t month) {
    for (Month i = 0; i < ARRAY_SIZE(LL_MONTH_ARRAY); i++) {
        if (month == LL_MONTH_ARRAY[i]) {
            return i;
        }
    }
    return 0;
}

static void setRTCAlarm(LL_RTC_AlarmTypeDef *RTC_AlarmPointer, uint8_t hours, uint8_t minutes, uint8_t seconds) {
    RTC_AlarmPointer->AlarmTime.Hours = hours;
    RTC_AlarmPointer->AlarmTime.Minutes = minutes;
    RTC_AlarmPointer->AlarmTime.Seconds = seconds;
}

static bool waitForFlagIsSet(uint8_t alarmFlag) {
    uint32_t count = RTC_TIMEOUT_VALUE  * (SystemCoreClock / 32U / 1000U);
    while ((READ_BIT(RTC->ISR, alarmFlag) == (alarmFlag)) == RESET && count-- > 0);
    return count != 0;
}