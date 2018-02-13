//
//  TMError.h
//  soapServer
//
//  Created by shanchun on 16/1/11.
//  Copyright © 2016年 shanchun. All rights reserved.
//

#ifndef TMError_h
#define TMError_h

#include <stdio.h>
typedef enum{
    execError = 1,
    noError = 0,
    paramError = -1,
    IOError = -2,
    methodNotExistError = -999
}TMError;
void getErrorInfo(TMError error,char *info,int length);
#endif /* TMError_h */
