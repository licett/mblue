#ifndef LOG_SERVICE_H
#define LOG_SERVICE_H

#include "mblue_segment.h"
#include "mblue_macro.h"

#define LOG_SERVICE    mblue_log_service
struct LOG_SERVICE
{
	struct mblue_segment ms_entity;
};

DECLARE_SERVICE_INCLUDE(LOG_SERVICE);

#endif