#include "mcrclientimpl.h"
#include "soapPackBinding.nsmap"
#include"cJSON.h"
#include"soapH.h"
#include"soapStub.h"
#include"stdsoap2.h"
#include<QString>
#include <stdio.h>

#include<QDebug>

#define WEBSERVICE_TIMEOUT 5

MCRClientImpl::MCRClientImpl()
{

}

bool setSocketKeepAlive(struct soap* s,int timeout)
{
    s->send_timeout = timeout;
    s->recv_timeout = timeout;
    s->connect_timeout = timeout;
#if 0
    int keepAlive = 1;
    int nRet ;
    SOCKET sock;

    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock == INVALID_SOCKET)
    {
        qWarning("create socket failed:%d\n",WSAGetLastError());
        return false;
    }
    nRet= setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE,
                     (char*)&keepAlive, sizeof(keepAlive));
    if (nRet == SOCKET_ERROR)
    {
        qWarning("setsockopt failed: %d/n", WSAGetLastError());
        return false;
    }
    // set KeepAlive parameter

    tcp_keepalive alive_in;
    tcp_keepalive alive_out;
    alive_in.keepalivetime  = timeout * 10;
    alive_in.keepaliveinterval  = 10; //当keepalivetime时间到达后，每隔多长时间(毫秒)发送一个心跳包，发5次(系统默认值)
    alive_in.onoff              = 1;
    unsigned long ulBytesReturn = 0;

    nRet = WSAIoctl(sock, SIO_KEEPALIVE_VALS, &alive_in, sizeof(alive_in),
                    &alive_out, sizeof(alive_out), &ulBytesReturn, NULL, NULL);
    if (nRet == SOCKET_ERROR)
    {
        qWarning("WSAIoctl failed: %d/n", WSAGetLastError());
        return false;
    }
#endif
    return true;

}

//function 1

void MCRClientImpl::getModuleCount(QVector<int> &array,const char* url,ErrorInfo &e)
{

    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }
    struct soap* s = soap_new();
    std::string run = "" ,data = "" ;
    char *tmp1 = NULL ;



   // qWarning("url = %s\n",url);

    cJSON * request = NULL ,*response = NULL, *count = NULL,*result = NULL,
                    *errorInfo = NULL,*error = NULL;

    cJSON *params = cJSON_CreateArray();
    cJSON *array_recv = cJSON_CreateArray();

    cJSON *cstr = cJSON_CreateString("");


    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "getModuleCount");
    cJSON_AddItemToArray(params,cstr);
    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;

    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
       // qDebug("run --function debug\n");
        qDebug("run function debug\n");
        //qDebug("---%s",run.c_str());
        int i,size;
        response = cJSON_Parse(run.c_str());//change string into json object
//        tmp1 = cJSON_Print(response);//change json object into string

       // qDebug("data--:%s\n",tmp1);
//        if(tmp1){
//            free(tmp1);
//            tmp1 = NULL;
//        }
        result = cJSON_GetObjectItem(response,"result");


        count = cJSON_GetObjectItem(result,"count");

        size = cJSON_GetArraySize(count);

       // qDebug("size is %d",size);
        for(i=0;i<size;i++) {
            array_recv = cJSON_GetArrayItem(count,i);
            if(array_recv->type == cJSON_String) {
             array.append(atoi(array_recv->valuestring));
            }else if (array_recv->type == cJSON_Number){
                array.append(array_recv->valueint);
            }

        }
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        error = cJSON_GetObjectItem(response,"error");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

    }else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return;
}

//function 2

void MCRClientImpl::setModuleCount(int Count,const char* url,ErrorInfo &e)//后续功能
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap* s = soap_new();
    std::string run ,data ;
    char* tmp1 = NULL;



    cJSON * request = NULL ,*response = NULL,*result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();


    cJSON *count = cJSON_CreateNumber(Count);

    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "setModuleCount");
    cJSON_AddItemToArray(params,count);
    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;

    cJSON_Delete(request);

    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
       // qDebug("in run function\n");
        response = cJSON_Parse(run.c_str());
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        error = cJSON_GetObjectItem(response,"error");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}


//function 3

void MCRClientImpl::getModuleConfig(ModuleConfig &mConf,int Mid,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }
    struct soap  *s =  soap_new();
    std::string run ;
    char* data = NULL;

    cJSON * request = NULL ,*response = NULL, *config = NULL,*result = NULL,
                    *error = NULL,*errorInfo = NULL;
    cJSON *mid = NULL,*name = NULL,*alias = NULL,*ip = NULL;
    cJSON *params = cJSON_CreateArray();
  //  cJSON *cidArray = cJSON_CreateArray();
   // cJSON *cid_recv = NULL;
    cJSON *num = cJSON_CreateNumber(Mid);

    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "getModuleConfig");
    cJSON_AddItemToArray(params,num);
    cJSON_AddItemToObject(request, "params", params);
    data = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",data);
    cJSON_Delete(request);

    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {

       // qWarning("in run function %s\n",run.c_str());
        //int i,size;
        response = cJSON_Parse(run.c_str());//change string into json object


        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        error = cJSON_GetObjectItem(response,"error");
        config = cJSON_GetObjectItem(result,"config");

        //recv a moduleConfig data struct from cJSON object

        mid = cJSON_GetObjectItem(config,"mid");
        name = cJSON_GetObjectItem(config,"name");
        alias = cJSON_GetObjectItem(config,"alias");
        ip = cJSON_GetObjectItem(config,"ip");

      //  cidArray = cJSON_GetObjectItem(config,"channelCount");
      //  size = cJSON_GetArraySize(cidArray);
//        for(i=0;i<size;i++){
//            cid_recv = cJSON_GetArrayItem(cidArray,i);
//            if(cid_recv->type==cJSON_String){
//                mConf.cidArray.append(atoi(cid_recv->valuestring));
//            }else if(cid_recv->type==cJSON_Number){
//                mConf.cidArray.append(cid_recv->valueint);
//            }
//        }
        if(mid->type==cJSON_String){
            mConf.mid = atoi(mid->valuestring);
        }else if(mid->type==cJSON_Number){
            mConf.mid = mid->valueint;
        }

        //mConf.mid = mid->valueint;
        mConf.name = (const char*)QByteArray::fromPercentEncoding(name->valuestring);
        mConf.alias = (const char*)QByteArray::fromPercentEncoding(alias->valuestring);

        //mConf.alias = alias->valuestring;
        mConf.ip = ip->valuestring;

        //mConf.channelCount = channelCount->valueint;

        e.result = error->valueint;
        e.info = errorInfo->valuestring;


    }else{
        e.result = -256;
        e.info = "webservice failed!";
    }

    if(response){
        cJSON_Delete(response);
    }
    if(data){
        free(data);
        data = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;

}

//function 4

