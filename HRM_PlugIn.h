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
	std::string m_config_path = "";
	//int m_scenery_number = 1;

	std::shared_ptr<HRMImguiWidget> imguiPtr;

	std::vector<HRM_Airport *> m_fse_airports;

	std::vector<HRM_Mission *> m_street_missions;
	std::vector<HRM_Mission *> m_urban_missions;
	std::vector<HRM_Mission *> m_sar_missions;
	std::vector<HRM_Mission *> m_sling_missions;

	std::vector<HRM_Waypoint *> m_street_waypoints;
	std::vector<HRM_Waypoint *> m_urban_waypoints;
	std::vector<HRM_Waypoint *> m_sar_waypoints;
	std::vector<HRM_Waypoint *> m_sling_waypoints;

	bool m_street_enable = true;
	bool m_urban_enable = false;
	bool m_sar_enable = false;
	bool m_sling_enable = false;


	//HRM_Mission *mp_current_mission = NULL;

	

	bool m_initialized = false;
	int m_plugin_enabled = 0;
	int m_aircraft_loaded = 0;

	float m_data_rate = -1;

	//MyIvyConfiguration *m_ivyConfig = NULL;
	
	int m_PluginMenu = 0;
	XPLMMenuID m_PluginMenuID = 0;

	float m_time_delta = 0;

	std::string m_custom_icao = "";

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// Mission Settings

	// Saved

	float m_cm_collective_min = 0.1;

	int m_cm_mission_type = 0;

	HRM::Scenario_Position m_cm_use_position = HRM::Scenairo_Aircraft;
	bool m_cm_use_airport = false;
	std::string m_cm_scenario_icao = "";
	bool m_cm_enable_fse = false;

	int m_cm_min_distance = 0;
	int m_cm_max_distance = 15;

	bool m_cm_estmimated_wp = true;
	int m_cm_estimated_radius_m = 500;

	std::string m_cm_hospital_icao = "";

	int m_difficutly = HRM::Normal;
	float m_position_calc_rate = 0.5;

	

	// Not Saved

	bool m_cm_creation_failed = false;
	bool m_cm_no_waypoint_found = false;

	HRM_Waypoint *mp_cm_waypoint = NULL;
	HRM_Mission *mp_cm_mission = NULL;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// Mission Status

	HRM::Mission_State m_mission_state = HRM::State_Create_Mission;
	float m_mission_time = 0;

	bool m_mission_scenario_icao_found = false;
	bool m_mission_hospital_icao_found = false;

	std::string m_mission_scenario_icao_name = "";
	std::string m_mission_hospital_icao_name = "";

	float m_mission_scenario_lat = HRM::coord_invalid;
	float m_mission_scenario_long = HRM::coord_invalid;

	float m_mission_hospital_lat = HRM::coord_invalid;
	float m_mission_hospital_long = HRM::coord_invalid;

	bool m_custom_hospital_icao_found = true;
	bool m_custom_hospital_icao_empty = false;
	

	float m_mission_preflight_time = 0;
	float m_mission_flight1_time = 0;
	float m_mission_flight2_time = 0;

	float m_mission_gf_low = 0;
	float m_mission_gf_med = 0;
	float m_mission_gf_high = 0;

	float m_mission_gs_low = 0;
	float m_mission_gs_med = 0;
	float m_mission_gs_high = 0;

	float m_mission_gv_low = 0;
	float m_mission_gv_med = 0;
	float m_mission_gv_high = 0;




	//////////////////////////////////////////////////////////////////////////////////////////////////
	// DATAREFS

	XPLMDataRef m_d_latitude;
	XPLMDataRef m_d_longitude;
	XPLMDataRef m_f_heading;

	XPLMDataRef m_f_g_normal;
	XPLMDataRef m_f_g_forward;
	XPLMDataRef m_f_g_side;


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

	float m_lf_g_normal;
	float m_lf_g_forward;
	float m_lf_g_side;

	int m_li_paused;
	int m_li_replay;

	int m_li_fse_flying;

	float m_lf_weight_max;
	float m_lf_payload;
	float m_lf_weight_total;

	int m_li_on_ground;
	float m_lfa_engines_running[2];
	float m_lf_park_brake;

	float m_lf_climb_rate;

	float m_li_sim_ground_speed;

	int m_li_battery_on;

	float m_lfa_prop_ratio[1];

	float m_lf_pitch;
	float m_lf_roll;

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

	void MissionCreate();
	void MissionStart();
	void MissionReset();
	void MissionCancel();

	void ReadFSEAirports();
	void ReadWaypoints(std::vector<HRM_Waypoint *> &waypoint_vector, std::string file_name);

	void SaveMissions();
	void ReadMissions();

	void ReadDataFast();
	void ReadDataSlow();

	void PluginKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey, void * inRefcon, int losingFocus);
	int PluginMouseClickCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void * inRefcon);

	float PluginFlightLoopCallback(float elapsedMe, float elapsedSim, int counter, void * refcon);
};

