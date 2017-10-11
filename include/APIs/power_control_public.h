
#ifndef _ALARM_PUBLIC_API_H
#define _ALARM_PUBLIC_API_H

typedef enum
{
	LED_OPEN,
	LED_CLOSE,
	LED_OPERATION_INVALID,
}LED_OPERATION;
int hdd_led_oper(LED_OPERATION led_oper);
int record_led_oper(LED_OPERATION led_oper);

typedef enum
{
	POWER_OPEN,
	POWER_CLOSE,
	OPERATION_INVALID,
}POWER_OPERATION;
int hdd_led_oper(LED_OPERATION led_oper);
int record_led_oper(LED_OPERATION led_oper);
int wifi_power_oper(POWER_OPERATION power_oper);
int lcd_power_oper(POWER_OPERATION power_oper);
int sd_power_oper(POWER_OPERATION power_oper);
int mic_pin_oper(void);//È¡·´
int charge_status_detect(unsigned char *charge_detect_status);
int send_signal(void);
int detect_power_signal(unsigned char *power_detect_pin_status);

typedef enum
{
	CAMERA_MODE,
	VGA_MODE,
	VIDEO_SOURCE_INVALID
}VIDEO_SOURCE_OPERATION;
int video_source_init_mode(VIDEO_SOURCE_OPERATION video_oper);
int video_source_control_oper(VIDEO_SOURCE_OPERATION video_oper);

int sonde_33_HZ(void);
int sonde_512_HZ(void);
int sonde_640_HZ(void);
int sonde_off(void);
int SOC_reset(void);
typedef enum
{
	AHD_MODE,
	TVI_MODE,
	AUDIO_SOURCE_INVALID
}AUDIO_SOURCE_OPERATION;
int audio_source_control_oper(AUDIO_SOURCE_OPERATION audio_oper);
typedef enum
{
	LCD_BACKLIGHT_DARK,
	LCD_BACKLIGHT_BRIGHT,
	LCD_BACKLIGHT_INVALID
}LCD_BACKLIGHT_TYPE;
int lcd_backlight_set(LCD_BACKLIGHT_TYPE backlight_type);
#endif



