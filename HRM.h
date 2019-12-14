/*
 * This file is part of the HRM distribution (https://github.com/kristian80/HRM).
 * Copyright (c) 2019 Kristian80.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#define NOMINMAX
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
#include <algorithm>

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
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string/replace.hpp>

// OS X: we use this to convert our file path.
#if APL
#include <Carbon/Carbon.h>
#endif

// Your include paths for OpenAL may vary by platform.
//#include "al.h"
//#include "alc.h"



//#define IVY_MAX_AIRCRAFT_CONFIG 100

//#define MAX_ERROR_HISTORY 100

//#define MAX_GRAPH_DATA 1000
//#define MAX_GRAPH_TIME 100

#define MAX_FIRES 18

struct HRM_Sound
{
	int sound_before = -1;
	int say_value = -1;
	int sound_after = -1;
};

struct HRM_Waypoint
{
	double latitude = 0;
	double longitude = 0;
	double heading = 0;
	double lat_heading = 0;
	double long_heading = 0;
};

struct HRM_Airport
{
	std::string icao = "";
	std::string name = "";
	double latitude = 0;
	double longitude = 0;
	
};

namespace HRM
{
	const int wp_code = 28;
	const int max_scenery = 100;

	

	const int pickup_max_distance = 100;
	const int hospital_max_distance = 500;

	const float fse_min_park_brake = 0.5;

	const float coord_invalid = -10000;

	const int type_sling = 3;

	enum Slingload_Type
	{
		HSL,
		AB412
	};
	enum FlightPlan_Type
	{
		FPL_XP11,
		FPL_XP10,
		FPL_GTN
	};

	enum AB412_Patient_Status_Type
	{
		Patient_Ground,
		Patient_Transit,
		Patient_OnBoard,
		Patient_Off
	};

	enum Mission_State
	{
		State_Create_Mission,
		State_Plan_Flight,
		State_Pre_Flight,
		State_Flight_1,
		State_At_Patient,
		State_Flight_2,
		State_At_Hospital,
		State_Mission_Finished,
		State_Mission_Cancelled,
		State_Fire_Fighting,
		State_Fire_Extinguished
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

	const float preflight_time_easy = 300;
	const float preflight_time_normal = 150;
	const float preflight_time_hard = 90;

	const float flight_time_up_down_easy = 300;
	const float flight_time_up_down_normal = 90;
	const float flight_time_up_down_hard = 45;

	const float flight_time_per_nm_easy = 60;
	const float flight_time_per_nm_normal = 45;
	const float flight_time_per_nm_hard = 30;

	const float flight_time_search_easy = 1800;
	const float flight_time_search_normal = 600;
	const float flight_time_search_hard = 150;

	const float flight_time_sling_easy = 600;
	const float flight_time_sling_normal = 300;
	const float flight_time_sling_hard = 150;

	//const float threshold_g_mult_easy = 1;
	//const float threshold_g_mult_normal = 1;
	//const float threshold_g_mult_hard = 1;

	//const float threshold_g_mult_flight1 = 1.2;

	const float threshold_g_mult_flight2 = 1.0f;

	const float threshold_gf_low = 0.2f;
	const float threshold_gf_med = 0.35f;
	const float threshold_gf_high = 0.5f;

	const float threshold_gs_low = 0.15f;
	const float threshold_gs_med = 0.35f;
	const float threshold_gs_high = 0.5f;

	const float threshold_gv_pos_low = 1.3f;
	const float threshold_gv_pos_med = 1.5f;
	const float threshold_gv_pos_high = 1.8f;

	const float threshold_gv_neg_low = 0.7f;
	const float threshold_gv_neg_med = 0.5f;
	const float threshold_gv_neg_high = 0.3f;

	const int points_difficulty = 10;
	const int points_speed_flight1 = 20;
	const int points_speed_flight2 = 20;
	const int points_g_flight2 = 40;
	const int points_search_range = 10;

	const int points_easy = 0;
	const int points_normal = 5;
	const int points_hard = 10;

	const float eval_g_total_factor = 10;

	const float eval_g_low_factor = 1;
	const float eval_g_med_factor = 10;
	const float eval_g_high_factor = 30;

	const float eval_flight1_nominal_speed = 120;
	const float eval_flight2_nominal_speed = 120;
	const float eval_flight2_sling_nominal_speed = 50;

	const int search_range_min = 500;
	const int search_range_max = 5000;

	const float hsl_rope_length_min = 0.5;

	const float fire_radius = 3.0f;
	
	const float fire_strength_start_easy = 1.0f;
	const float fire_strength_start_normal = 50.0f;
	const float fire_strength_start_hard = 250.0f;

	const float fire_strength_inc_easy = 0.0f;
	const float fire_strength_inc_normal = 0.0f;
	const float fire_strength_inc_hard = 0.5f;

	const float fire_strength_max_easy = 1.0f;
	const float fire_strength_max_normal = 50.0f;
	const float fire_strength_max_hard = 500.0f;



	


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

inline void SetVectorDataRef(XPLMDataRef& dataref, float &x, float &y, float &z)
{
	float values[3];
	values[0] = x;
	values[1] = y;
	values[2] = z;
	XPLMSetDatavf(dataref, values, 0, 3);
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
int WrapToggleControlWindowCallback(XPLMCommandRef cmd, XPLMCommandPhase phase, void* refcon);

void WrapKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey, void * inRefcon, int losingFocus);
int WrapMouseClickCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void * inRefcon);

PLUGIN_API float WrapFlightLoopCallback(float elapsedMe, float elapsedSim, int counter, void * refcon);