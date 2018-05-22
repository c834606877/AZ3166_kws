#include "AppDefine.h"
#include "oled.h"
#include "HTS221Sensor.h"
#include "lis2mdl_class.h"
#include "LPS22HBSensor.h"
#include "LSM6DSLSensor.h"

#define az3166_sensors_demo_log(M, ...) custom_log("AZ3166_SENSORS", M, ##__VA_ARGS__)
char page = 1;
extern bool _wifiConnected ;

void BUTTON_A_CB(void * );
void BUTTON_B_CB( );
void do_nothing();


void sensor_update_thread(mico_thread_arg_t arg)
{
    app_context_t* app_context = (app_context_t *)arg;
    MicoGpioInitialize( (mico_gpio_t) LED_WIFI_PORT, OUTPUT_PUSH_PULL );
    MicoGpioOutputLow( (mico_gpio_t) LED_WIFI_PORT );
    MicoGpioInitialize( (mico_gpio_t) LED_AZURE_PORT, OUTPUT_PUSH_PULL );
    MicoGpioOutputLow( (mico_gpio_t) LED_AZURE_PORT );
    MicoGpioInitialize( (mico_gpio_t) LED_USER_PORT, OUTPUT_PUSH_PULL );
    MicoGpioOutputLow( (mico_gpio_t) LED_USER_PORT );
    MicoGpioInitialize( (mico_gpio_t) MICO_GPIO_31, OUTPUT_PUSH_PULL );
    MicoGpioOutputLow( (mico_gpio_t) MICO_GPIO_31 );
    MicoGpioInitialize( (mico_gpio_t) MICO_GPIO_27, OUTPUT_PUSH_PULL );
    MicoGpioOutputLow( (mico_gpio_t) MICO_GPIO_27 );
    MicoGpioInitialize( (mico_gpio_t) MICO_GPIO_12, OUTPUT_PUSH_PULL );
    MicoGpioOutputLow( (mico_gpio_t) MICO_GPIO_12 );

   // init OLED
   uint32_t display_row_num[4] = {OLED_DISPLAY_ROW_1, OLED_DISPLAY_ROW_2, OLED_DISPLAY_ROW_3, OLED_DISPLAY_ROW_4};
   char oled_show_line[4][OLED_DISPLAY_MAX_CHAR_PER_ROW + 1] = { "Mico AZ3166", "\0", "\0", "\0" };   // max char each line
   OLED_Init();
   OLED_FillAll();
   OLED_Clear();

   // set RGB
   uint8_t cur_rgb = 0;

   /*init HTS221*/
   HTS221Sensor hts221( MICO_I2C_1 );
   float hts221_humidity = 0;
   float hts221_temp = 0;
   hts221.init( NULL );
   hts221.enable( );

   /*init Magnetometer*/
   int32_t p_data[3] = { 0 };
   LIS2MDL lis2mdl( MICO_I2C_1 );
   lis2mdl.init(NULL);

   /*button callback*/
   MicoGpioEnableIRQ( USER_BUTTON_A_PORT, IRQ_TRIGGER_RISING_EDGE, (void (*)(void *))&BUTTON_A_CB, app_context );
   MicoGpioEnableIRQ( USER_BUTTON_B_PORT, IRQ_TRIGGER_RISING_EDGE, (void (*)(void *))&BUTTON_B_CB, NULL );

   /*init Accelerometer*/
   int32_t x_axes[3] = { 0 };
   int32_t g_axes[3] = { 0 };
   LSM6DSLSensor lsm6dsl( MICO_I2C_1, (mico_gpio_t)NULL, (mico_gpio_t)NULL );
   while(lsm6dsl.init( NULL ));
   lsm6dsl.enable_x( );
   lsm6dsl.enable_g( );

   /*init LPS22HB */
   float lps22hb_temp_data = 0;
   float lps22hb_pres_data = 0;
   LPS22HBSensor lps22hb(MICO_I2C_1);
   lps22hb.init(NULL);
   lps22hb.enable();




   while (1){
        if ( cur_rgb % 3 == 0 )
            {
            MicoGpioOutputTrigger( LED_WIFI_PORT );
            MicoGpioOutputTrigger( MICO_GPIO_31 );
            }
        if ( cur_rgb % 3 == 1 )
        {
            MicoGpioOutputTrigger( LED_AZURE_PORT );
            MicoGpioOutputTrigger( MICO_GPIO_27 );
        }

        if ( cur_rgb % 3 == 2 )
        {
            MicoGpioOutputTrigger( LED_USER_PORT );
            MicoGpioOutputTrigger( MICO_GPIO_12 );
        }

        mico_rtos_delay_milliseconds( 2000 );

        OLED_ShowString(OLED_DISPLAY_COLUMN_START, 0, "Mico AZ3166");

        cur_rgb++;
        continue;




        mico_rtos_delay_milliseconds( 2000 );

        cur_rgb++;
   }
}



void BUTTON_A_CB( void* arg )
{
    app_context_t* app_context = (app_context_t *)arg;
    app_context->appStatus.button_A = 1;

}

void BUTTON_B_CB( )
{
    page = (page==1?2:1);
}

void do_nothing()
{
    int i = 0;
    i++;
}