void MCRClientImpl::setModuleConfig(ModuleConfig * Config,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap* s = soap_new();
    std::string run ,data ;
    char* tmp1 = NULL;

    cJSON * request = NULL ,*response = NULL,*result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();
   // cJSON *cidArray = cJSON_CreateArray();
    cJSON *config = cJSON_CreateObject();

    cJSON *mid = cJSON_CreateNumber(Config->mid);
    cJSON_AddItemToObject(config,"mid",mid);
    QString nameStr = Config->name.c_str();
    cJSON *name = cJSON_CreateString(nameStr.toLatin1().toPercentEncoding());
    cJSON_AddItemToObject(config,"name",name);
    QString aliasStr = Config->alias.c_str();
    cJSON *alias = cJSON_CreateString(aliasStr.toLatin1().toPercentEncoding());

   // cJSON *alias = cJSON_CreateString(Config->alias.c_str());
    cJSON_AddItemToObject(config,"alias",alias);
    cJSON *ip = cJSON_CreateString(Config->ip.c_str());
    cJSON_AddItemToObject(config,"ip",ip);
   // int i;

//    for(i=0;i<Config->cidArray.size();i++) {
//        cJSON_AddItemToArray(cidArray, cJSON_CreateNumber(Config->cidArray[i]));
//    }

//    cJSON_AddItemToObject(config,"channelCount",cidArray);
    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "setModuleConfig");

    cJSON_AddItemToArray(params,config);
    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;

    cJSON_Delete(request);

    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
       // qDebug("in run function\n");
        response = cJSON_Parse(run.c_str());
        result = cJSON_GetObjectItem(response,"result");
        error = cJSON_GetObjectItem(response,"error");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;


    }
    else{
        e.result = -256;
        e.info = "webservice failed!";
    }

    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;

}

//function 5
void MCRClientImpl::getChannelConfig(ChannelConfig &cConf,int Mid,int Cid,const char* url,ErrorInfo &e)
{

    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap  *s =  soap_new();

    std::string run ;
    char* data = NULL;

    cJSON * request = NULL ,*response = NULL, *result = NULL,*config = NULL,
                    *error = NULL,*errorInfo = NULL;
    cJSON *name = NULL,*alias = NULL,*ip = NULL,*type = NULL,*streamUrl = NULL ,\
                    *streamServerUrl = NULL,*recordServerUrl = NULL,*fileServerUrl = NULL;
    cJSON *params = cJSON_CreateArray();


    cJSON *mid = cJSON_CreateNumber(Mid);
    cJSON *cid = cJSON_CreateNumber(Cid);

    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "getChannelConfig");
    cJSON_AddItemToArray(params,mid);
    cJSON_AddItemToArray(params,cid);
    cJSON_AddItemToObject(request, "params", params);
    char* tmp1 = cJSON_Print(request);//change json object into string
    data = tmp1;

    qWarning("data:%s\n",data);
    cJSON_Delete(request);

    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
        if(tmp1){
            free(tmp1);
            tmp1 = NULL;
        }
       // qWarning("\nChannelConfig=%s\n\n",run.c_str());
        response = cJSON_Parse(run.c_str());//change string into json object
        result = cJSON_GetObjectItem(response,"result");
        config = cJSON_GetObjectItem(result,"config");
        error = cJSON_GetObjectItem(response,"error");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;
        //recv a moduleConfig data struct from cJSON object
        name = cJSON_GetObjectItem(config,"Name");
        alias = cJSON_GetObjectItem(config,"Alias");
        ip = cJSON_GetObjectItem(config,"Ip");
        type = cJSON_GetObjectItem(config,"Type");
        streamUrl = cJSON_GetObjectItem(config,"StreamUrl");
        streamServerUrl = cJSON_GetObjectItem(config,"StreamServerUrl");
        recordServerUrl = cJSON_GetObjectItem(config,"RecordServerUrl");
        fileServerUrl = cJSON_GetObjectItem(config,"FileServerUrl");
        QString nameStr = name->valuestring;
        cConf.name = (const char*)QByteArray::fromPercentEncoding(nameStr.toLatin1());
        QString aliasStr = alias->valuestring;
        cConf.alias = (const char*)QByteArray::fromPercentEncoding(aliasStr.toLatin1());
        cConf.ip = ip->valuestring;
        if(type->type==cJSON_String){
            cConf.type = atoi(type->valuestring);
        }else if(type->type==cJSON_Number){
            cConf.type = type->valueint;
        }
        cConf.mid = Mid;
        cConf.cid = Cid;
        cConf.streamUrl = streamUrl->valuestring;
        cConf.streamServerUrl = streamServerUrl->valuestring;
        cConf.recordServerUrl = recordServerUrl->valuestring;
        cConf.fileServerUrl = fileServerUrl->valuestring;

    }
    else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}


//function 6
void MCRClientImpl::setChannelConfig(ChannelConfig *Config,const char* url,ErrorInfo &e)
{
#ifndef USE_QJSON
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }


    struct soap* s = soap_new();
    std::string run ,data ;
    char* tmp1 = NULL;

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();
    cJSON *config = cJSON_CreateObject();

    cJSON *mid = cJSON_CreateNumber(Config->mid);
    cJSON_AddItemToObject(config,"mid",mid);
    cJSON *cid = cJSON_CreateNumber(Config->cid);
    cJSON_AddItemToObject(config,"cid",cid);
    QString nameStr = Config->name.c_str();
    cJSON *name = cJSON_CreateString(nameStr.toLatin1().toPercentEncoding());
    cJSON_AddItemToObject(config,"name",name);
    QString aliasStr = Config->alias.c_str();
    cJSON *alias = cJSON_CreateString(aliasStr.toLatin1().toPercentEncoding());
    cJSON_AddItemToObject(config,"alias",alias);
    cJSON *ip = cJSON_CreateString(Config->ip.c_str());
    cJSON_AddItemToObject(config,"ip",ip);
    cJSON *type = cJSON_CreateNumber(Config->type);
    cJSON_AddItemToObject(config,"type",type);
    cJSON *streamUrl = cJSON_CreateString(Config->streamUrl.c_str());
    cJSON_AddItemToObject(config,"streamUrl",streamUrl);
    cJSON *streamServerUrl = cJSON_CreateString(Config->streamServerUrl.c_str());
    cJSON_AddItemToObject(config,"streamServerUrl",streamServerUrl);
    cJSON *recordServerUrl = cJSON_CreateString(Config->recordServerUrl.c_str());
    cJSON_AddItemToObject(config,"recordServerUrl",recordServerUrl);
    cJSON *fileServerUrl = cJSON_CreateString(Config->fileServerUrl.c_str());
    cJSON_AddItemToObject(config,"fileServerUrl",fileServerUrl);
    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "setChannelConfig");

    cJSON_AddItemToArray(params,config);

    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
   // qWarning("SetChannelConfig:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);

    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
        response = cJSON_Parse(run.c_str());
        error = cJSON_GetObjectItem(response,"error");
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;
    }
    else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
#else

#endif
    return;
}

//function 7
void MCRClientImpl::getChannelFileCount(int &fCount,int Mid,int Cid,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap  *s =  soap_new();

    std::string run,data ;
    char *tmp1 = NULL;

    cJSON * request = NULL ,*response = NULL, *result = NULL,*count = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();


    cJSON *mid = cJSON_CreateNumber(Mid);
    cJSON *cid = cJSON_CreateNumber(Cid);

    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "getChannelFileCount");
    cJSON_AddItemToArray(params,mid);
    cJSON_AddItemToArray(params,cid);
    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;

    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
        //qDebug("in run function\n");
        response = cJSON_Parse(run.c_str());//change string into json object

        error = cJSON_GetObjectItem(response,"error");
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

        count = cJSON_GetObjectItem(result,"count");
        fCount = count->valueint;
    }else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}

