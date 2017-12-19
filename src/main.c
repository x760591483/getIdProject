#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
static sysdata sdata;



int main(int argc, char *argv[])
{
    memset(&sdata,0,sizeof(sysdata));
    sdata.fdata =FileLoad("./test.conf");    
    if(sdata.fdata==NULL)
    {
        printf("loadFile is NULL\n");
        return -1;
    }

    int ret = FileRead(sdata.fdata);
    if(ret <0)
    {
        printf("FileRead is ERR %d\n",ret);
        return -1;
    }

    playFiledata(sdata.fdata->fdatahead,stdout);


    

    return 0;
}
