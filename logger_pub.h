/**
 * @Author: Inbar Shalitin
 * @Creation Date: 16/03/2016
 * @Last Modification Data:	21/03/2016
 * @Description: header file for logger functions
 *****************************************************/
#ifndef __LOGGERPUB_H_
#define __LOGGERPUB_H_
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "ADTErr.h"
#define BUFSIZE 1024
#define PREBUFSIZE 64
#define MAX_LEVEL 8
#define LOGS_NUM 2
typedef struct Logger Logger;
extern char buff[];
extern char preBuff[];

enum e_LogLevel {		/* logging levels 				*/
    LOG_NONE = 0,	/* silent mode					*/
    LOG_ERR  = 1,    /* error     messages			*/
    LOG_WAR  = 2,    /* warning   messages			*/
    LOG_DEB  = 4,    /* debugging messages			*/
    LOG_RMG  = 8     /* resource manager messages	*/
};
typedef enum e_LogLevel e_LogLevel;

/** @brief: Create logfile   
	@param[in]: _modes: logging level mode  
	@param[in]: _logName: logger file name   
	@retval: ERR_OK if created
	@retval: ERR_NOT_INITIALIZED if _logName == NULL
	@retval: ERR_WRONG_INDEX if _mode was not enabled
	@retval: ERR_GENERAL if open log file has failed */
ADTErr LogCreate( e_LogLevel _modes, int index, const char* _logName);

/** @brief: Store log message  
	@param[in]: _mode: logging level mode  
	@param[in]: _preMessage: 
	@param[in]: _message: the message itself  
	@retval: none*/
void LogRegister (e_LogLevel _mode, int index, const char* _preMessage, const char* _message);

/** @brief: checks if mode was not enabled
	@param[in]: _mode: logging level mode  
	@retval: 0 if corresponding mode was not enabled, 1 otherwise*/
int TestMode(e_LogLevel _mode);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvariadic-macros"
#define LOG_ERR_PRINT(x, y, ...)\
                if(TestMode(LOG_ERR)) { \
                PRINT_NAMES \
                snprintf(buff,BUFSIZE,x,##__VA_ARGS__);\
                LogRegister (LOG_ERR, y, preBuff , buff );}
#define LOG_WARN_PRINT(x, y, ...)\
                if(TestMode(LOG_WAR)) { \
                PRINT_NAMES \
                snprintf(buff,BUFSIZE,x,##__VA_ARGS__);\
                LogRegister (LOG_WAR, y, preBuff , buff );}
#define LOG_DEBUG_PRINT(x, y, ...)\
                if(TestMode(LOG_DEB)) { \
                PRINT_NAMES \
                snprintf(buff,BUFSIZE,x,##__VA_ARGS__);\
                LogRegister (LOG_DEB, y, preBuff , buff );}
#define LOG_RMN_PRINT(x, y, ...)\
                if(TestMode(LOG_RNM)) { \
                PRINT_NAMES \
                snprintf(buff,BUFSIZE,x,##__VA_ARGS__);\
                LogRegister (LOG_RNM, y, preBuff , buff );}
#define PRINT_NAMES\
        snprintf(preBuff,BUFSIZE,"%s::%s[%d]",__FILE__, __FUNCTION__, __LINE__);
#pragma GCC diagnostic pop
#endif /* __LOGGER_H_ */