//function 8
void MCRClientImpl::getChannelFileList(int Mid,int Cid,vector<FileInfo> &list,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap  *s =  soap_new();
    int num ;
    std::string run ;
    char* data = NULL,*tmp1 = NULL;
    FileInfo fi ;

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;
    cJSON *fileList = NULL,*localUrl = NULL,*httpUrl = NULL,*downloadUrl = NULL, *size = NULL,*ctime = NULL,
            *etime = NULL,*type = NULL ;
    cJSON *fileListItem = NULL;
    cJSON *params = cJSON_CreateArray();


    cJSON *mid = cJSON_CreateNumber(Mid);
    cJSON *cid = cJSON_CreateNumber(Cid);

    request = cJSON_CreateObject();


    cJSON_AddStringToObject(request, "function", "getChannelFileList");
    cJSON_AddItemToArray(params,mid);
    cJSON_AddItemToArray(params,cid);
    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT*10);
    s->connect_timeout = WEBSERVICE_TIMEOUT*10;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
        qDebug("in run function\n");
        response = cJSON_Parse(run.c_str());//change string into json object
        result = cJSON_GetObjectItem(response,"result");
        error = cJSON_GetObjectItem(response,"error");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

        fileList = cJSON_GetObjectItem(result,"fileList");
        int arraySize = 0;
        arraySize = cJSON_GetArraySize(fileList);
        num = 0;
        //qWarning("arrarSize is %d\n",arraySize);
        while(arraySize != 0) {
            fileListItem = cJSON_GetArrayItem(fileList,num);

            localUrl = cJSON_GetObjectItem(fileListItem,"localUri");
          //  qWarning("localUrl = %s\n",localUrl->valuestring);

            httpUrl = cJSON_GetObjectItem(fileListItem,"httpUri");
            downloadUrl = cJSON_GetObjectItem(fileListItem,"downloadUri");
            size = cJSON_GetObjectItem(fileListItem,"size");
            etime = cJSON_GetObjectItem(fileListItem,"etime");
            ctime = cJSON_GetObjectItem(fileListItem,"ctime");
            type = cJSON_GetObjectItem(fileListItem,"type");

            fi.localUrl = localUrl->valuestring;
            fi.httpURL = httpUrl->valuestring;
            fi.downloadUrl = downloadUrl->valuestring;
            if(size->type==cJSON_String){
                fi.size = atoi(size->valuestring);
            }else if(size->type==cJSON_Number){
                fi.size = size->valueint;
            }
            if(ctime->type==cJSON_String){
                fi.cTime = atoi(ctime->valuestring);
            }else if(ctime->type==cJSON_Number){
                fi.cTime = ctime->valueint;
            }
            if(etime){
                if(etime->type==cJSON_String){
                     fi.eTime = atoi(etime->valuestring);
                }else if(etime->type==cJSON_Number){
                     fi.eTime = etime->valueint;
                }
            }else{
                fi.eTime = -1;
            }
            if(type->type==cJSON_String){
                fi.type = atoi(type->valuestring);
            }else if(type->type==cJSON_Number){
                fi.type = type->valueint;
            }
            //fi.size = size->valueint;
            //fi.cTime = ctime->valueint;
            //fi.type = type->valueint;
            list.push_back(fi);
            num++;
            arraySize--;

          //  qDebug("in while ()\n");
        }

        qDebug("out while ()\n");
    }else{
        e.result = -256;
        e.info = "webservice failed!";
    }

    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return  ;
}

//function 9
void MCRClientImpl::getChannelStreamStatus(int &Status,int Mid,int Cid,const char* url,ErrorInfo &e) //return status 0(no signal) or 1(have signal)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap  *s =  soap_new();

    std::string run ,data;
    char *tmp1 = NULL; 

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;
    cJSON *status = NULL;
    cJSON *params = cJSON_CreateArray();


    cJSON *mid = cJSON_CreateNumber(Mid);
    cJSON *cid = cJSON_CreateNumber(Cid);

    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "getChannelStreamStatus");
    cJSON_AddItemToArray(params,mid);
    cJSON_AddItemToArray(params,cid);
    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;

    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    //s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
       // qDebug("in run function %s\n",run.c_str());
        response = cJSON_Parse(run.c_str());//change string into json object
        error = cJSON_GetObjectItem(response,"error");
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

        status = cJSON_GetObjectItem(result,"status");
        //Status = status->valueint;
        if(status->type==cJSON_String){
            Status = atoi(status->valuestring);
        }else if(status->type==cJSON_Number){
            Status = status->valueint;
        }
    }else{
        Status = -1;
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return;
}
//function 10
void MCRClientImpl::getChannelRecordStatus(int &Status,int Mid,int Cid,const char* url,ErrorInfo &e)//return status 0(未录制)1(录制进行中)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap  *s =  soap_new();

    std::string run,data ;
    char *tmp1 = NULL;

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;
    cJSON *status = NULL;
    cJSON *params = cJSON_CreateArray();


    cJSON *mid = cJSON_CreateNumber(Mid);
    cJSON *cid = cJSON_CreateNumber(Cid);

    request = cJSON_CreateObject();


    cJSON_AddStringToObject(request, "function", "getChannelRecordStatus");
    cJSON_AddItemToArray(params,mid);
    cJSON_AddItemToArray(params,cid);
    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    //s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
       // qDebug("in record status run function %s %s \n",run.c_str(),url);
        response = cJSON_Parse(run.c_str());//change string into json object
        error = cJSON_GetObjectItem(response,"error");
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

        status = cJSON_GetObjectItem(result,"status");
        //Status = status->valueint;
        if(status->type==cJSON_String){
            Status = atoi(status->valuestring);
        }else if(status->type==cJSON_Number){
            Status = status->valueint;
        }
    }else{
        Status = -1;
        e.result = -256;
        e.info = "webservice failed!";
    }

    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}

//function 11
void MCRClientImpl::ctrlChannelRecord(int Mid,int Cid,int Cmd,const char* url,ErrorInfo &e)
{

    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap* s = soap_new();
    std::string run ,data ;
    char* tmp1 = NULL; 

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();


    cJSON *mid = cJSON_CreateNumber(Mid);
    cJSON *cid = cJSON_CreateNumber(Cid);
    cJSON *cmd = cJSON_CreateNumber(Cmd);

    request = cJSON_CreateObject();


    cJSON_AddStringToObject(request, "function", "ctrlChannelRecord");
    cJSON_AddItemToArray(params,mid);
    cJSON_AddItemToArray(params,cid);
    cJSON_AddItemToArray(params,cmd);

    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data.insert(0,tmp1);
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
        response = cJSON_Parse(run.c_str());
        error = cJSON_GetObjectItem(response,"error");
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

    }
    else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}
