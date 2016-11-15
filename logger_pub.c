/** @author: Inbar Shalitin
 *  @date of creation: 16/03/2016
 *  @updated: 21/03/2016
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "logger_pub.h"

char preBuff[PREBUFSIZE] = {0};
char buff[BUFSIZE] = {0};

struct Logger
{
	e_LogLevel m_mode;
	FILE* m_logFile;
};
Logger g_log[LOGS_NUM];
int TestMode(e_LogLevel _mode)
{
 	if (_mode < 0 || _mode > MAX_LEVEL)
 	{
 		return 0;
 	}
 	return 1;
}

ADTErr LogCreate(e_LogLevel _modes, int index, const char* _logName)
{
	if(NULL == _logName) 
 	{
 		return ERR_NOT_INITIALIZED;
 	}
 	if (!TestMode(_modes))
 	{
 		return ERR_WRONG_INDEX;
 	}
 	if(NULL != g_log[index].m_logFile)
	{
		fclose(g_log[index].m_logFile);
		g_log[index].m_mode =  LOG_NONE;
	}
 	g_log[index].m_logFile = fopen(_logName, "w");
 	if( NULL == g_log[index].m_logFile)
 	{
 		return ERR_GENERAL;
 	}
 	g_log[index].m_mode = _modes;
 	return ERR_OK;
}

void LogRegister (e_LogLevel _mode, int index, const char* _preMessage, const char* _message)
{
	time_t rawtime;
	struct tm * timeinfo;
	if(NULL == g_log[index].m_logFile || NULL == _message || NULL == _preMessage || _mode <= 0 || _mode > g_log[index].m_mode || !TestMode(_mode)) 
	{
		return;
	}
	switch (_mode){
	case     LOG_NONE:
		fprintf(g_log[index].m_logFile, "%s  NONE   %s\n", _preMessage, _message);
		fflush(g_log[index].m_logFile);
		break;
	case     LOG_ERR:
		fprintf(g_log[index].m_logFile, "%s   ERROR  %s\n", _preMessage, _message);
		fflush(g_log[index].m_logFile);
		break;
	case     LOG_WAR:
		fprintf(g_log[index].m_logFile, "%s  WARNING   %s\n", _preMessage, _message);
		fflush(g_log[index].m_logFile);
		break;
	case     LOG_DEB: 
		fprintf(g_log[index].m_logFile, "%s   DEBUG  %s\n", _preMessage, _message);
		fflush(g_log[index].m_logFile);
		break;
	case     LOG_RMG:
		fprintf(g_log[index].m_logFile, "%s   RMG  %s\n", _preMessage, _message);
		fflush(g_log[index].m_logFile);
		break;
	}
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	fprintf ( g_log[index].m_logFile, "[%s]", asctime (timeinfo) );
	fflush(g_log[index].m_logFile);
}

