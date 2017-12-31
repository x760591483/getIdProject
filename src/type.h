#ifndef _TYPE_H
#define _TYPE_H
#include <evhtp/evhtp.h>
#include "fileread.h"
#include "logLog.h"
#include <time.h>

#define KEEPMAXNUM 32
#define KEEPALLMAX 65535
#define KEEPONEMAX 200
#define INFDATANUM 5
extern const char *INFdata[INFDATANUM]=
{
    "ok",
    "Http not have type data",
    "Http type is not right",
    "Http num is not rigth",
    "unknow err happen"

};


typedef struct IDLIST
{
    time_t tdata;
    int type;
    int start;
    int num;
}idlist;

typedef struct KEEPDATA
{
    int usenum;
    FILE *filefd;
    time_t tdata;
    time_t trange;
    int addData[KEEPMAXNUM]; 

}keepdata;
typedef struct SYSDATA
{
    evbase_t *evbase;
    evhtp_t *htp;
    keepdata *kdata;

    filestruct *fdata;
    logdata *ld;    
}sysdata;



#endif