//function 12
void MCRClientImpl::getChannelStreamConfig(StreamConfig &sConf,int Mid,int Cid,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap* s = soap_new();
    std::string run ,data ;
    char* tmp1 = NULL; 

    cJSON * request = NULL ,*response = NULL, *result = NULL,   \
                    *config = NULL,*width = NULL,*height = NULL ,\
                    *maxbitrate = NULL,*samplerate = NULL,*audio_bitrate = NULL;
    cJSON *v_bitrate = NULL;
    cJSON *params = cJSON_CreateArray();
    cJSON *error = NULL,*errorInfo = NULL;

    cJSON *mid = cJSON_CreateNumber(Mid);
    cJSON *cid = cJSON_CreateNumber(Cid);


    request = cJSON_CreateObject();


    cJSON_AddStringToObject(request, "function", "getChannelStreamConfig");
    cJSON_AddItemToArray(params,mid);
    cJSON_AddItemToArray(params,cid);


    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data.insert(0,tmp1);
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
        response = cJSON_Parse(run.c_str());
        result = cJSON_GetObjectItem(response,"result");
        config = cJSON_GetObjectItem(result,"config");
        error = cJSON_GetObjectItem(response,"error");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

        mid = cJSON_GetObjectItem(config,"mid");
        cid = cJSON_GetObjectItem(config,"cid");
        width = cJSON_GetObjectItem(config,"media_codec1_video_width");
        height = cJSON_GetObjectItem(config,"media_codec1_video_height");
        v_bitrate = cJSON_GetObjectItem(config,"media_codec1_video_bitrate");
        maxbitrate = cJSON_GetObjectItem(config,"media_codec1_video_maxbitrate");
        samplerate = cJSON_GetObjectItem(config,"media_capture_audio_samplerate");
        audio_bitrate = cJSON_GetObjectItem(config,"media_codec1_audio_bitrate");

        if(mid->type==cJSON_String){
            sConf.mid = atoi(mid->valuestring);
        }else if(mid->type==cJSON_Number){
            sConf.mid = mid->valueint;
        }
        if(cid->type==cJSON_String){
            sConf.cid = atoi(cid->valuestring);
        }else if(cid->type==cJSON_Number){
            sConf.cid = cid->valueint;
        }
        if(width->type==cJSON_String){
            sConf.media_codec1_video_width = atoi(width->valuestring);
        }else if(width->type==cJSON_Number){
            sConf.media_codec1_video_width = width->valueint;
        }
        if(height->type==cJSON_String){
            sConf.media_codec1_video_height = atoi(height->valuestring);
        }else if(height->type==cJSON_Number){
            sConf.media_codec1_video_height = height->valueint;
        }
        if(v_bitrate->type==cJSON_String){
            sConf.media_codec1_video_bitrate = atoi(v_bitrate->valuestring);
        }else if(v_bitrate->type==cJSON_Number){
            sConf.media_codec1_video_bitrate = v_bitrate->valueint;
        }
        if(maxbitrate->type==cJSON_String){
            sConf.media_codec1_video_maxbitrate = atoi(maxbitrate->valuestring);
        }else if(maxbitrate->type==cJSON_Number){
            sConf.media_codec1_video_maxbitrate = maxbitrate->valueint;
        }
        if(samplerate != NULL && samplerate->type==cJSON_String  ){
            sConf.media_capture_audio_samplerate = atoi(samplerate->valuestring);
        }else if(samplerate != NULL &&samplerate->type==cJSON_Number){
            sConf.media_capture_audio_samplerate = samplerate->valueint;
        }
        if(audio_bitrate != NULL && audio_bitrate->type==cJSON_String){
            sConf.media_codec1_audio_bitrate = atoi(audio_bitrate->valuestring);
        }else if(audio_bitrate != NULL && audio_bitrate->type==cJSON_Number){
            sConf.media_codec1_audio_bitrate = audio_bitrate->valueint;
        }

    }else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}

//function 13
void MCRClientImpl::setChannelStreamConfig(StreamConfig *Config,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap* s = soap_new();
    std::string run ,data ;
    char* tmp1 = NULL;

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();
    cJSON *config = cJSON_CreateObject();


    cJSON *mid = cJSON_CreateNumber(Config->mid);
    cJSON_AddItemToObject(config,"mid",mid);
    cJSON *cid = cJSON_CreateNumber(Config->cid);
    cJSON_AddItemToObject(config,"cid",cid);
    cJSON *width = cJSON_CreateNumber(Config->media_codec1_video_width);
    cJSON_AddItemToObject(config,"media_codec1_video_width",width);
    cJSON *height = cJSON_CreateNumber(Config->media_codec1_video_height);
    cJSON_AddItemToObject(config,"media_codec1_video_height",height);
    cJSON *v_bitrate = cJSON_CreateNumber(Config->media_codec1_video_bitrate);
    cJSON_AddItemToObject(config,"media_codec1_video_bitrate",v_bitrate);
    cJSON *maxbitrate = cJSON_CreateNumber(Config->media_codec1_video_maxbitrate);
    cJSON_AddItemToObject(config,"media_codec1_video_maxbitrate",maxbitrate);
    cJSON *samplerate = cJSON_CreateNumber(Config->media_capture_audio_samplerate);
    cJSON_AddItemToObject(config,"media_capture_audio_samplerate",samplerate);
    cJSON *audio_bitrate = cJSON_CreateNumber(Config->media_codec1_audio_bitrate);
    cJSON_AddItemToObject(config,"media_codec1_audio_bitrate",audio_bitrate);
    request = cJSON_CreateObject();


    cJSON_AddStringToObject(request, "function", "setChannelStreamConfig");
    cJSON_AddItemToArray(params,config);


    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
        response = cJSON_Parse(run.c_str());
        error = cJSON_GetObjectItem(response,"error");
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

    }
    else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}

//function 14
void MCRClientImpl::getChannelRecordConfig(RecordConfig &rConf,int Mid,int Cid,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap  *s =  soap_new();

    std::string run ,data;
    char *tmp1 = NULL;  

    cJSON * request = NULL ,*response = NULL, *result = NULL,*config = NULL,\
                    *error = NULL,*errorInfo = NULL,*scrUrl = NULL,*prefix = NULL,*type = NULL, \
            *segment = NULL ;

    cJSON *params = cJSON_CreateArray();


    cJSON *mid = cJSON_CreateNumber(Mid);
    cJSON *cid = cJSON_CreateNumber(Cid);

    request = cJSON_CreateObject();


    cJSON_AddStringToObject(request, "function", "getChannelRecordConfig");
    cJSON_AddItemToArray(params,mid);
    cJSON_AddItemToArray(params,cid);
    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
      //  qDebug("in run function\n");
        response = cJSON_Parse(run.c_str());//change string into json object
        result = cJSON_GetObjectItem(response,"result");
        config = cJSON_GetObjectItem(result,"config");

        error = cJSON_GetObjectItem(response,"error");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

        mid = cJSON_GetObjectItem(config,"mid");
        cid = cJSON_GetObjectItem(config,"cid");
        scrUrl = cJSON_GetObjectItem(config,"srcUrl");
      //  qWarning("scrUrl = %s\n",scrUrl->valuestring);
        prefix = cJSON_GetObjectItem(config,"prefix");
        type = cJSON_GetObjectItem(config,"type");
        segment = cJSON_GetObjectItem(config,"segment");
       // timeServer = cJSON_GetObjectItem(config,"timeServer");

        if(mid->type == cJSON_String) {
            rConf.mid = atoi(mid->valuestring);
        }else if (mid->type == cJSON_Number) {
            rConf.mid = mid->valueint;
        }
        if(cid->type == cJSON_String) {
            rConf.cid = atoi(cid->valuestring);
        }else if (cid->type == cJSON_Number) {
            rConf.cid = cid->valueint;
        }
        rConf.srcUrl = scrUrl->valuestring;
        rConf.prefix = prefix->valuestring;
        rConf.type = type->valuestring;
        //rConf.timeServer = timeServer->valuestring;
        rConf.timeServer = "";
        if(segment != NULL && segment->type == cJSON_String) {
            rConf.segment = atoi(segment->valuestring);
        }else if (segment != NULL && segment->type == cJSON_Number) {
            rConf.segment = segment->valueint;
        }


    }else{
        e.result = -256;
        e.info = "webservice failed!";
    }

    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}

