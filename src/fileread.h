#ifndef _FILEREAD_H
#define _FILEREAD_H
#include <stdio.h>
typedef struct MYSTRING
{
    char *data;
    int len;
}mystring;

typedef struct ONEDATA
{
    mystring key;
    mystring value;
    struct ONEDATA *next;
}onedata;

typedef struct FILEDATA
{
    char name[32];
    onedata *data;
    struct FILEDATA *next;
}filedata;


typedef struct FILESTRUCT
{
    const char *filepath;
    char *cdata;
    filedata *fdatahead;
}filestruct;


filestruct* FileLoad(const char *path);

int FileRead(filestruct *dat);

char* FileFindOneData(filestruct *dat,const char *filename,const char *key,char *value,int *valuelen);

/**
 *dat is loadFile of return 
 res is out
 * */
int readFile(void *dat,filedata **res);

/**
 *get key find  value of filedata 
 key is char* 
 value is out
 value is out
return :if value is NULL, then is filedata information
            value not NULL ,then copy filedata to value

 * */
char* getvalue(filedata *fdata,const char* key,char *value,int *valuelen);

/**
 *find one filedata ,while filedata name is same of filename
 * */
filedata* getfiledata(filedata *head,const char *filename); 

/*
 *free struct filedata 
 * */
int freeFiledata(filedata *dat);

/*
 *free onedata struct data 
 * **/
int freeOnedata(onedata *data);

/**
 *read file data and get void* is char*
 *
 * */
void* loadFile(const char *path);

int playFiledata(filedata *fhead,FILE *fd);
#endif
