############################################################################### 
#
#  The MIT License
#  Copyright (c) 2016 MXCHIP Inc.
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy 
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights 
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is furnished
#  to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
#  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
############################################################################### 


NAME := App_AZ3166_demo

$(NAME)_SOURCES := AZ3166_main.cpp\
                   SensorUpdate.cpp\
                   AudioRecord.c\
                   AudioRecognition.cpp\
                   NN/nn.cpp\
                   NN/DNN/dnn.cpp\
                   MFCC/mfcc.cpp\
                   KWS/kws.cpp\
                   KWS/KWS_DNN/kws_dnn.cpp\
                   cmsis-nn/FullyConnectedFunctions/arm_fully_connected_q7.c\
                   cmsis-nn/ActivationFunctions/arm_relu_q7.c\
                   cmsis-nn/SoftmaxFunctions/arm_softmax_q7.c
                                      #RemoteTcpClient.c\

$(NAME)_COMPONENTS := drv_AZ3166\
                      drv_AZ3166.drv_VGM128064\
                      protocols/mqtt

GLOBAL_DEFINES += SSD1106_USE_I2C OLED_I2C_PORT=MICO_I2C_1 LED_WIFI_PORT=MICO_GPIO_2 LED_AZURE_PORT=MICO_GPIO_9 LED_USER_PORT=MICO_GPIO_16 \
                  USER_BUTTON_A_PORT=MICO_GPIO_38 USER_BUTTON_B_PORT=MICO_GPIO_33  ARM_MATH_CM4 ARCH_MAX __CC_

                  
$(NAME)_PREBUILT_LIBRARY := ./libarm_cortexM4l_math.a