//function 15
void MCRClientImpl::setChannelRecordConfig(RecordConfig *Config,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap* s = soap_new();
    std::string run ,data ;
    char* tmp1 = NULL;

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;
    //record config struct item variable
    cJSON *mid = NULL,*cid = NULL,*srcUrl = NULL,*prefix = NULL, \
            *type = NULL ,*segment = NULL,*timeServer = NULL;

    cJSON *params = cJSON_CreateArray();
    cJSON *config = cJSON_CreateObject();

    mid = cJSON_CreateNumber(Config->mid);
    cJSON_AddItemToObject(config,"mid",mid);
    cid = cJSON_CreateNumber(Config->cid);
    cJSON_AddItemToObject(config,"cid",cid);
    srcUrl = cJSON_CreateString(Config->srcUrl.c_str());
    cJSON_AddItemToObject(config,"srcUrl",srcUrl);
    prefix = cJSON_CreateString(Config->prefix.c_str());
    cJSON_AddItemToObject(config,"prefix",prefix);
    type = cJSON_CreateString(Config->type.c_str());
    cJSON_AddItemToObject(config,"type",type);
    segment = cJSON_CreateNumber(Config->segment);
    cJSON_AddItemToObject(config,"segment",segment);
    timeServer = cJSON_CreateString(Config->timeServer.c_str());
    cJSON_AddItemToObject(config,"timeServer",timeServer);

    request = cJSON_CreateObject();


    cJSON_AddStringToObject(request, "function", "setChannelRecordConfig");
    cJSON_AddItemToArray(params,config);


    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
        response = cJSON_Parse(run.c_str());
        error = cJSON_GetObjectItem(response,"error");
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

    }else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}

//function 16
void MCRClientImpl::deleteChannelFile(int Mid,int Cid,std::string FileUrl,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap* s = soap_new();
    std::string run ,data ;
    char* tmp1 = NULL;

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();


    cJSON *mid = cJSON_CreateNumber(Mid);
    cJSON *cid = cJSON_CreateNumber(Cid);
    cJSON *fileUrl = cJSON_CreateString(FileUrl.c_str());

    request = cJSON_CreateObject();


    cJSON_AddStringToObject(request, "function", "deleteChannelFile");
    cJSON_AddItemToArray(params,mid);
    cJSON_AddItemToArray(params,cid);
    cJSON_AddItemToArray(params,fileUrl);


    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    s->send_timeout = WEBSERVICE_TIMEOUT;
    s->recv_timeout = WEBSERVICE_TIMEOUT;

    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
        response = cJSON_Parse(run.c_str());
        error = cJSON_GetObjectItem(response,"error");
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;
    }
    else{
        e.result = -256;
        e.info = "webservice failed!";
        qDebug("######webservice failed!\n");
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}
//function 17
void MCRClientImpl::deleteChannelFileAll(int Mid,int Cid,const char* url,ErrorInfo &e)
{

    if(url == NULL) {
        qWarning("get url error\n");
        return ;
    }

    struct soap* s = soap_new();
    std::string run ,data ;
    char* tmp1 = NULL;

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();


    cJSON *mid = cJSON_CreateNumber(Mid);
    cJSON *cid = cJSON_CreateNumber(Cid);

    request = cJSON_CreateObject();


    cJSON_AddStringToObject(request, "function", "deleteChannelFileAll");
    cJSON_AddItemToArray(params,mid);
    cJSON_AddItemToArray(params,cid);



    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT*30);
    s->connect_timeout = 30*WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
        response = cJSON_Parse(run.c_str());

        error = cJSON_GetObjectItem(response,"error");
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;
    }
    else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;

}
//function 18
void MCRClientImpl::deleteChannel(int Mid,int Cid,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return ;
    }

    struct soap* s = soap_new();
    std::string run ,data ;
    char* tmp1 = NULL;  

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();


    cJSON *mid = cJSON_CreateNumber(Mid);
    cJSON *cid = cJSON_CreateNumber(Cid);

    request = cJSON_CreateObject();


    cJSON_AddStringToObject(request, "function", "deleteChannel");
    cJSON_AddItemToArray(params,mid);
    cJSON_AddItemToArray(params,cid);



    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
        response = cJSON_Parse(run.c_str());

        error = cJSON_GetObjectItem(response,"error");
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;
    }
    else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}
//function 19
void MCRClientImpl::deleteModule(int Mid,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return ;
    }

    struct soap* s = soap_new();
    std::string run ,data ;
    char* tmp1 = NULL;

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();


    cJSON *mid = cJSON_CreateNumber(Mid);


    request = cJSON_CreateObject();


    cJSON_AddStringToObject(request, "function", "deleteModule");
    cJSON_AddItemToArray(params,mid);

    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT*30);
    s->connect_timeout = 30*WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
        response = cJSON_Parse(run.c_str());

        error = cJSON_GetObjectItem(response,"error");
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;
    }
    else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}
//function 20

void MCRClientImpl::addModule(ModuleConfig *Config,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap  *s =  soap_new();

    std::string run ;
    char* data = NULL; 

    cJSON * request = NULL ,*response = NULL,*result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();
   // cJSON *cidArray = cJSON_CreateArray();
    cJSON *config = cJSON_CreateObject();

    cJSON *mid = cJSON_CreateNumber(Config->mid);
    cJSON_AddItemToObject(config,"mid",mid);
    QString nameStr = Config->name.c_str();
    cJSON *name = cJSON_CreateString(nameStr.toLatin1().toPercentEncoding());
    cJSON_AddItemToObject(config,"name",name);
    QString aliasStr = Config->alias.c_str();
    cJSON *alias = cJSON_CreateString(aliasStr.toLatin1().toPercentEncoding());
    cJSON_AddItemToObject(config,"alias",alias);
    cJSON *ip = cJSON_CreateString(Config->ip.c_str());
    cJSON_AddItemToObject(config,"ip",ip);
   // int i;

//    for(i=0;i<Config->cidArray.size();i++) {
//        cJSON_AddItemToArray(cidArray, cJSON_CreateNumber(Config->cidArray[i]));
//    }

//    cJSON_AddItemToObject(config,"channelCount",cidArray);
    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "addModule");

    cJSON_AddItemToArray(params,config);
    cJSON_AddItemToObject(request, "params", params);
    data = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",data);
    cJSON_Delete(request);

    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {

       // qWarning("in run function %s\n",run.c_str());
        response = cJSON_Parse(run.c_str());//change string into json object
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        error = cJSON_GetObjectItem(response,"error");
       // config = cJSON_GetObjectItem(result,"config");

        //recv a moduleConfig data struct from cJSON object
        e.result = error->valueint;
        e.info = errorInfo->valuestring;
    }else{
        e.result = -256;
        e.info = "webservice failed!";
    }

    if(response){
        cJSON_Delete(response);
    }
    if(data){
        free(data);
        data = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;

}

//function 21

