/*
 * kws.cpp
 *
 *  Created on: 2018Äê5ÔÂ20ÈÕ
 *      Author: Administrator
 */

#include "mico.h"

#include "AppDefine.h"

#include "KWS/KWS_dnn/kws_dnn.h"
#include "MFCC/mfcc.h"
#include "NN/DNN/dnn.h"

#define client_log(M, ...) custom_log("Audio Recognition", M, ##__VA_ARGS__)
#define client_log_trace() custom_log_trace("Audio Recognition")


void run_kws();

KWS *kws = NULL;

void audio_recognition_thread(void *inContext)
{
    client_log("Audio Recognition Thread Started£¡");

    kws = new KWS(recording_win,averaging_window_len);

    kws->start_kws();

    while(1)
    {
        mico_thread_sleep(10);
    }
}

void run_kws()
{
  kws->extract_features();    //extract mfcc features
  kws->classify();        //classify using dnn
  kws->average_predictions();

  int max_ind = kws->get_top_class(kws->averaged_output);
  if(kws->averaged_output[max_ind]>detection_threshold*128/100)
    sprintf(lcd_output_string,"%d%% %s",((int)kws->averaged_output[max_ind]*100/128),output_class[max_ind]);

}
