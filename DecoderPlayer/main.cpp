#include <QCoreApplication>
#include"soapH.h"
#include <pthread.h>
#include <signal.h>
#include"cJSON.h"
#include "string.h"
#include "soapPackBinding.nsmap"
#include "myffplayer.h"
#include "dataStruct.h"
#include "TMError.h"
#include"workerthread.h"
#define SOAP_PORT 5678
#define STACKSIZE 4*1024*1024

WorkerThread *mThread = NULL;
MyFfPlayer *mPlayer = NULL;

static int quit = 0;
struct soap soapObj;

void signal_handler(int sig){
    switch (sig) {
        case SIGINT:
            printf("SIGINT detected!\n");
            quit = 1;
            soap_destroy(&soapObj);
            soap_end(&soapObj);
            soap_done(&soapObj);
            break;
        default:
            break;
    }
}
static int http_send_header(struct soap *soap, const char *s){
    const char *t;
    do{
    t = strchr(s, '\n'); /* disallow \n in HTTP headers */
        if (!t)
            t = s + strlen(s);
        if (soap_send_raw(soap, s, t - s))
            return soap->error;
        s = t + 1;
    } while (*t);
    return SOAP_OK;
}
//static int http_post_header(struct soap *soap, const char *key, const char *val)
//{ if (key)
//  { if (http_send_header(soap, key))
//      return soap->error;
//    if (val && (soap_send_raw(soap, ": ", 2) || http_send_header(soap, val)))
//      return soap->error;
//  }
//  return soap_send_raw(soap, "\r\n", 2);
//}
int http_get(struct soap * soap);

void getErrorInfo(TMError error,char *info,int length){
    switch(error){
        case noError :
            snprintf(info, length, "");
            break;
        case paramError :
            snprintf(info, length, "param error");
            break;
        case IOError :
            snprintf(info, length, "IO error");
            break;
        case execError :
            snprintf(info, length, "background application exec error");
            break;
        case methodNotExistError :
            snprintf(info, length, "method not exist");
            break;
        default:
            snprintf(info, length, "unknown error");
    }
}

void copy_struct_data_to_json(void *stru,DataType type,cJSON *json){
    ParamType type_def = paramTypes[type];
    ParamStru *param_stru = type_def.stru;
    int i;
    for(int i=0;i<type_def.length;i++){
        ParamStru element_def = param_stru[i];
        if(element_def.type == int_type){
            int element;
            memcpy(&element,(char*)stru+element_def.offset,element_def.size);
            cJSON_AddNumberToObject(json, element_def.name, element);
        }else if(element_def.type == int_array_type){
            int array_length = element_def.size / sizeof(int);
            int element[array_length];
            memcpy(element,(char*)stru+element_def.offset,element_def.size);
            cJSON* array_json = cJSON_CreateIntArray(element, array_length);
            cJSON_AddItemToObject(json, element_def.name, array_json);
        }else if(element_def.type == str_type){
            char element[element_def.size];
            memcpy(element,(char*)stru+element_def.offset,element_def.size);
            cJSON_AddStringToObject(json, element_def.name, element);
        } //结构体内的变量类型暂时就这三种
    }
}


void copy_json_data_to_struct(cJSON *json,DataType type,void *stru){
    ParamType type_def = paramTypes[type];
    ParamStru *param_stru = type_def.stru;
    int i;
    for(i=0;i<type_def.length;i++){
        ParamStru element_def = param_stru[i];
        if(element_def.type == int_type){
            int element = cJSON_GetObjectItem(json, element_def.name)->valueint;
            memcpy((char*)stru+element_def.offset,&element,element_def.size);
        }else if(element_def.type == int_array_type){
            cJSON* json_arr = cJSON_GetObjectItem(json, element_def.name);
            int array_length = cJSON_GetArraySize(json_arr);
            int element[array_length];
            for(int j=0; j<array_length; j++){
                element[j] = cJSON_GetArrayItem(json_arr, j)->valueint;
            }
            memcpy((char*)stru+element_def.offset,element,element_def.size);
        }else if(element_def.type == str_type){
            memcpy((char*)stru+element_def.offset,cJSON_GetObjectItem(json, element_def.name)->valuestring,element_def.size);
        }
    }
}
TMError parse_param(int index,DataType type,cJSON *params_json,void* param){
    cJSON *param_json = cJSON_GetArrayItem(params_json, index);
    if(param_json == NULL){
        return paramError;
    }
    if(type == int_type){
        *(int *)param = param_json->valueint;
    }else if(type == longlong_type){
        *(long long *)param = param_json->valuedouble;
    }else if(type == str_type){
        *(char **)param = param_json->valuestring;
    }else{   //all is struct
        copy_json_data_to_struct(param_json, type, param);
    }
    return noError;
}

