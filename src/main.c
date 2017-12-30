#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "type.h"
#include "json/json.h"
#include <unistd.h>

static sysdata sdata;



int evhtp_send_data(evhtp_request_t *req,const char *data,int datalen)
{
    if(req ==NULL || data ==NULL || datalen <1)
    {
        return -1;
    }
    evhtp_kv_t * ctheader = 0;
    evhtp_kv_t * clheader = 0;
    char buflen[32]={0};
    snprintf(buflen,32,"%d",datalen);

printf("buf %s %d\n",data,(int)strlen(data));
    
    ctheader = evhtp_header_new("Content-Type","application/json; charset=utf-8", 1, 1);
    clheader = evhtp_header_new("Content-Length",buflen, 1,1);
    evhtp_headers_add_header (req->headers_out,ctheader);
    evhtp_headers_add_header (req->headers_out,clheader);
    evhtp_send_reply_start(req, EVHTP_RES_OK);
    evbuffer_add(req->buffer_out, data, strlen(data));
    evhtp_send_reply_body(req, req->buffer_out);
    evhtp_send_reply_end(req);
    printf("id_get_pr end\n");
    

    return 0;
}

int idmake(void *jdat,idlist *idat)
{
    if(jdat == NULL || idat == NULL)
    {
        return -1;
    }
    json_object *jarray= (json_object*)jdat;
    time_t ttem = idat->tdata;
    int inum= idat->num;
    int sta=idat->start;
    int type = idat->type;
    int last=0;
    int i=0;
    int j=0;
    char valuestr[32]={0};
    unsigned long long value = 0;
    for(;i<inum;++i)
    {
        last =0;
        for(j=0;j<32;++j)
        {
            if(ttem>>j & 0x01)
            {
                ++last;
            }
            if(sta>>j & 0x01)
            {
                ++last;
            }
            if(type>>j & 0x01)
            {
                ++last;
            }
        
        }
        value = (ttem <<32) + (sta<<16) + (type<<8) + last;
        memset(valuestr,0,sizeof(valuestr));
        snprintf(valuestr,32,"%lld",value);
        json_object_array_add(jarray,json_object_new_string(valuestr));
        ++sta;

    }
    return 0;
}

int jsonmake(idlist *iddata,char **data)
{
    if(data == NULL || iddata ==NULL)
    {
        return -1;
    }
    json_object *my_object;
    json_object *my_array;
    my_array = json_object_new_array();
    my_object = json_object_new_object();
    if(iddata->type <0)
    {
        json_object_object_add(my_object, "state", json_object_new_int(iddata->type));
    }
    else
    {
        json_object_object_add(my_object, "state", json_object_new_int(0));
        idmake(my_array,iddata);
    }
    json_object_object_add(my_object, "inf", json_object_new_string("test"));
    json_object_object_add(my_object, "data", my_array);
    
    char *outdat = json_object_to_json_string(my_object);
    int outdatlen = strlen(outdat);
      *data = (char*)malloc(outdatlen +1);
      memset(*data,0,outdatlen+1);
      memcpy(*data,outdat,outdatlen);
    json_object_put(my_object);
}

void id_get_pr(evhtp_request_t *req, void *arg ) 
{
    const char * type = evhtp_kv_find(req->uri->query,"type");
    const char * num = evhtp_kv_find(req->uri->query,"num");
    evhtp_kv_t * ctheader = 0;
    evhtp_kv_t * clheader = 0;
    if(!type)
    {

    }
    char buf[64]="abcdefghijklmnopqrstuvwyz";
    evhtp_send_data(req,buf,strlen(buf));
    printf("id_get_pr end\n");  
}

int evhtpSet(sysdata *sdata)
{
    if(sdata ==NULL)
    {
        return -1;
    }
    sdata->evbase = event_base_new();
    sdata->htp = evhtp_new(sdata->evbase, NULL);
    evhtp_use_threads(sdata->htp, NULL, 2, NULL); 
    evhtp_bind_socket(sdata->htp,"0.0.0.0" ,8000 , 1024);
    evhtp_set_cb(sdata->htp, "/test", id_get_pr, NULL);

    return 0;
}

int keepFileInit(keepdata *kdata,const char* path)
{
    if(path ==NULL || kdata ==NULL)
    {
        return -1;
    }

    FILE *data = fopen(path,"w+");
    if(data == NULL)
    {
        return -2;
    }
    if(kdata->filefd != NULL)
    {
        fclose(kdata->filefd);
        kdata->filefd = NULL;
    }

    kdata->filefd = data;
    return 0;
}
keepdata* keepInit(int type)
{
    if(type <0)
    {
        return NULL;
    }
    keepdata *data= (keepdata *)malloc(sizeof(keepdata));
    if(data==NULL)
    {
        return NULL;
    }
    memset(data,0,sizeof(keepdata));
    data->usenum = type;
    return data;
}
int keepUpdatTime(keepdata *kdata)
{
        if(kdata == NULL)
        {
            return -1;
        }
        time_t tem = time(NULL);
        if(tem == (kdata->tdata + kdata->trange))
        {
            return 0;
        }
        kdata->tdata = tem - kdata->trange;
        memset(kdata->addData,0,sizeof(kdata->addData));
        return 1;//表示更新过
}
int keepGetData(keepdata *kdata,int type,int num,idlist *out)
{
    if(out == NULL || kdata == NULL || type >= KEEPMAXNUM || type <0 || num <1 || num >KEEPONEMAX)
    {
        return -1;
    }
    keepUpdatTime(kdata);
    int data = kdata->addData[type];
    if(data + num > KEEPALLMAX)
    {
        return -2;
    }
    kdata->addData[type] +=num;

    out->tdata =kdata->tdata + kdata->trange;
    out->start = data;
    out->num = num;
    out->type = type;

    return 0;
}

int main(int argc, char *argv[])
{
    memset(&sdata,0,sizeof(sysdata));
   // time_t ti = time(NULL);
   // printf("ti %ld\n",ti);
   // printf("time_t %d\n",(int)sizeof(time_t));
   // printf("int %d longlong %d\n",(int)sizeof(int),(int)sizeof(long long));
    

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

    char logfile[32]={0};
    int loglen=0;

    sdata.ld = logInit("log.log",10);
    if(sdata.ld == NULL)
    {
        printf("logInit is NULL\n");
        return -1;
    }
    
    sdata.kdata = keepInit(1);
    if(sdata.kdata==NULL)
    {
        printf("keepFileInit is err %d\n",ret);
        return -2;
    }
    idlist idata;
    memset(&idata,0,sizeof(idata));
    keepGetData(sdata.kdata,0,5,&idata);
    char *oudat=NULL;
    jsonmake(&idata,&oudat);
    printf("out %s\n",oudat);

    //创建evhtp接口  用于接收HTTP请求
    ret = evhtpSet(&sdata);
    if(ret !=0)
    {
        printf("evhtpSet is err %d\n",ret);
        return -1;
    }
    printf("loop is pre\n");
   event_base_loop(sdata.evbase, 0); 
   printf("loop is next\n");
   evhtp_unbind_socket(sdata.htp);
   evhtp_free(sdata.htp);
   event_base_free(sdata.evbase);
    printf("project stop\n");

    return 0;
}
