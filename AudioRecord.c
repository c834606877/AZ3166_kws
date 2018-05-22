#include "mico.h"
#include "MiCODrivers/MiCODriverI2c.h"
#include "nau88c10.h"
#include "stm32f4xx_hal.h"
#include "audio_config.h"
#include "audio_op.h"
#include "AppDefine.h"




#define app_log(M, ...) custom_log("APP", M, ##__VA_ARGS__)
#define app_log_trace() custom_log_trace("APP")
#define az3166_sensors_demo_log(M, ...) custom_log("AZ3166_SENSORS", M, ##__VA_ARGS__)




int16_t buf_tx_rx[2][BUFF_SIZE] = { 0 };//  channel(L/R) and in/out ;
int16_t audio_buff[AUDIO_BUFF_SIZE] = {0};


mico_i2c_device_t device;
mico_semaphore_t  _recordComplated_sem = NULL;





static volatile char flag = 0;


typedef enum
{
    AUDIO_STATE_IDLE = 0,
    AUDIO_STATE_INIT,
    AUDIO_STATE_PLAYING,
} AUDIO_PLAYBACK_StateTypeDef;
static AUDIO_PLAYBACK_StateTypeDef audio_state;



void BSP_AUDIO_IN_TransferComplete_CallBack( void );
void BSP_AUDIO_OUT_TransferComplete_CallBack( void );
void BSP_AUDIO_OUT_Error_CallBack( void );

uint8_t last_vol;





void audio_thread(void *inContext)
{
    app_log("Audio Thread Started£¡");

    app_context_t* app_context = (app_context_t *)inContext;

    mico_rtos_init_semaphore(&_recordComplated_sem, 1);

    app_context->appStatus.audio_vol = app_context->appConfig->audio_vol;
    last_vol = 128;


    /*init audio*/
    audio_state = AUDIO_STATE_INIT;
    BSP_AUDIO_IN_OUT_Init( OUTPUT_DEVICE_AUTO, I2S_DATAFORMAT_16B, I2S_AUDIOFREQ_16K );


    /*record*/
    audio_state = AUDIO_STATE_PLAYING;
    BSP_AUDIO_In_Out_Transfer( buf_tx_rx[0], buf_tx_rx[1], BUFF_SIZE );

    while(1)
    {
        if( last_vol != app_context->appStatus.audio_vol)
        {
            last_vol = app_context->appStatus.audio_vol;
            BSP_AUDIO_OUT_SetVolume(last_vol);

        }
        mico_thread_sleep(5);
    }
    app_log("return");
    return ;
}


void BSP_AUDIO_IN_TransferComplete_CallBack( void )
{
    if ( audio_state == AUDIO_STATE_PLAYING )
    {
        if ( 0 == flag ) {
            BSP_AUDIO_In_Out_Transfer( buf_tx_rx[1], buf_tx_rx[0], BUFF_SIZE);
            for(int i = 0; i< AUDIO_BUFF_SIZE; i++)
                 audio_buff[i] = buf_tx_rx[1][i<<1];
            flag = 1;
        }
        else {
            BSP_AUDIO_In_Out_Transfer( buf_tx_rx[0], buf_tx_rx[1], BUFF_SIZE);
            for(int i = 0; i< AUDIO_BUFF_SIZE; i++)
                  audio_buff[i] = buf_tx_rx[0][i<<1];
            MicoGpioOutputTrigger( LED_USER_PORT );
            flag = 0;
        }
        mico_rtos_set_semaphore (_recordComplated_sem);

    }
}

void BSP_AUDIO_OUT_TransferComplete_CallBack( void )
{

}

void BSP_AUDIO_OUT_Error_CallBack( void )
{

}