void MCRClientImpl::addChannel(ChannelConfig *Config,int &GetCid,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap  *s =  soap_new();

    std::string run ;
    char* data = NULL;

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *getCid = NULL,*error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();
    cJSON *config = cJSON_CreateObject();

    cJSON *mid = cJSON_CreateNumber(Config->mid);
    cJSON_AddItemToObject(config,"mid",mid);
    cJSON *cid = cJSON_CreateNumber(Config->cid);
    cJSON_AddItemToObject(config,"cid",cid);
    QString nameStr = Config->name.c_str();
    cJSON *name = cJSON_CreateString(nameStr.toLatin1().toPercentEncoding());
    cJSON_AddItemToObject(config,"name",name);
    QString aliasStr = Config->alias.c_str();
    cJSON *alias = cJSON_CreateString(aliasStr.toLatin1().toPercentEncoding());
    cJSON_AddItemToObject(config,"alias",alias);
    cJSON *ip = cJSON_CreateString(Config->ip.c_str());
    cJSON_AddItemToObject(config,"ip",ip);
    cJSON *type = cJSON_CreateNumber(Config->type);
    cJSON_AddItemToObject(config,"type",type);
    cJSON *streamUrl = cJSON_CreateString(Config->streamUrl.c_str());
    cJSON_AddItemToObject(config,"streamUrl",streamUrl);
    cJSON *streamServerUrl = cJSON_CreateString(Config->streamServerUrl.c_str());
    cJSON_AddItemToObject(config,"streamServerUrl",streamServerUrl);
    cJSON *recordServerUrl = cJSON_CreateString(Config->recordServerUrl.c_str());
    cJSON_AddItemToObject(config,"recordServerUrl",recordServerUrl);
    cJSON *fileServerUrl = cJSON_CreateString(Config->fileServerUrl.c_str());
    cJSON_AddItemToObject(config,"fileServerUrl",fileServerUrl);
    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "addChannel");

    cJSON_AddItemToArray(params,config);

    cJSON_AddItemToObject(request, "params", params);
    char* tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",data);
    data = tmp1;
    cJSON_Delete(request);

    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
      //  qWarning("\nChannelConfig=%s\n\n",run.c_str());
        response = cJSON_Parse(run.c_str());//change string into json object
        result = cJSON_GetObjectItem(response,"result");
        getCid = cJSON_GetObjectItem(result,"cid");
        config = cJSON_GetObjectItem(result,"config");
        error = cJSON_GetObjectItem(response,"error");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;
        //recv a moduleConfig data struct from cJSON object
        if(getCid->type==cJSON_String){
            GetCid = atoi(getCid->valuestring);
        }else if(getCid->type==cJSON_Number){
            GetCid = getCid->valueint;
        }
    }
    else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;

}
//function 22
void MCRClientImpl::getChannelCount(int Mid,QVector<int> &array,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap* s = soap_new();
    std::string run = "" ,data = "" ;
    char *tmp1 = NULL ;

   // qWarning("url = %s\n",url);

    cJSON * request = NULL ,*response = NULL, *count = NULL,*result = NULL,
                    *errorInfo = NULL,*error = NULL;

    cJSON *params = cJSON_CreateArray();
    cJSON *array_recv = cJSON_CreateArray();

    cJSON *mid = cJSON_CreateNumber(Mid);


    request = cJSON_CreateObject();
    cJSON_AddStringToObject(request, "function", "getChannelCount");

    cJSON_AddItemToArray(params,mid);
    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
       // qDebug("run --function debug\n");
       // qDebug("run function debug\n");
        //qDebug("---%s",run.c_str());
        int i,size;
        response = cJSON_Parse(run.c_str());//change string into json object
       // tmp1 = cJSON_Print(response);//change json object into string
       // qDebug("data--:%s\n",tmp1);

        result = cJSON_GetObjectItem(response,"result");


        count = cJSON_GetObjectItem(result,"channelCount");

        size = cJSON_GetArraySize(count);

       // qDebug("size is %d",size);
        for(i=0;i<size;i++) {
            array_recv = cJSON_GetArrayItem(count,i);
            if(array_recv->type == cJSON_String) {
             array.append(atoi(array_recv->valuestring));
            }else if (array_recv->type == cJSON_Number){
                array.append(array_recv->valueint);
            }

        }
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        error = cJSON_GetObjectItem(response,"error");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

    }else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return;
}
//function 23
void MCRClientImpl::addChannelRecord(RecordConfig *Config,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap  *s =  soap_new();

    std::string run ;
    char* data = NULL;

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();
    cJSON *config = cJSON_CreateObject();

    cJSON *mid = cJSON_CreateNumber(Config->mid);
    cJSON_AddItemToObject(config,"mid",mid);
    cJSON *cid = cJSON_CreateNumber(Config->cid);
    cJSON_AddItemToObject(config,"cid",cid);

    cJSON *srcUrl = cJSON_CreateString(Config->srcUrl.c_str());
    cJSON_AddItemToObject(config,"srcUrl",srcUrl);

    cJSON *prefix = cJSON_CreateString(Config->prefix.c_str());
    cJSON_AddItemToObject(config,"prefix",prefix);

    cJSON *segment = cJSON_CreateNumber(Config->segment);
    cJSON_AddItemToObject(config,"segment",segment);

    cJSON *timeServer = cJSON_CreateString(Config->timeServer.c_str());
    cJSON_AddItemToObject(config,"timeServer",timeServer);

    cJSON *type = cJSON_CreateString(Config->type.c_str());
    cJSON_AddItemToObject(config,"type",type);


    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "addChannelRecord");

    cJSON_AddItemToArray(params,config);

    cJSON_AddItemToObject(request, "params", params);
    char* tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",data);
    data = tmp1;
    cJSON_Delete(request);

    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
       // qWarning("\nChannelConfig=%s\n\n",run.c_str());
        response = cJSON_Parse(run.c_str());//change string into json object
        result = cJSON_GetObjectItem(response,"result");
        config = cJSON_GetObjectItem(result,"config");
        error = cJSON_GetObjectItem(response,"error");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;
        //recv a moduleConfig data struct from cJSON object

    }
    else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;

}
//function 24
void MCRClientImpl::deleteChannelRecord(int Mid,int Cid,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return ;
    }

    struct soap* s = soap_new();
    std::string run ,data ;
    char* tmp1 = NULL;

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();


    cJSON *mid = cJSON_CreateNumber(Mid);
    cJSON *cid = cJSON_CreateNumber(Cid);

    request = cJSON_CreateObject();


    cJSON_AddStringToObject(request, "function", "deleteChannelRecord");
    cJSON_AddItemToArray(params,mid);
    cJSON_AddItemToArray(params,cid);



    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
        response = cJSON_Parse(run.c_str());

        error = cJSON_GetObjectItem(response,"error");
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;
    }
    else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}
//function 25
void MCRClientImpl::reboot(const char*url,ErrorInfo &e)
{
    if(url == NULL) {
        printf("get url error\n");
        return  ;
    }

    struct soap  *s =  soap_new();

    std::string run ;
    char* data = NULL;

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();
    cJSON *config = cJSON_CreateObject();


    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "reboot");

    cJSON_AddItemToArray(params,config);

    cJSON_AddItemToObject(request, "params", params);
    char* tmp1 = cJSON_Print(request);//change json object into string
    //printf("data:%s\n",data);
   // qDebug("data reboot:%s",data);
    data = tmp1;
    cJSON_Delete(request);

    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
       // qWarning("\nChannelConfig=%s\n\n",run.c_str());
        response = cJSON_Parse(run.c_str());//change string into json object
        result = cJSON_GetObjectItem(response,"result");
        config = cJSON_GetObjectItem(result,"config");
        error = cJSON_GetObjectItem(response,"error");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;
        //recv a moduleConfig data struct from cJSON object

    }
    else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}