void response_result(const char *name,DataType type,void *p_result,TMError error, char **responseJsonStr){
    cJSON* response_json = cJSON_CreateObject();
    cJSON* result_json = cJSON_CreateObject();
    if(name != NULL){
        if(type == int_type){
            cJSON_AddNumberToObject(result_json, name, *(int*)p_result);
        }else if(type == longlong_type){
            cJSON_AddNumberToObject(result_json, name, *(long long*)p_result);
        }else if(type == str_type){
            cJSON_AddStringToObject(result_json, name, (char*)p_result);
        }else{  //all struct type
            cJSON *stru_json = cJSON_CreateObject();
            copy_struct_data_to_json(p_result, type, stru_json);
            cJSON_AddItemToObject(result_json, name, stru_json);
        }
    }
    char errorInfo[256];
    getErrorInfo(error, errorInfo, 256);
    cJSON_AddStringToObject(result_json, "errorInfo", errorInfo);
    cJSON_AddItemToObject(response_json, "result", result_json);
    cJSON_AddNumberToObject(response_json, "error", error);
    *responseJsonStr = cJSON_Print(response_json);
    cJSON_Delete(response_json);
}

void run_closePlayer(cJSON *params_json,char **responseJsonStr)
{
        TMError error = noError;
        qDebug("closePlayer");
        if(mPlayer->close()){
            error = noError;
            qDebug("closePlayer1");
        }else{
            error = execError;//close error
            qDebug("closePlayer2");
        }
        qDebug("closePlayer3");
        response_result(NULL, data_null, NULL, error, responseJsonStr);
}
void run_openPlayer(cJSON *params_json,char **responseJsonStr)
{
    TMError error = noError;
    char* url;
    int  isLive = 0;
    qDebug("openPlayer");
    error = parse_param(0, str_type, params_json, (void*)&url);
    if(error != noError){
        response_result(NULL, data_null, NULL, error, responseJsonStr);
        return;
    }
    error = parse_param(1, int_type, params_json, (void*)&isLive);
    if(error != noError){
        response_result(NULL, data_null, NULL, error, responseJsonStr);
        return;
    }
     qDebug("url %s ",url);
    if(mPlayer->open(QString::fromLatin1(url),isLive)){
        error = noError;
        qDebug("openPlayer1");
    }else{
        error = execError;
        qDebug("openPlayer2");
    }
    qDebug("openPlayer3");
    response_result(NULL, data_null, NULL, error, responseJsonStr);
}
void run_getStatusIsOpenedPlayer(cJSON *params_json,char **responseJsonStr)
{
    TMError error = noError;
    int isOpened = 0;
    isOpened = mPlayer->isOpened();
    response_result("isOpened",int_type,(void*)&isOpened,error,responseJsonStr);
}
void run_getStatusIsPlayingPlayer(cJSON *params_json,char **responseJsonStr)
{
    TMError error = noError;
    int isPlaying = 0;
   // qDebug("getStatusIsPlayingPlayer 0");
    isPlaying = mPlayer->isPlaying();
   // qDebug("getStatusIsPlayingPlayer 1--%d",isPlaying);
    response_result("isPlaying",int_type,(void*)&isPlaying,error,responseJsonStr);
    //qDebug("getStatusIsPlayingPlayer 2");
}
void run_getStatusCanSeekPlayer(cJSON *params_json,char **responseJsonStr)
{
    TMError error = noError;
    int isCanSeek = 0;
    isCanSeek = mPlayer->canSeek();
    response_result("canSeek",int_type,(void*)&isCanSeek,error,responseJsonStr);
}
void run_getFileLengthPlayer(cJSON *params_json,char **responseJsonStr)
{
    TMError error = noError;
    long long  length = 0;
    length = mPlayer->length();
    response_result("fileLength",longlong_type,(void*)&length,error,responseJsonStr);
}
void run_getFileTimePlayer(cJSON *params_json,char **responseJsonStr)
{
    TMError error = noError;
    long long  time = 0;
    time = mPlayer->time();
    response_result("fileTime",longlong_type,(void*)&time,error,responseJsonStr);
}
void run_playFilePlayer(cJSON *params_json,char **responseJsonStr)
{
    TMError error;
    if(mPlayer->play()){
        error = noError;
    }else{
        error = execError;
    }
    response_result(NULL, data_null, NULL, error, responseJsonStr);
}
void run_setPosition(cJSON *params_json,char **responseJsonStr)
{
    TMError error;
    int64_t pos;
    error = parse_param(0, longlong_type, params_json, (void*)&pos);
    if(error != noError){
        response_result(NULL,data_null,NULL,error,responseJsonStr);
        return;
    }
    qDebug("pos %lld",pos);
    if(mPlayer->setPosition(pos)){
        error = noError;
    }else{
        error = execError;
    }
    response_result(NULL, data_null, NULL, error, responseJsonStr);
}


