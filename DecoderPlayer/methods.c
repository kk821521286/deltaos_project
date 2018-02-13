//
//  methods.c
//  soapServer
//
//  Created by shanchun on 16/1/8.
//  Copyright © 2016年 shanchun. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <libini.h>
//#include <sys/wait.h>
#include "methods.h"
//#include "sqlite3Wrapper.h"
//#include "utils.h"

static const char* tm_dir = "/opt/tml";

TMError get_ip(char *ip,int len){
    /*
    char cmd[64];
    snprintf(cmd,64,"%s/get_ip.sh",tm_dir);
    FILE *fp = popen(cmd,"r");
    if(fp == NULL){
        return execError;
    }
    if(topmoo_fgets(fp,ip, len) < 0){
        return IOError;
    }
    if(pclose(fp) != 0){
        return execError;
    }
    */
    return noError;
}
/*
TMError getChannelFileCount(int mid,int cid,int *p_count){
    const char* sql = "select count(fid) from files;";
    char ** db_result;
    int nRow = 0;
    int nColomn = 0;
    int sql_ret = run_sql(sql, 1, &db_result,&nRow,&nColomn);
    if(sql_ret < 0){
        return databaseError;
    }
    *p_count = atoi(db_result[nColomn]);
    free_db_result(db_result);
    return noError;
}

TMError getChannelFileList(int mid, int cid, int *p_count, FileInfo **p_fileList){
    char local_ip[16];
    TMError error = get_ip(local_ip,16);
    if(error != noError){
        return error;
    }
    const char* sql = "select * from files order by end_time DESC;";
    char ** db_result;
    int nRow = 0;
    int nColomn = 0;
    int sql_ret = run_sql(sql, 1, &db_result,&nRow,&nColomn);
    if(sql_ret < 0){
        return databaseError;
    }
    int index = nColomn;
    *p_fileList = (FileInfo *)malloc(nRow * sizeof(FileInfo));
    memset(*p_fileList,0,nRow * sizeof(FileInfo));
    //printf("ip:%s",local_ip);
    for(int i=0;i<nRow;i++){
        //int fid = atoi(db_result[index++]);
        index ++;
        char filename[64];
        memcpy(filename,db_result[index++],64);
        snprintf((*p_fileList)[i].localUri,sizeof((*p_fileList)[i].localUri),"/mnt/hda1/videos/%s",filename);
        snprintf((*p_fileList)[i].httpUri,sizeof((*p_fileList)[i].httpUri),"http://%s/files/videos/%s",local_ip,filename);
        snprintf((*p_fileList)[i].downloadUri,sizeof((*p_fileList)[i].downloadUri),"http://%s/downloads/videos/%s",local_ip,filename);
        (*p_fileList)[i].type = atoi(db_result[index++]);
        (*p_fileList)[i].size = atoi(db_result[index++]);
        (*p_fileList)[i].ctime = atoi(db_result[index++]);
        (*p_fileList)[i].etime = atoi(db_result[index++]);
    }
    *p_count = nRow;
    free_db_result(db_result);
    return noError;
}

int _getCaptureInfo(CaptureInfo *captureInfo){
	FILE *fp = fopen("/var/run/capture_status","r");
	if(fp != NULL){
		if(fscanf(fp,"%d,%d,%d,%d,%d,%d",
		&(captureInfo->width),
		&(captureInfo->height),
		&(captureInfo->fpsNum),
		&(captureInfo->fpsDen),
		&(captureInfo->inter),
		&(captureInfo->lines)) == -1){
			printf("not get capture info\n");
		}
		fclose(fp);
	}else{
		return -1;
	}
	return 0;
}

TMError getChannelLog(int mid, int cid, char *log, int len){
	char cmd[64];
    snprintf(cmd,64,"%s/get_sys_info.sh",tm_dir);
    FILE *fp = popen(cmd,"r");
    if(fp == NULL){
        return execError;
    }
	char sysVerInfo[128] = "";
	char appVerInfo[128] = "";
	char upSecInfo[128] = "";
	char cpuInfo[128] = "";
	char totalMem[16] = "";
	char usedMem[16] = "";
	char freeMem[16] = "";
	topmoo_fgets(fp,sysVerInfo,128);
	topmoo_fgets(fp,appVerInfo,128);
	topmoo_fgets(fp,upSecInfo,128);
	topmoo_fgets(fp,cpuInfo,128);
	topmoo_fgets(fp,totalMem,16);
	topmoo_fgets(fp,usedMem,16);
	topmoo_fgets(fp,freeMem,16);
	int ret = pclose(fp);
    if(ret == -1 || !WIFEXITED(ret)){
        return execError;
    }
	long sec = 0;
	int upDay;
	int upHour;
	int upMin;
	int upSec;
	if(sscanf(upSecInfo,"%ld",&sec) != -1){
		upDay = sec / 86400;
		int secDay = sec % 86400;
		upHour = secDay / 3600;
		int secHour = secDay % 3600;
		upMin = secHour / 60;
		upSec = secHour % 60;
	}
	CaptureInfo captureInfo;
	memset(&captureInfo,0,sizeof(CaptureInfo));
	_getCaptureInfo(&captureInfo);
	int frameRate;
	if(captureInfo.fpsDen != 0){
		frameRate = captureInfo.fpsNum / captureInfo.fpsDen;
	}else{
		frameRate = 0;
	}
	snprintf(log,len,"系统版本:%s,%s\n通道设备已运行%d天%d小时%d分%d秒\nCPU负载:%s\n内存总量:%sKB,已用内存:%sKB,剩余内存:%sKB\n视频采集信息: 宽度:%d,高度:%d,帧率:%d,交错:%d,行数%d\n",
	sysVerInfo,appVerInfo,
	upDay,upHour,upMin,upSec,
	cpuInfo,totalMem,usedMem,freeMem,
	captureInfo.width,captureInfo.height,frameRate,captureInfo.inter,captureInfo.lines
	);
    return noError;	
}

TMError getChannelVersion(int mid, int cid, char *version,int len){
	FILE *fp = fopen("/etc/version","r");
	if(fp == NULL){
		return IOError;
	}
	topmoo_fgets(fp,version,len);	
	fclose(fp);
	return noError;
}

TMError getChannelAppVersion(int mid, int cid, char *version,int len){
	FILE *fp = fopen("/opt/tml/version","r");
	if(fp == NULL){
		return IOError;
	}
	topmoo_fgets(fp,version,len);	
	fclose(fp);
	return noError;
}



TMError getChannelCaptureStatus(int mid, int cid, int *p_status){
	CaptureInfo captureInfo;
	memset(&captureInfo,0,sizeof(CaptureInfo));
	int ret = _getCaptureInfo(&captureInfo);
	if(ret == -1){
		*p_status = 0;
	}else if(captureInfo.width == 0 || captureInfo.height == 0){
		*p_status = 0;
	}else{
		*p_status = 1;
	}
	return noError;
}

TMError getChannelStreamStatus(int mid, int cid, int *p_status){
	FILE *fp = fopen("/var/run/stream_status_0","r");
    *p_status = 0;
    if(fp != NULL){
        char statusStr[16];
        if(topmoo_fgets(fp, statusStr, 16) >= 0){
            *p_status = atoi(statusStr);
        }
    	fclose(fp);
    }
    return noError;
}

TMError getChannelRecordStatus(int mid, int cid, int *p_status){
    char cmd[64];
    *p_status = 0;
    snprintf(cmd,64,"%s/tm_recorder_1 status | grep not",tm_dir);
    FILE *fp = popen(cmd,"r");
    if(fp == NULL){
        return execError;
    }
    char statusStr[32];
    if(topmoo_fgets(fp, statusStr, 32) < 0){
        *p_status = 1;
    }
    int ret = pclose(fp);
    if(ret == -1 || !WIFEXITED(ret)){
        return execError;
    }
    return noError;
}

TMError ctrlChannelRecord(int mid, int cid, int cmd_int){
    char cmd[64];
    if(cmd_int == 1){
        snprintf(cmd, 64, "%s/tm_recorder_1 start",tm_dir);
    }else{
        snprintf(cmd, 64, "%s/tm_recorder_1 stop",tm_dir);
    }
    FILE *fp = popen(cmd, "r");
    if(fp == NULL){
        return execError;
    }
    int ret = pclose(fp);
    if(ret != 0){
        return execError;
    }
    return noError;
}




TMError getChannelStreamConfig(int mid, int cid, StreamConfig *config){
    config->mid = mid;
    config->cid = cid;
    TMError error = channelStreamConfig("r", config);
    return error;
}

TMError setChannelStreamConfig(StreamConfig *config){
    TMError error = channelStreamConfig("a", config);
    return error;
}




TMError getChannelRecordConfig(int mid, int cid, RecordConfig *config){
    TMError error = get_srcUrl(config->srcUrl,sizeof(config->srcUrl));
    if(error != noError){
		printf("%d\n",error);
        return error;
    }
    error = channelRecordConfig("r", config);
    config->mid = mid;
    config->cid = cid;
    return error;
}

TMError setChannelRecordConfig(RecordConfig *config){
    TMError error = channelRecordConfig("a", config);
    return error;
}

TMError deleteChannelFile(int mid, int cid, const char *fileUrl){
    char *filename = strrchr(fileUrl, '/');
    if(filename != NULL){
        filename += 1;
    }else{
        return paramError;
    }
    char sql[128];
    snprintf(sql, 128, "delete from files where filename = '%s';",filename);
    int ret = run_sql(sql, 0, NULL, NULL, NULL);
    if(ret < 0){
        return databaseError;
    }
    ret = remove(fileUrl);
    if(ret < 0){
        return IOError;
    }
    return noError;
}

TMError deleteChannelFileAll(int mid, int cid){
    char sql[128] = "delete from files;";
    int ret = run_sql(sql,0,NULL,NULL,NULL);
    if(ret < 0){
        return databaseError;
    }
    ret = system("rm -rf /mnt/hda1/videos/*.mp4 /mnt/hda1/videos/*.ts");
    if(ret < 0){
        return IOError;
    }
    return noError;
}

TMError tmReboot(){
    int ret = system("/opt/tml/mreboot.sh &> /dev/null &");
    if(ret < 0){
        return IOError;
    }
    return noError;
}
void updateIni(){
	system("/opt/tml/update_ini");
}

TMError tmUpdate(const char *updateFile){
    char cmd[128];
    snprintf(cmd,128,"/opt/tml/upgrade_sys %s",updateFile);
    int ret = system(cmd);
    if(ret < 0){
		char filePath[128];
		snprintf(filePath,128,"/mnt/hda1/tmp/%s",updateFile);
		unlink(filePath);
        return IOError;
    }
	updateIni();
    return noError;
}
TMError checkUpdateStatus(int *p_status){
	FILE *fp = fopen("/var/run/update.status","r");
    *p_status = 0;
    if(fp != NULL){
        char statusStr[16];
        if(topmoo_fgets(fp, statusStr, 16) >= 0){
            *p_status = atoi(statusStr);
        }
    	fclose(fp);
    }
    return noError;
}

TMError checkType(int type){
	if(type != 2){
		return paramError;
	}
	return noError;
}

TMError quickConfig(int action){
	int ret = 0;
	switch(action){
		case 0:
			ret = system("cp /opt/tml/settings/stream.ini.av_audio /opt/tml/settings/stream_0.ini");
		break;
		case 1:
			ret = system("cp /opt/tml/settings/stream.ini.av /opt/tml/settings/stream_0.ini");
		break;
		case 2:
			ret = system("cp /opt/tml/settings/stream.ini.vga_audio /opt/tml/settings/stream_0.ini");
		break;
		case 3:
			ret = system("cp /opt/tml/settings/stream.ini.vga /opt/tml/settings/stream_0.ini");
		break;
		case 4:
			ret = system("cp /opt/tml/settings/stream.ini.audio /opt/tml/settings/stream_0.ini");
		break;
		default:
		break;
	}
	if(ret == 0){
		return noError;
	}else{
		return execError;
	}
}

*/
