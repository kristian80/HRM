#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <queue>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <set>

#include "XPLMDefs.h"
#include "XPLMProcessing.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"
#include "XPLMNavigation.h"
#include "XPLMPlugin.h"
#include "XPLMMenus.h"
#include "XPWidgetDefs.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"
#include "XPWidgetUtils.h"
#include "XPLMInstance.h"

#include "ImgWindow.h"
#include "imgui.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

// OS X: we use this to convert our file path.
#if APL
#include <Carbon/Carbon.h>
#endif

// Your include paths for OpenAL may vary by platform.
#include "al.h"
#include "alc.h"



//#define IVY_MAX_AIRCRAFT_CONFIG 100

//#define MAX_ERROR_HISTORY 100

//#define MAX_GRAPH_DATA 1000
//#define MAX_GRAPH_TIME 100

struct HRM_Waypoint
{
	double latitude;
	double longitude;
	double heading;
	double lat_heading;
	double long_heading;
};

struct HRM_Airport
{
	std::string icao;
	std::string name;
	double latitude;
	double longitude;
	
};

namespace HRM
{
	const int wp_code = 28;
	const int max_scenery = 100;

	const float patient_weight = 75;
	const float crew_weight = 225;
	const float ems_equippment_weight = 350;

	const float fse_min_park_brake = 0.5;

	const float coord_invalid = -10000;

	enum Mission_State
	{
		State_Create_Mission,
		State_Plan_Flight,
		State_Pre_Flight,
		State_Flight_1,
		State_At_Patient,
		State_Patient_Loaded,
		State_Flight_2,
		State_Mission_Finished,
		State_Mission_Cancelled
	};

	enum Difficulty
	{
		Easy,
		Normal,
		Hard
	};

	enum Scenario_Position
	{
		Scenairo_Aircraft,
		Scenario_ICAO
	};

	const float m_threshold_gf_low = 0;
	const float m_threshold_gf_med = 0;
	const float m_threshold_gf_high = 0;

	const float m_threshold_gs_low = 0;
	const float m_threshold_gs_med = 0;
	const float m_threshold_gs_high = 0;

	const float m_threshold_gv_low = 0;
	const float m_threshold_gv_med = 0;
	const float m_threshold_gv_high = 0;

}


class HRM_PlugIn;
extern HRM_PlugIn *pHRM;
extern std::ofstream hrm_output_file;

inline bool file_exists(const std::string& name) 
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

inline bool is_int(float f)
{
	if (f == ((int)f)) return true;
	return false;
}

inline void HRMDebugString(std::string output)
{
	hrm_output_file << output << std::endl;
	hrm_output_file.flush();
}

static void load_cb(const char * real_path, void * ref)
{
	XPLMObjectRef * dest = (XPLMObjectRef *)ref;
	if (*dest == NULL)
	{
		*dest = XPLMLoadObject(real_path);
	}
}

double calc_distance_m(double lat1, double long1, double lat2, double long2);
double calc_distance_nm(double lat1, double long1, double lat2, double long2);


int WrapVSpeedHandler(XPWidgetMessage  inMessage, XPWidgetID  inWidget, intptr_t inParam1, intptr_t inParam2);
int WrapLogbookHandler(XPWidgetMessage  inMessage, XPWidgetID  inWidget, intptr_t  inParam1, intptr_t  inParam2);
int WrapLogbookScrollHandler(XPWidgetMessage  inMessage, XPWidgetID  inWidget, intptr_t inParam1, intptr_t inParam2);


void WrapDrawOutputWindow(XPLMWindowID in_window_id, void * in_refcon);
void WrapMenuHandler(void * in_menu_ref, void * in_item_ref);
int WrapSayBaroCallback(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon);
int WrapSayWindCallback(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon);
int WrapAnnouncementCallback(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon);
int WrapResetHRMCallback(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon);
int WrapToogleWindowCallback(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon);

void WrapKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey, void * inRefcon, int losingFocus);
int WrapMouseClickCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void * inRefcon);

PLUGIN_API float WrapFlightLoopCallback(float elapsedMe, float elapsedSim, int counter, void * refcon);