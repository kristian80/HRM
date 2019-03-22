#pragma once
#include "HRM.h"
//#include "MyIvyConfiguration.h"
#include "HRMImguiWidget.h"
#include "HRM_Mission.h"





class HRM_PlugIn
{
public:

	std::string m_ds = "";

	std::string m_system_path = "";
	std::string m_scenery_file = "";
	int m_scenery_number = 1;

	std::shared_ptr<HRMImguiWidget> imguiPtr;

	XPLMObjectRef m_obj_ref = NULL;
	XPLMInstanceRef m_inst_ref = NULL;

	//HRM_Object *p_myobject = NULL;

	std::vector<HRM_Mission *> m_street_missions;
	std::vector<HRM_Mission *> m_urban_missions;
	std::vector<HRM_Mission *> m_sar_missions;
	std::vector<HRM_Mission *> m_sling_missions;

	HRM_Mission *mp_current_mission = NULL;

	

	bool m_initialized = false;
	int m_plugin_enabled = 0;
	int m_aircraft_loaded = 0;

	float m_data_rate = 0.1;

	//MyIvyConfiguration *m_ivyConfig = NULL;
	
	int m_PluginMenu = 0;
	XPLMMenuID m_PluginMenuID = 0;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// Mission Settings

	// Saved

	float m_cm_collective_min = 0.1;

	int m_cm_mission_type = 0;

	bool m_cm_use_acr_position = true;
	bool m_cm_use_airport = false;
	std::string m_cm_dep_airport = "";
	bool m_cm_enable_fse = true;

	int m_cm_min_distance = 0;
	int m_cm_max_distance = 15;

	bool m_cm_estmimated_wp = true;
	int m_cm_estimated_radius_m = 500;

	std::string m_cm_arr_airport = "";

	int m_difficutly = 0;
	float m_position_calc_rate = 0.5;

	// Not Saved

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// Mission Status

	
	float m_mission_time = 0;
	float m_mission_delta = 0;

	float m_mission_preflight_time = 0;
	float m_mission_flight1_time = 0;
	float m_mission_flight2_time = 0;




	//////////////////////////////////////////////////////////////////////////////////////////////////
	// DATAREFS

	XPLMDataRef m_d_latitude;
	XPLMDataRef m_d_longitude;
	XPLMDataRef m_f_heading;


	XPLMDataRef m_i_paused;
	XPLMDataRef m_i_replay;

	XPLMDataRef m_i_fse_flying;

	XPLMDataRef m_f_weight_max;
	XPLMDataRef m_f_payload;
	XPLMDataRef m_f_weight_total;

	XPLMDataRef m_i_on_ground;
	XPLMDataRef m_fa_engines_running;
	XPLMDataRef m_f_park_brake;

	XPLMDataRef m_f_climb_rate;

	XPLMDataRef m_i_sim_ground_speed;

	XPLMDataRef m_i_battery_on;

	XPLMDataRef m_fa_prop_ratio;

	XPLMDataRef m_f_pitch;
	XPLMDataRef m_f_roll;

	/////////////////////////////////////////////////////////////////////////////////
	// Dataref Variables

	double m_ld_latitude;
	double m_ld_longitude;
	float m_lf_heading;

	int m_i_paused;
	int m_i_replay;

	int m_i_fse_flying;

	float m_f_weight_max;
	float m_f_payload;
	float m_f_weight_total;

	int m_i_on_ground;
	float m_fa_engines_running[20];
	float m_f_park_brake;

	float m_f_climb_rate;

	float m_i_sim_ground_speed;

	int m_i_battery_on;

	float m_fa_prop_ratio[20];

	float m_f_pitch;
	float m_f_roll;

public:
	HRM_PlugIn();
	~HRM_PlugIn();

	void PluginStart();
	void PluginStop();
	void PluginEnable();
	void PluginDisable();

	void UpdatePosition();


	void PluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam);
	//void IvyDrawOutputWindow(XPLMWindowID in_window_id, void * in_refcon);
	void PluginMenuHandler(void * in_menu_ref, void * in_item_ref);

	void SaveMissions();
	void ReadMissions();

	void PluginKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey, void * inRefcon, int losingFocus);
	int PluginMouseClickCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void * inRefcon);

	float PluginFlightLoopCallback(float elapsedMe, float elapsedSim, int counter, void * refcon);
};

