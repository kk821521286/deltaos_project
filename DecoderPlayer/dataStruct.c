//
//  dataStruct.c
//  soapServer
//
//  Created by shanchun on 16/1/8.
//  Copyright © 2016年 shanchun. All rights reserved.
//

#include "dataStruct.h"
#include <stdio.h>

#define moduleConfigStruLen 5
#define channelConfigStruLen 9
#define fileInfoStruLen 7
#define recordConfigStruLen 7
#define streamConfigStruLen 8

ParamStru moduleConfigStru[moduleConfigStruLen] = {
    {"mid",int_type,sizeof(int),offset_not_count},
    {"name",str_type,64,0},
    {"alias",str_type,64,0},
    {"ip",str_type,32,0},
    {"cid_sequence",int_array_type,256*sizeof(int),0}
};
ParamStru channelConfigStru[channelConfigStruLen] = {
    {"mid",int_type,sizeof(int),offset_not_count},
    {"cid",int_type,sizeof(int),0},
    {"name",str_type,64,0},
    {"alias",str_type,64,0},
    {"type",int_type,sizeof(int),0},
    {"streamUrl",str_type,128,0},
    {"streamServerUrl",str_type,128,0},
    {"recordServerUrl",str_type,128,0},
    {"fileServerUrl",str_type,128,0}
};

ParamStru fileInfoStru[fileInfoStruLen] = {
    {"localUri",str_type,128,offset_not_count},
    {"httpUri",str_type,128,0},
    {"downloadUri",str_type,128,0},
    {"size",int_type,sizeof(int),0},
    {"ctime",int_type,sizeof(int),0},
    {"etime",int_type,sizeof(int),0},
    {"type",int_type,sizeof(int),0}
};
ParamStru recordConfigStru[recordConfigStruLen] = {
    {"mid",int_type,sizeof(int),offset_not_count},
    {"cid",int_type,sizeof(int),0},
    {"srcUrl",str_type,128,0},
    {"prefix",str_type,64,0},
    {"type",str_type,16,0},
    {"segment",int_type,sizeof(int),0},
    {"timeServer",str_type,64,0}
};

ParamStru streamConfigStru[streamConfigStruLen] = {
    {"mid",int_type,sizeof(int),offset_not_count},
    {"cid",int_type,sizeof(int),0},
    {"media_codec1_video_width",int_type,sizeof(int),0},
    {"media_codec1_video_height",int_type,sizeof(int),0},
    {"media_codec1_video_bitrate",int_type,sizeof(int),0},
    {"media_codec1_video_maxbitrate",int_type,sizeof(int),0},
    {"media_capture_audio_samplerate",int_type,sizeof(int),0},
    {"media_codec1_audio_bitrate",int_type,sizeof(int),0}
};

ParamType paramTypes[nb_dataType] = { //数组的顺序和DataType定义的顺序务必保持一致
    {int_type,NULL,0},
    {int_array_type,NULL,0},
    {str_type,NULL,0},
    {module_config_type,moduleConfigStru,moduleConfigStruLen},
    {channel_config_type,channelConfigStru,channelConfigStruLen},
    {file_info_type,fileInfoStru,fileInfoStruLen},
    {record_config_type,recordConfigStru,recordConfigStruLen},
    {stream_config_type,streamConfigStru,streamConfigStruLen},
    {file_info_arr_type,NULL,0}
};

void countParamStruOffset(ParamStru* stru,int length){
    if(length > 0 && stru[0].offset == offset_not_count){
        int currentOffset = 0;
        int i = 0;
        for(i=0; i<length; i++){
            stru[i].offset = currentOffset;
            currentOffset += stru[i].size;
        }
    }
}

void initParamStru(){
    int i = 0;
    for(i=0; i<nb_dataType;i++){
        countParamStruOffset(paramTypes[i].stru,paramTypes[i].length);
    }
}
