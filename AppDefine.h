
#include "mico_common.h"


#define CONFIGURATION_VERSION               0x00007
#define DEAFULT_REMOTE_SERVER               "t.mypre.cn"
#define DEFAULT_REMOTE_SERVER_PORT          9997
#define DEFAULT_AUDIO_VOL                   128

#define STACK_SIZE_MQTT_CLIENT_THREAD  0x500




typedef struct {
    float hts221_humidity;
    float hts221_temp;

    /*init Accelerometer*/
    int32_t x_axes[3];
    int32_t g_axes[3];

    /*init LPS22HB */
    float lps22hb_temp_data;
    float lps22hb_press_data;

    /*init Magnetometer*/
    int32_t p_mags[3];
} sensor_status;

/*Running status*/
typedef struct  {
  /*Local clients port list*/
    sensor_status sensors;
    uint8_t audio_vol;
    uint8_t button_A;

} current_app_status_t;

/*Application's configuration stores in flash*/
typedef struct
{
  uint32_t          configDataVer;

  /*local services*/
  bool              remoteServerEnable;
  char              remoteServerDomain[64];
  int               remoteServerPort;
  uint8_t           audio_vol;

} application_config_t;

typedef struct _app_context_t
{
  /*Flash content*/
  application_config_t*     appConfig;

  /*Running status*/
  current_app_status_t      appStatus;
} app_context_t;



