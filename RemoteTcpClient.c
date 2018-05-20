

#include "mico.h"

#include "AppDefine.h"

#define client_log(M, ...) custom_log("TCP client", M, ##__VA_ARGS__)
#define client_log_trace() custom_log_trace("TCP client")

#define CLOUD_RETRY  1

extern bool _wifiConnected ;
extern mico_semaphore_t  _wifiConnected_sem;
extern mico_semaphore_t  _recordComplated_sem;
extern int16_t audio_buff[AUDIO_BUFF_SIZE];

void remoteTcpClient_thread(uint32_t inContext)
{
  client_log_trace();
  OSStatus err = kUnknownErr;
  int len;
  app_context_t * app_context = (app_context_t *)inContext;
  struct sockaddr_in addr;
  fd_set readfds;
  fd_set writeSet;
  char ipstr[16];
  struct timeval t;
  int remoteTcpClient_fd = -1;


  LinkStatusTypeDef wifi_link;
  int sent_len, errno;
  struct hostent* hostent_content = NULL;
  char **pptr = NULL;
  struct in_addr in_addr;
  client_log("enter connent remote thread");

  
  
  err = micoWlanGetLinkStatus( &wifi_link );
  require_noerr( err, exit );
  
  if( wifi_link.is_connected == true )
    _wifiConnected = true;
  
  client_log("Wifi_link is connected, now enter the while");

  while(1) {
    if(remoteTcpClient_fd == -1 ) {
      if(_wifiConnected == false){
          client_log("wait for semaphore");
        require_action_quiet(mico_rtos_get_semaphore(&_wifiConnected_sem, 200000) == kNoErr, Continue, err = kTimeoutErr);
      }
      

      hostent_content = gethostbyname( (char *)app_context->appConfig->remoteServerDomain );
      require_action_quiet( hostent_content != NULL, exit, err = kNotFoundErr);
      pptr=hostent_content->h_addr_list;

      client_log("gethostbyname : %d.%d.%d.%d", (*pptr)[0], (*pptr)[1], (*pptr)[2], (*pptr)[3]);
      in_addr.s_addr = *(uint32_t *)(*pptr);
      strcpy( ipstr, inet_ntoa(in_addr));
      remoteTcpClient_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      addr.sin_family = AF_INET;
      addr.sin_addr.s_addr = inet_addr(ipstr);
      addr.sin_port = htons(app_context->appConfig->remoteServerPort);
      
      err = connect(remoteTcpClient_fd, (struct sockaddr *)&addr, sizeof(addr));
      require_noerr_quiet(err, ReConnWithDelay);
      client_log("Remote server connected at port: %d, fd: %d",  app_context->appConfig->remoteServerPort,
                 remoteTcpClient_fd);


    }else{
        //uint16_t nonBlock = 1;
        //setsockopt(remoteTcpClient_fd, SOL_SOCKET, SO_BLOCKMODE,&nonBlock, sizeof(nonBlock) );

        require_action_quiet(mico_rtos_get_semaphore(&_recordComplated_sem, 200000) == kNoErr, Continue, err = kTimeoutErr);

        sent_len = write(remoteTcpClient_fd, audio_buff, AUDIO_BUFF_SIZE * sizeof(int16_t));


        if (sent_len <= 0) {
            len = sizeof(errno);
            getsockopt(remoteTcpClient_fd, SOL_SOCKET, SO_ERROR, &errno, (socklen_t *)&len);

            if (errno != ENOMEM) {
                client_log("write error, fd: %d, errno %d", remoteTcpClient_fd, errno );
                goto ReConnWithDelay;
            }
        }

      }


    Continue:    
      continue;
      
    ReConnWithDelay:
        if(remoteTcpClient_fd != -1){
          SocketClose(&remoteTcpClient_fd);
        }
        mico_rtos_thread_sleep(CLOUD_RETRY);

  }
    
exit:

  client_log("Exit: Remote TCP client exit with err = %d", err);
  mico_rtos_delete_thread(NULL);
  return;
}