void json_parse(const char *requestJsonStr,char **responseJsonStr){
    cJSON *request_json = cJSON_Parse(requestJsonStr);
    if(request_json == NULL){
        response_result(NULL, data_null, NULL, paramError, responseJsonStr);
        return;
    }
    cJSON *method_json = cJSON_GetObjectItem(request_json, "function");
    cJSON *params_json = cJSON_GetObjectItem(request_json, "params");
    if(method_json == NULL || params_json == NULL){
        response_result(NULL,data_null,NULL, paramError, responseJsonStr);
        cJSON_Delete(request_json);
        return;
    }
    if(strcmp(method_json->valuestring,"closePlayer") == 0){
        run_closePlayer(params_json,responseJsonStr);
    }else if(strcmp(method_json->valuestring,"openPlayer") == 0){
        run_openPlayer(params_json,responseJsonStr);
    }else if(strcmp(method_json->valuestring,"getStatusIsOpenedPlayer") == 0){
        run_getStatusIsOpenedPlayer(params_json,responseJsonStr);
    }else if(strcmp(method_json->valuestring,"getStatusIsPlayingPlayer") == 0){
        run_getStatusIsPlayingPlayer(params_json,responseJsonStr);
    }else if(strcmp(method_json->valuestring,"getStatusCanSeekPlayer") == 0){
        run_getStatusCanSeekPlayer(params_json,responseJsonStr);
    }else if(strcmp(method_json->valuestring,"getFileLengthPlayer") == 0){
        run_getFileLengthPlayer(params_json,responseJsonStr);
    }else if(strcmp(method_json->valuestring,"getFileTimePlayer") == 0){
        run_getFileTimePlayer(params_json,responseJsonStr);
    }else if(strcmp(method_json->valuestring,"playFilePlayer") == 0){
        run_playFilePlayer(params_json,responseJsonStr);
    }else if(strcmp(method_json->valuestring,"setPosition") == 0){
        run_setPosition(params_json,responseJsonStr);
    }else{
        response_result(NULL, data_null, NULL, methodNotExistError, responseJsonStr);
    }
    cJSON_Delete(request_json);
}