//function 26

void MCRClientImpl::update(QString *pStr,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        printf("get url error\n");
        return  ;
    }

    struct soap* s = soap_new();
    std::string run ,data ;
    char* tmp1 = NULL;

    cJSON * request = NULL ,*response = NULL,*result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();


    cJSON *str = cJSON_CreateString(pStr->toStdString().c_str());

    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "update");
    cJSON_AddItemToArray(params,str);
    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // printf("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);

    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
      //  qDebug("in run function\n");
        response = cJSON_Parse(run.c_str());
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        error = cJSON_GetObjectItem(response,"error");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

    }else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}
//function 27
void MCRClientImpl::checkUpdateStatus(int &Status,int Mid,int Cid,const char* url,ErrorInfo &e)
{
    if(url == NULL) {
        printf("get url error\n");
        return  ;
    }

    struct soap  *s =  soap_new();

    std::string run ,data;
    char *tmp1 = NULL;

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;
    cJSON *status = NULL;
    cJSON *params = cJSON_CreateArray();


    cJSON *mid = cJSON_CreateNumber(Mid);
    cJSON *cid = cJSON_CreateNumber(Cid);

    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "checkUpdateStatus");
    cJSON_AddItemToArray(params,mid);
    cJSON_AddItemToArray(params,cid);
    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // printf("data:%s\n",tmp1);
    data = tmp1;

    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
        //qDebug("in run function %s\n",run.c_str());
        response = cJSON_Parse(run.c_str());//change string into json object
        error = cJSON_GetObjectItem(response,"error");
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

        status = cJSON_GetObjectItem(result,"status");
        //Status = status->valueint;
        if(status->type==cJSON_String){
            Status = atoi(status->valuestring);
        }else if(status->type==cJSON_Number){
            Status = status->valueint;
        }
    }else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return;
}

//function 28
 void MCRClientImpl::closePlayer(const char*url,ErrorInfo &e)
{
     if(url == NULL) {
         printf("get url error\n");
         return  ;
     }

     struct soap  *s =  soap_new();

     std::string run ;
     char* data = NULL;

     cJSON * request = NULL ,*response = NULL, *result = NULL,
                     *error = NULL,*errorInfo = NULL;

     cJSON *params = cJSON_CreateArray();
     cJSON *config = cJSON_CreateObject();


     request = cJSON_CreateObject();

     cJSON_AddStringToObject(request, "function", "closePlayer");

     cJSON_AddItemToArray(params,config);

     cJSON_AddItemToObject(request, "params", params);
     char* tmp1 = cJSON_Print(request);//change json object into string
     //printf("data:%s\n",data);
    // qDebug("data reboot:%s",data);
     data = tmp1;
     cJSON_Delete(request);

     setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
     s->connect_timeout = WEBSERVICE_TIMEOUT;
     if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
        // qWarning("\nChannelConfig=%s\n\n",run.c_str());
         response = cJSON_Parse(run.c_str());//change string into json object
         result = cJSON_GetObjectItem(response,"result");
         config = cJSON_GetObjectItem(result,"config");
         error = cJSON_GetObjectItem(response,"error");
         errorInfo = cJSON_GetObjectItem(result,"errorInfo");
         e.result = error->valueint;
         e.info = errorInfo->valuestring;
         //recv a moduleConfig data struct from cJSON object

     }
     else{
         e.result = -256;
         e.info = "webservice failed!";
     }
     if(response){
         cJSON_Delete(response);
     }
     if(tmp1){
         free(tmp1);
         tmp1 = NULL;
     }
     soap_destroy(s);
     soap_end(s);
     soap_done(s);
     free(s);
     return ;
}
//function 29
void MCRClientImpl::openPlayer(QString *streamUrl,const char *url,int isLive,ErrorInfo &e)
{
    if(url == NULL) {
        printf("get url error\n");
        return  ;
    }

    struct soap  *s =  soap_new();

    std::string run ;
    char* data = NULL;

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();

    cJSON * StreamUrl = cJSON_CreateString(streamUrl->toStdString().c_str());
    cJSON * live = cJSON_CreateNumber(isLive);
    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "openPlayer");


    cJSON_AddItemToArray(params,StreamUrl);
    cJSON_AddItemToArray(params,live);

    cJSON_AddItemToObject(request, "params", params);
    char* tmp1 = cJSON_Print(request);//change json object into string
    //printf("data:%s\n",data);
   // qDebug("data reboot:%s",data);
    data = tmp1;
    cJSON_Delete(request);

    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
       // qWarning("\nChannelConfig=%s\n\n",run.c_str());
        response = cJSON_Parse(run.c_str());//change string into json object
        result = cJSON_GetObjectItem(response,"result");

        error = cJSON_GetObjectItem(response,"error");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;
        //recv a moduleConfig data struct from cJSON object

    }
    else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}
