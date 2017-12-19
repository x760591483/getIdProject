//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "fileread.h"

void trimspace(char *dat,int num)
{
    if(dat == NULL || num <1)
    {
        return;
    }
    int len=(int)strlen(dat);
    if(len>num)
    {len=num;}
    int i=len-1;
    for(;i>=0;--i)
    {//清除尾部的 空格 回车 换行
        if(*(dat+i)==13 || *(dat+i)==10 || *(dat+i)==32)
        {
            *(dat+i)=0;
        }
        else
        {
            break;
        }                                                                                             
    }
    if(i<=0 || *(dat) != 32)
    {
        return;                                                                          
    }
    int j=0;
    for(;;)//表示字符串开始有空格 且字符串长度大于1
    {
        if(*(dat + j) != 32)
        {
            break;
        }
        ++j;
    }
    int k=0;
    for(;j<=i;++k,++j)
    {
        *(dat + k)= *(dat + j);
    }
    for(;k<=i;++k)
    {
        *(dat + k)=0;
    }
}

filestruct* FileLoad(const char *path)
{

    if(path ==NULL)
    {
        return NULL;
    }
    char *tem = (char*)loadFile(path);
    if(tem == NULL)
    {
        return NULL;
    }

    filestruct *fdata =(filestruct*)malloc(sizeof(filestruct));
    if(fdata ==NULL)
    {
        return NULL;
    }
    memset(fdata,0,sizeof(filestruct));

    fdata->filepath = path;
    fdata->cdata = tem;
    return fdata;
}

void* loadFile(const char *path)
{
    if(path == NULL)
    {   
        return NULL;
    }   
    FILE *fg = fopen(path,"r");
    if(fg==NULL)
    {   
        return NULL;
    }   
    int iswrite = 0;
    char chartem[1024]={0};
    int maxsize = sizeof(char) * 1024;
    int wherenow = 0;
    char *head = (char*)malloc(maxsize);
    if(head == NULL)
    {  
        fclose(fg);
        return NULL;
    }   
    memset(head,0,maxsize);
    while(1)
    {
        memset(chartem,0,sizeof(chartem));
        fgets(chartem,sizeof(chartem)-1,fg);
        if(feof(fg))
        {   
            break;                                
        }  
        trimspace(chartem,1024);
        if(chartem[0] == '/' && chartem[1] == '/')
        {//跳过注释行
            continue;
        }
        int temlen = strlen(chartem);
        if(temlen == 0)
        {
            if(iswrite)
            {
                iswrite =0;
                temlen = 1;
                chartem[0] = 32;                                                         
            }
            else
            {
                continue;
            }
        }
        else
        {
            iswrite =1;
        }
        if(temlen >= maxsize - wherenow -1 )
        {//表示剩余空间不足
            maxsize+=sizeof(char)*1024;
            char *headTem = (char*)malloc(maxsize);
            if(headTem == NULL)
            {
                free(head);
                fclose(fg);
                return NULL;
            }
            memset(headTem,0,maxsize);
            memcpy(headTem,head,wherenow);
            free(head);
            head=headTem;
        }
        strncpy(head + wherenow,chartem,temlen);
        wherenow =wherenow + temlen + 1;//多加一 为字符串后0
    }
    if(wherenow ==0)
    {
        free(head);
        fclose(fg);
        return NULL;
    }
    return head;
}

onedata* fetchInf(char *date)                                                            
{//传入的字符串必须是  ***:***** 的格式
    if(date ==NULL)
    {
        return NULL;
    }
    int tt =0;
    for(;;++tt)
    {
        if(*(date + tt)==0)
        {//不符合格式
            return NULL;
        }
        if(*(date + tt)== ':')
        {//找到 ：
            break;
        }
    }
    if(tt == 0 || *(date + tt +1)==0)
    {
        return NULL;
    }
    onedata *res =(onedata*)malloc(sizeof(onedata));
    memset(res,0,sizeof(onedata));
    res->key.data=date;
    res->key.len = tt;
    res->value.data=date + tt +1;
    res->value.len=strlen(date + tt +1);
    return res;                  
}
int freeOnedata(onedata *data)
{
    if(data == NULL)
    {
        return -1;
    }
    onedata *temhead = data;
    while(data != NULL)
    {
        temhead = data->next;
        free(data);
        data = temhead;
    }
    return 0;
}

int freeFiledata(filedata *dat)
{
    if(dat ==NULL)
    {
        return -1;
    }
    filedata *fileAA = dat;
    while(fileAA != NULL)
    {
        if(fileAA->data != NULL)
        {
            freeOnedata(fileAA->data);
        }
        dat = dat ->next;
        free(fileAA);                                                                    
        fileAA = dat;
    }
    return 0;
}