int  ns1__run(struct soap*, struct soap *p_soap, char *data, char **run)
{
        char *buf = NULL;
            //printf("receive json:%s\n",data);
        //fflush(stdout);
        json_parse(data, &buf);
        size_t len = strlen(buf);
        *run = (char *)soap_malloc(p_soap, len);
        strcpy(*run,buf);
        free(buf);
        //printf("response json:%s\n",*run);
        return SOAP_OK;
}
void* soapPthread(void* arg){
    pthread_t tid = pthread_self();
    pthread_detach(tid);
    struct soap *p_soap = (struct soap*)(arg);
    soap_serve(p_soap);
    soap_destroy(p_soap);
    soap_end(p_soap);
    soap_free(p_soap);
    p_soap = NULL;
    pthread_exit(NULL);
    return NULL;
}

void *soap_func(void* arg)
{
    SOAP_SOCKET bindSocket = 0,socket = 0;
    soap_init(&soapObj);
    soap_init2(&soapObj,SOAP_IO_KEEPALIVE,SOAP_IO_KEEPALIVE);
    soapObj.send_timeout = 5;
    soapObj.recv_timeout = 5;
    soapObj.max_keep_alive = 86400;
    soapObj.connect_flags = SO_LINGER;
    soapObj.bind_flags |= SO_REUSEADDR;
    soapObj.linger_time = 3;
//    soapObj.fget = http_get;
    bindSocket = soap_bind(&soapObj, NULL, SOAP_PORT, 100);
    if(!soap_valid_socket(bindSocket)){
        soap_print_fault(&soapObj, stderr);
        exit(-1);
    }
    printf("Bind success!\n");
    while(quit == 0){
        //  printf("Waiting for new connection...\n");
        struct soap *cp_soap;
        cp_soap = soap_copy(&soapObj);
        socket = soap_accept(cp_soap);
        if(!soap_valid_socket(socket)){
            printf("Accept soap connection failed!\n");
            sleep(1);
            soap_print_fault(&soapObj,stderr);
            continue;
        }
        cp_soap->socket = socket;
        pthread_t soap_tid;

        pthread_attr_t attr;

        pthread_attr_init(&attr);

        pthread_attr_setstacksize(&attr,STACKSIZE);

        pthread_create(&soap_tid,&attr,soapPthread,(void *)cp_soap);

        pthread_attr_destroy(&attr);
    }
    soap_done(&soapObj);
    printf("Quit!\n");
    return 0;
}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    struct sigaction act;
    memset(&act,0,sizeof(act));
    act.sa_handler = signal_handler;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGPIPE, &act, NULL);
    mPlayer = new MyFfPlayer();
 //   mPlayer->open("rtsp://192.168.0.210:8554/test-0",true);

    pthread_t soap_id;

    pthread_attr_t attr;

   int  ret = pthread_attr_init(&attr);
   if(ret != 0)
       return -1;

    ret = pthread_attr_setstacksize(&attr,STACKSIZE);
    if(ret == 0){
        pthread_create(&soap_id,&attr,soap_func,NULL);
    }

//      mThread = new WorkerThread(NULL,soap_func,NULL);
//      mThread->setStackSize(1*1024*1024);
//      mThread->start();
    while(1){
        QCoreApplication::processEvents();
        struct timespec tv;
        struct timespec tv_rval;
        tv.tv_sec = 0;
        tv.tv_nsec = 10000000;
        nanosleep(&tv,&tv_rval);
    }
//    pthread_attr_destroy(&attr);
    return a.exec();
}
int http_get(struct soap * soap)
{
    FILE *fd = NULL;

    char *s = strchr(soap->path, '?');
    if (!s || strcmp(s, "?wsdl"))
        return SOAP_GET_METHOD;

    fd = fopen("ns1.wsdl", "rb");

    if (!fd){
        fprintf(stderr,"can't open ns1.wsdl");
        return 404;
    }
    soap->http_content = "text/xml";

    soap_response(soap, SOAP_FILE);

    for (;;)
    {
        size_t r = fread(soap->tmpbuf, 1, sizeof(soap->tmpbuf), fd);
        if (!r)
            break;
        if (soap_send_raw(soap, soap->tmpbuf, r))
            break;
    }

    fclose(fd);
    soap_end_send(soap);

    return SOAP_OK;
}