//function 30
void MCRClientImpl::getStatusIsOpenedPlayer(int &isOpened, const char *url, ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap* s = soap_new();
    std::string run = "" ,data = "" ;
    char *tmp1 = NULL ;


    cJSON * request = NULL ,*response = NULL, *status = NULL,*result = NULL,
                    *errorInfo = NULL,*error = NULL;

    cJSON *params = cJSON_CreateArray();

    request = cJSON_CreateObject();
    cJSON_AddStringToObject(request, "function", "getStatusIsOpenedPlayer");

    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
       // qDebug("run --function debug\n");
       // qDebug("run function debug\n");
        //qDebug("---%s",run.c_str());

        response = cJSON_Parse(run.c_str());//change string into json object
       // tmp1 = cJSON_Print(response);//change json object into string
       // qDebug("data--:%s\n",tmp1);

        result = cJSON_GetObjectItem(response,"result");


        status = cJSON_GetObjectItem(result,"isOpened");

        if(status){
            if(status->type == cJSON_Number){
                if(1 == status->valueint){
                    isOpened = 1;
                }else if(0 == status->valueint){
                    isOpened = 0;
                }
            }else if(status->type == cJSON_String){
                if("1" == status->valuestring){
                    isOpened = 1;
                }else if("0" == status->valuestring){
                    isOpened = 0;
                }
            }

        }else{
            isOpened = false;
        }
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        error = cJSON_GetObjectItem(response,"error");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

    }else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return;
}
//function 31
void MCRClientImpl::getStatusIsPlayingPlayer(int &isPlaying,const char *url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap* s = soap_new();
    std::string run = "" ,data = "" ;
    char *tmp1 = NULL ;


    cJSON * request = NULL ,*response = NULL, *status = NULL,*result = NULL,
                    *errorInfo = NULL,*error = NULL;

    cJSON *params = cJSON_CreateArray();

    request = cJSON_CreateObject();
    cJSON_AddStringToObject(request, "function", "getStatusIsPlayingPlayer");

    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
       // qDebug("run --function debug\n");
        //qDebug("run function debug\n");
        //qDebug("---%s",run.c_str());

        response = cJSON_Parse(run.c_str());//change string into json object
       // tmp1 = cJSON_Print(response);//change json object into string
       // qDebug("data--:%s\n",tmp1);

        result = cJSON_GetObjectItem(response,"result");


        status = cJSON_GetObjectItem(result,"isPlaying");

        if(status){
            if(status->type == cJSON_Number){
                if(1 == status->valueint){
                    isPlaying = true;
                }else if(0 == status->valueint){
                    isPlaying = false;
                }
            }else if(status->type == cJSON_String){
                if("1" == status->valuestring){
                    isPlaying = true;
                }else if("0" == status->valuestring){
                    isPlaying = false;
                }
            }
        }else{
            isPlaying = false;
        }
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        error = cJSON_GetObjectItem(response,"error");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

    }else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return;
}
//function 32
void MCRClientImpl::getStatusCanSeekPlayer(int &canSeek, const char *url , ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap* s = soap_new();
    std::string run = "" ,data = "" ;
    char *tmp1 = NULL ;


    cJSON * request = NULL ,*response = NULL, *status = NULL,*result = NULL,
                    *errorInfo = NULL,*error = NULL;

    cJSON *params = cJSON_CreateArray();

    request = cJSON_CreateObject();
    cJSON_AddStringToObject(request, "function", "getStatusCanSeekPlayer");

    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
       // qDebug("run --function debug\n");
      //  qDebug("run function debug\n");
        //qDebug("---%s",run.c_str());

        response = cJSON_Parse(run.c_str());//change string into json object
       // tmp1 = cJSON_Print(response);//change json object into string
       // qDebug("data--:%s\n",tmp1);

        result = cJSON_GetObjectItem(response,"result");


        status = cJSON_GetObjectItem(result,"canSeek");

        if(status){
            if(status->type == cJSON_Number){
               if( 1 == status->valueint){
                   canSeek = 1;
               }else if(0 == status->valueint){
                   canSeek = 0;
               }
            }else if(status->type == cJSON_String){
                if("1" == status->valuestring){
                    canSeek = 1;
                }else if("0" == status->valuestring){
                    canSeek = 0;
                }
            }
        }else{
            canSeek = false;
        }
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        error = cJSON_GetObjectItem(response,"error");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

    }else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return;
}
//function 33
void MCRClientImpl::getFileLengthPlayer(int64_t &length,const char *url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap* s = soap_new();
    std::string run = "" ,data = "" ;
    char *tmp1 = NULL ;



    cJSON * request = NULL ,*response = NULL, *Length = NULL,*result = NULL,
                    *errorInfo = NULL,*error = NULL;

    cJSON *params = cJSON_CreateArray();


    request = cJSON_CreateObject();
    cJSON_AddStringToObject(request, "function", "getFileLengthPlayer");


    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
       // qDebug("run --function debug\n");
      //  qDebug("run function debug\n");
       // qDebug("---%s",run.c_str());

        response = cJSON_Parse(run.c_str());//change string into json object
       // tmp1 = cJSON_Print(response);//change json object into string
       // qDebug("data--:%s\n",tmp1);

        result = cJSON_GetObjectItem(response,"result");


        Length = cJSON_GetObjectItem(result,"fileLength");

        if(Length){
         if(Length->type == cJSON_String){
             qWarning("length is string\n");
                length = atoll(Length->valuestring);
         }else if(Length->type == cJSON_Number){
            // qWarning("length is number\n");
                length = Length->valuedouble;
         }else{
             qWarning("length is unknown\n");
         }
        }else{
            qWarning("length is NULL\n");
        }
//        qWarning("length is %lld\n",length);
//        qWarning("Length->valuelonglong is %lld\n",Length->valuedouble);
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        error = cJSON_GetObjectItem(response,"error");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

    }else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return;
}
//function 34
void MCRClientImpl::getFileTimePlayer(int64_t &time, const char *url, ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap* s = soap_new();
    std::string run = "" ,data = "" ;
    char *tmp1 = NULL ;



    cJSON * request = NULL ,*response = NULL, *Time = NULL,*result = NULL,
                    *errorInfo = NULL,*error = NULL;

    cJSON *params = cJSON_CreateArray();


    request = cJSON_CreateObject();
    cJSON_AddStringToObject(request, "function", "getFileTimePlayer");


    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;
    cJSON_Delete(request);
    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
       // qDebug("run --function debug\n");
       // qDebug("run function debug\n");
        //qDebug("---%s",run.c_str());

        response = cJSON_Parse(run.c_str());//change string into json object
       // tmp1 = cJSON_Print(response);//change json object into string
       // qDebug("data--:%s\n",tmp1);

        result = cJSON_GetObjectItem(response,"result");


        Time = cJSON_GetObjectItem(result,"fileTime");

        if(Time){
         if(Time->type == cJSON_String){
                time = atoll(Time->valuestring);
         }else if(Time->type == cJSON_Number){
                time = Time->valuedouble;
         }
        }
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        error = cJSON_GetObjectItem(response,"error");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

    }else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return;
}
//function 35
void MCRClientImpl::playFilePlayer(const char *url,ErrorInfo &e)
{
    if(url == NULL) {
        printf("get url error\n");
        return  ;
    }

    struct soap  *s =  soap_new();

    std::string run ;
    char* data = NULL;

    cJSON * request = NULL ,*response = NULL, *result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();
    cJSON *config = cJSON_CreateObject();


    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "playFilePlayer");

    cJSON_AddItemToArray(params,config);

    cJSON_AddItemToObject(request, "params", params);
    char* tmp1 = cJSON_Print(request);//change json object into string
    //printf("data:%s\n",data);
   // qDebug("data reboot:%s",data);
    data = tmp1;
    cJSON_Delete(request);

    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
       // qWarning("\nChannelConfig=%s\n\n",run.c_str());
        response = cJSON_Parse(run.c_str());//change string into json object
        result = cJSON_GetObjectItem(response,"result");
        config = cJSON_GetObjectItem(result,"config");
        error = cJSON_GetObjectItem(response,"error");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;
        //recv a moduleConfig data struct from cJSON object

    }
    else{
        e.result = -256;
        e.info = "webservice failed!";
    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}
//function 36
void MCRClientImpl::setPosition(int64_t pos,const char *url,ErrorInfo &e)
{
    if(url == NULL) {
        qWarning("get url error\n");
        return  ;
    }

    struct soap* s = soap_new();
    std::string run ,data ;
    char* tmp1 = NULL;



    cJSON * request = NULL ,*response = NULL,*result = NULL,
                    *error = NULL,*errorInfo = NULL;

    cJSON *params = cJSON_CreateArray();


    cJSON *position = cJSON_CreateNumber(pos);

    request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "function", "setPosition");
    cJSON_AddItemToArray(params,position);
    cJSON_AddItemToObject(request, "params", params);
    tmp1 = cJSON_Print(request);//change json object into string
   // qWarning("data:%s\n",tmp1);
    data = tmp1;

    cJSON_Delete(request);

    setSocketKeepAlive(s,WEBSERVICE_TIMEOUT);
    s->connect_timeout = WEBSERVICE_TIMEOUT;
    if(soap_call_ns1__run(s,url,NULL,data,run) == SOAP_OK) {
       // qDebug("in run function\n");
        response = cJSON_Parse(run.c_str());
        result = cJSON_GetObjectItem(response,"result");
        errorInfo = cJSON_GetObjectItem(result,"errorInfo");
        error = cJSON_GetObjectItem(response,"error");
        e.result = error->valueint;
        e.info = errorInfo->valuestring;

    }
    if(response){
        cJSON_Delete(response);
    }
    if(tmp1){
        free(tmp1);
        tmp1 = NULL;
    }
    soap_destroy(s);
    soap_end(s);
    soap_done(s);
    free(s);
    return ;
}
