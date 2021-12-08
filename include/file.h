/*
* Created by Aen on 2021/7/21.
*/
#ifndef _FILE_H
#define _FILE_H
#include <stdio.h>
typedef struct File_Map {
    char strFileName[256];
    //int iFd;
    FILE * tFp;
    int iFileSize;
    unsigned char *pucFileMapMem;
}T_File_Map, *P_File_Map;

typedef enum {
    FILETYPE_DIR = 0,
    FILETYPE_FILE,
}E_FileType;

typedef struct Dir_Content {
    char strName[256];
    E_FileType eFileType;
}T_Dir_Content, *P_Dir_Content;

int Map_file(P_File_Map ptFileMap);
void Un_map_file(P_File_Map ptFileMap);
void Free_dir_contents(P_Dir_Content *aptDirContents, int iNumber);
int Get_dir_contents(char *strDirName, P_Dir_Content **pptDirContents, int *piNumber);
int Get_files_indir(char *strDirName, int *piStartNumberToRecord, int *piCurFileNumber, int *piFileCountHaveGet, int iFileCountTotal, char apstrFileNames[][256]);


#endif //_FILE_H
