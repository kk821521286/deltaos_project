//
//  methods.h
//  soapServer
//
//  Created by shanchun on 16/1/8.
//  Copyright © 2016年 shanchun. All rights reserved.
//

#ifndef methods_h
#define methods_h

#include "TMError.h"
#include "dataStruct.h"
extern TMError getChannelFileCount(int mid, int cid, int* p_count);
extern TMError getChannelFileList(int mid, int cid, int *p_count, FileInfo **fileList);
extern TMError getChannelLog(int mid, int cid, char *log, int len);
extern TMError getChannelVersion(int mid, int cid, char *version, int len);
extern TMError getChannelAppVersion(int mid, int cid, char *version, int len);
extern TMError getChannelCaptureStatus(int mid, int cid, int *p_status);
extern TMError getChannelStreamStatus(int mid, int cid, int *p_status);
extern TMError getChannelRecordStatus(int mid, int cid, int *p_status);
extern TMError ctrlChannelRecord(int mid, int cid, int cmd_int);
extern TMError getChannelStreamConfig(int mid, int cid, StreamConfig *config);
extern TMError setChannelStreamConfig(StreamConfig *config);
extern TMError getChannelRecordConfig(int mid, int cid, RecordConfig *config);
extern TMError setChannelRecordConfig(RecordConfig *config);
extern TMError deleteChannelFile(int mid, int cid, const char *fileUrl);
extern TMError deleteChannelFileAll(int mid, int cid);
extern TMError tmReboot();
extern TMError tmUpdate(const char *updateFile);
extern TMError checkUpdateStatus(int *p_status);
extern TMError checkType(int type);
extern TMError quickConfig(int action);

#endif /* methods_h */
