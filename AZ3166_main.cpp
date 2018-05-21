/**
 ******************************************************************************
 * @file    az3166_sensors_demo.cpp
 * @author  Yangjie Gu
 * @version V1.0.0
 * @date    5-June-2017
 * @brief   az3166 sensors demo
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ******************************************************************************
 */

#include "AppDefine.h"
#include "mico.h"


#define app_log(M, ...) custom_log("APP", M, ##__VA_ARGS__)
#define app_log_trace() custom_log_trace("APP")


bool _wifiConnected = false;
mico_semaphore_t  _wifiConnected_sem = NULL;



extern "C" {          /*C++编译器才能支持，C编译器不支持*/

    void audio_thread( mico_thread_arg_t arg );
    void mqtt_client_thread( mico_thread_arg_t arg );
    void remoteTcpClient_thread(mico_thread_arg_t arg );
}
void sensor_update_thread(mico_thread_arg_t arg);
void audio_recognition_thread( mico_thread_arg_t arg );


/* MICO system callback: Restore default configuration provided by application */
void appRestoreDefault_callback( void * const user_config_data, uint32_t size )
{
    UNUSED_PARAMETER( size );
    application_config_t* appConfig = (application_config_t*)user_config_data;
    appConfig->configDataVer = CONFIGURATION_VERSION;
    sprintf(appConfig->remoteServerDomain, DEAFULT_REMOTE_SERVER);
    appConfig->remoteServerPort = DEFAULT_REMOTE_SERVER_PORT;
    appConfig->audio_vol = DEFAULT_AUDIO_VOL;

}

void clientNotify_WifiStatusHandler(int event, void* arg )
{
  app_log_trace();
  (void)arg;
  switch (event) {
  case NOTIFY_STATION_UP:
    _wifiConnected = true;
    mico_rtos_set_semaphore(&_wifiConnected_sem);
    mico_rtos_set_semaphore(&_wifiConnected_sem);
    break;
  case NOTIFY_STATION_DOWN:
    _wifiConnected = false;
    break;
  default:
    break;
  }
  return;
}

int main( void )
{
    _exit:
    app_log_trace();
    OSStatus err = kNoErr;

    app_context_t* app_context;
    mico_Context_t* mico_context;

    /* Create application context */
    app_context = (app_context_t *) calloc( 1, sizeof(app_context_t) );
    require_action( app_context, _exit, err = kNoMemoryErr );

    /* Create mico system context and read application's config data from flash */
    mico_context = mico_system_context_init( sizeof(application_config_t) );
    app_context->appConfig = (application_config_t *)mico_system_context_get_user_data( mico_context );



    /* user params restore check */
    if(app_context->appConfig->configDataVer != CONFIGURATION_VERSION){
      app_log("WARNGIN: user params restored! %d", app_context->appConfig->configDataVer);
      err = mico_system_context_restore(mico_context);
      require_noerr( err, _exit );
    }


    /* mico system initialize */
    err = mico_system_init( mico_context );
    require_noerr( err, _exit );

    mico_rtos_init_semaphore(&_wifiConnected_sem, 2);
    /* Regisist notifications */
    err = mico_system_notify_register( mico_notify_WIFI_STATUS_CHANGED, (void *)clientNotify_WifiStatusHandler, NULL );
    require_noerr( err, _exit );


    /* MQTT Client */
    err = mico_rtos_create_thread( NULL, MICO_APPLICATION_PRIORITY, "MQTT Client", mqtt_client_thread,
                                   STACK_SIZE_MQTT_CLIENT_THREAD, (mico_thread_arg_t)app_context );
    require_noerr_action( err, _exit, app_log("ERROR: Unable to start the MQTT Client thread.") );

    /* Audio Record */
    err = mico_rtos_create_thread( NULL, MICO_APPLICATION_PRIORITY, "Audio Record Client", audio_thread,
                                   STACK_SIZE_MQTT_CLIENT_THREAD, (mico_thread_arg_t)app_context );
    require_noerr_action( err, _exit, app_log("ERROR: Unable to start the Audio Record thread.") );



    /* Sensor Update */
    err = mico_rtos_create_thread( NULL, MICO_APPLICATION_PRIORITY, "Sensor Update Client", sensor_update_thread,
                                   STACK_SIZE_MQTT_CLIENT_THREAD, (mico_thread_arg_t)app_context );
    require_noerr_action( err, _exit, app_log("ERROR: Unable to start the Sensor Update thread.") );

    /* Remote TCP Record */
//    err = mico_rtos_create_thread( NULL, MICO_APPLICATION_PRIORITY, "Remote Tcp Client", remoteTcpClient_thread,
//                                   STACK_SIZE_MQTT_CLIENT_THREAD, (mico_thread_arg_t)app_context );
//    require_noerr_action( err, _exit, app_log("ERROR: Unable to start the Remote Tcp Client thread.") );
//

    /* Audio Recognition */
    err = mico_rtos_create_thread( NULL, MICO_APPLICATION_PRIORITY, "Audio Recognition Client", audio_recognition_thread,
                                   STACK_SIZE_MQTT_CLIENT_THREAD, (mico_thread_arg_t)app_context );
    require_noerr_action( err, _exit, app_log("ERROR: Unable to start the Audio Recognition thread.") );
  return 0;

}