filedata* getfiledata(filedata *head,const char *filename) 
{
    if(head ==NULL || filename == NULL)
    {
        return NULL;
    }
    while(head !=NULL)
    {
        if(strcasecmp(head->name,filename)==0)
        {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

char* getvalue(filedata *fdata,const char* key,char *value,int *valuelen)
{
    if(fdata == NULL || key ==NULL || valuelen==NULL)
    {
        return NULL;
    }
    onedata *temone = fdata->data;
    int valen = (int)strlen(key);
    for(;temone !=NULL;temone = temone->next)
    {
        if(valen != temone->key.len)
        {
            continue;
        }
        if(strncmp(temone->key.data,key,valen)!=0)
        {
            continue;
        }
        if(temone->value.len <1)
        {
            *valuelen = -1;//表示value无数据
            return NULL;
        }
        else
        {
            *valuelen = temone->value.len;
            if(value ==NULL)
            {
                return temone->value.data;
            }
            strncpy(value,temone->value.data,temone->value.len);
            return value;
        }

    }
    *valuelen =0;
    return NULL;
}

int playFiledata(filedata *fhead,FILE *fd)
{
    if(fhead == NULL || fd == NULL)
    {
        return -1;
    }
    onedata *onetem=NULL;
    for(;fhead!=NULL;fhead = fhead->next)
    {
        fprintf(fd,"%s:%d\n",fhead->name,(int)strlen(fhead->name));
        onetem = fhead->data;
        for(;onetem !=NULL;onetem = onetem->next)
        {
            fwrite(onetem->key.data,onetem->key.len,1,fd);
            fprintf(fd,":%d|",onetem->key.len);
            fwrite(onetem->value.data,onetem->value.len,1,fd);
            fprintf(fd,":%d\n",onetem->value.len);
        }
        fprintf(fd,"\n");
    }

    return 0;
}

int FileRead(filestruct *dat)
{
    if(dat ==NULL)
    {
        return -1;
    }
    filedata *fdat=NULL;

    int ret = readFile(dat->cdata,&fdat);
    if(ret <0)
    {
        return ret;
    }
    if(dat->fdatahead !=NULL)
    {
        freeFiledata(dat->fdatahead);
    }
    dat->fdatahead = fdat;
    return 0;
}


char* FileFindOneData(filestruct *dat,const char *filename,const char *key,char *value,int *valuelen)                                                                                             
{
    if(dat ==NULL || filename ==NULL || key ==NULL || valuelen==NULL)
    {   
        *valuelen =-11;
        return NULL;
    }   
    filedata *tem=getfiledata(dat->fdatahead,filename);
    if(tem ==NULL)
    {   
        *valuelen = -12;
        return NULL;
    }   
    return getvalue(tem,key,value,valuelen);


}



int readFile(void *dat,filedata **res)
{
    if(dat ==NULL || res ==NULL)
    {
        return -1;

    }
    char *head = (char*)dat;
    filedata filehead;
    memset(&filehead,0,sizeof(filedata));
    filedata *filetem = &filehead;

    onedata onehead;
    memset(&onehead,0,sizeof(onedata));
    onedata *onetem = &onehead;
    int wherenow =0;
    int isnest =1;//表示是否重新一个结构体

    while(1)
    {
        if(*(head+wherenow)==0)
        {
            break;

        }
        else if(*(head+wherenow)== 32)
        {//一个结构体结束了
            filetem->data = onehead.next;

            isnest =1;
            wherenow = wherenow + 2;//跳过空格 和 0
            continue;
        }
        if(isnest)
        {//创建消息体 并赋值消息体的名字
            isnest =0;
            filetem->next =  (filedata*)malloc(sizeof(filedata));
            memset(filetem->next,0,sizeof(filedata));
            filetem = filetem->next;
            strncpy(filetem->name,head + wherenow,sizeof(filetem->name));
            wherenow = wherenow + strlen(head + wherenow)+1;
            memset(&onehead,0,sizeof(onedata));
            onetem = &onehead;
            continue;
        }
        onetem->next = fetchInf(head + wherenow);
        if(onetem->next==NULL)
        {
            freeFiledata(filehead.next);
            return -2;
        }
        onetem = onetem->next;                                                           
        wherenow = wherenow + strlen(head + wherenow) + 1;
    }
    filetem->data = onehead.next;
    *res = filehead.next;
    return 0;
}



