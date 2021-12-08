/*
* Created by Aen on 2021/7/23.
*/
#ifndef _PICFMT_MANAGER_H
#define _PICFMT_MANAGER_H
#include <config.h>
#include <pic_analysis.h>
#include <picfmt_manager.h>
#include <string.h>
#include <file.h>
int Register_pic_file_parser(P_Pic_File_Parser ptPicFileParser);
void Show_pic_fmts(void);
P_Pic_File_Parser Parser(char *pcName);
P_Pic_File_Parser Get_parser(P_File_Map ptFileMap);
int Pic_fmts_init(void);

int BMP_parser_init(void);
int JPG_parser_init(void);
#endif //_PICFMT_MANAGER_H
