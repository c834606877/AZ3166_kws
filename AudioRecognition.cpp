/*
 * kws.cpp
 *
 *  Created on: 2018Äê5ÔÂ20ÈÕ
 *      Author: Administrator
 */

#include "mico.h"
#include "oled.h"
#include "AppDefine.h"

#include "KWS/KWS_dnn/kws_dnn.h"
#include "KWS/kws.h"
#include "MFCC/mfcc.h"
#include "NN/DNN/dnn.h"

#define client_log(M, ...) custom_log("Audio Recognition", M, ##__VA_ARGS__)
#define client_log_trace() custom_log_trace("Audio Recognition")



extern  int16_t audio_buff[AUDIO_BUFF_SIZE];
extern  mico_semaphore_t  _recordComplated_sem;



void run_kws();

KWS_DNN *kws = NULL;
int recording_win = 3;
int averaging_window_len = 3;
int detection_threshold = 88;


void audio_recognition_thread( mico_thread_arg_t arg )
{
    client_log("Audio Recognition Thread Started£¡");


    kws = new KWS_DNN(audio_buff, recording_win, averaging_window_len);

    //kws->start_kws();
    while (_recordComplated_sem == 0) mico_thread_sleep(1);

    while(1)
    {
        mico_rtos_get_semaphore(&_recordComplated_sem, 20000);
        run_kws();
    }
}


char lcd_output_string[128];
char output_class[12][9] = {"Silence",
                            "Unknown",
                            "stop   ",
                            "wow    ",
                            "six    ",
                            "seven  ",
                            "right  ",
                            "happy  ",
                            "house  ",
                            "left   ",
                            "right  ",
                            "go     "
                        };
void run_kws()
{
    OLED_ShowString(OLED_DISPLAY_COLUMN_START, 2, "KeyWord Spot:");

    kws->extract_features();    //extract mfcc features
    kws->classify();        //classify using dnn
    kws->average_predictions();

    int max_ind = kws->get_top_class(kws->averaged_output);
    if(kws->averaged_output[max_ind]>detection_threshold*128/100)
      sprintf(lcd_output_string,"%d%% %s",((int)kws->averaged_output[max_ind]*100/128),output_class[max_ind]);
    OLED_ShowString(OLED_DISPLAY_COLUMN_START, 4, lcd_output_string);

}
