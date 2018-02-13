//
//  dataStruct.h
//  soapServer
//
//  Created by shanchun on 16/1/7.
//  Copyright © 2016年 shanchun. All rights reserved.
//
#ifndef dataStruct_h
#define dataStruct_h
#define offset_not_count -1

#define nb_dataType 9

typedef struct {
	int width;
	int height;
	int fpsNum;
	int fpsDen;
	int inter;
	int lines;
}CaptureInfo;

typedef enum{
    int_type = 0,
    int_array_type,
    str_type,
    module_config_type,
    channel_config_type,
    file_info_type,
    record_config_type,
    stream_config_type,
    file_info_arr_type,
    longlong_type,
    data_null
}DataType;

typedef struct paramStructure{
    char name[64];
    DataType type;
    int  size;
    int  offset; //-1表示offset还未经过初始化计算
}ParamStru;

typedef struct{
    int mid;
    char name[64];
    char alias[64];
    char ip[32];
    int  cid_sequence[256];
}ModuleConfig;

typedef struct{
    int mid;
    int cid;
    char name[64];
    char alias[64];
    char ip[32];
    int type;
    char streamUrl[128];
    char streamServerUrl[128];
    char recordServerUrl[128];
    char fileServerUrl[128];
}ChannelConfig;

//file information struct
typedef struct{
    char localUri[128];
    char httpUri[128];
    char downloadUri[128];
    int size;
    int ctime;
    int etime;
    int type;
}FileInfo;

//record config struct
typedef struct{
    int mid;
    int cid;
    char srcUrl[128];
    char prefix[64];
    char type[16];
    int segment;
    char timeServer[64];
}RecordConfig;

//stream config struct
typedef struct{
    int mid;
    int cid;
    int media_codec1_video_width;
    int media_codec1_video_height;
    int media_codec1_video_bitrate;
    int media_codec1_video_maxbitrate;
    int media_capture_audio_samplerate;
    int media_codec1_audio_bitrate;
}StreamConfig;

//error struct
typedef struct{
    int result;
    char info[128];
}ErrorInfo;

typedef struct{
    DataType type;
    ParamStru *stru;
    int length;
}ParamType;
extern ParamType paramTypes[nb_dataType];

extern void initParamStru();

#endif /* dataStruct_h */
