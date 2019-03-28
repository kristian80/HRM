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
	std::string m_fms_file = "";
	//int m_scenery_number = 1;

	std::shared_ptr<HRMImguiWidget> imguiPtr;

	std::vector<HRM_Airport *> m_fse_airports;
	std::vector<HRM_Airport *> m_custom_hospitals;

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
	int m_cm_airac_cycle = 1809;

	float m_patient_weight = 75;
	float m_crew_weight = 225;
	float m_ems_equippment_weight = 350;

	bool m_adjust_payload = false;

	HRM::Scenario_Position m_cm_use_position = HRM::Scenairo_Aircraft;
	//bool m_cm_use_airport = false;
	std::string m_cm_scenario_icao = "";
	bool m_cm_enable_fse = false;
	bool m_cm_autoconnect_fse = false;
	double m_cm_fse_airport_radius = 200;

	int m_cm_min_distance = 0;
	int m_cm_max_distance = 15;

	bool m_cm_estmimated_wp = true;
	int m_cm_estimated_radius_m = 500;

	std::string m_cm_hospital_icao = "";

	int m_difficutly = HRM::Normal;
	float m_position_calc_rate = 0.5;
	float m_patient_countdown_value = 30;
	float m_hospital_countdown_value = 10;

	

	// Not Saved

	bool m_cm_creation_failed = false;
	bool m_cm_no_waypoint_found = false;
	bool m_cm_cancelling = false;
	bool m_cm_not_on_ground = false;
	bool m_custom_icao_exists = false;

	HRM_Waypoint *mp_cm_waypoint = NULL;
	HRM_Mission *mp_cm_mission = NULL;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// Mission Status

	HRM::Mission_State m_mission_state = HRM::State_Create_Mission;
	float m_mission_time = 0;
	bool m_mission_time_failed = false;

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
	

	float m_mission_preflight_countdown = 0;
	float m_mission_flight1_countdown = 0;
	float m_mission_flight2_countdown = 0;
	float m_mission_at_patient_countdown = 0;
	float m_mission_at_hospital_countdown = 0;

	float m_mission_flight1_time = 0;
	float m_mission_flight2_time = 0;

	float m_mission_flight1_distance = 0;
	float m_mission_flight2_distance = 0;

	int m_mission_flight1_avg_speed = 0;
	int m_mission_flight2_avg_speed = 0;




	float m_mission_gf_low = 0;
	float m_mission_gf_med = 0;
	float m_mission_gf_high = 0;

	float m_mission_gs_low = 0;
	float m_mission_gs_med = 0;
	float m_mission_gs_high = 0;

	float m_mission_gv_pos_low = 0;
	float m_mission_gv_pos_med = 0;
	float m_mission_gv_pos_high = 0;

	float m_mission_gv_neg_low = 0;
	float m_mission_gv_neg_med = 0;
	float m_mission_gv_neg_high = 0;

	int m_mission_points_total = 0;
	int m_mission_points_search_range = 0;
	int m_mission_points_flight1 = 0;
	int m_mission_points_flight2 = 0;
	int m_mission_points_g_force = 0;
	int m_mission_points_difficulty = 0;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// FSEconomy

	XPLMDataRef m_fse_dr_is_connected = NULL;
	XPLMDataRef m_fse_dr_is_flying = NULL;
	XPLMDataRef m_fse_dr_can_end_flight = NULL;

	int m_fse_li_is_connected = 0;
	int m_fse_li_is_flying = 0;
	int m_fse_li_can_end_flight = 0;

	XPLMCommandRef m_fse_command_login = NULL;
	XPLMCommandRef m_fse_command_start = NULL;
	XPLMCommandRef m_fse_command_end = NULL;


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
	XPLMDataRef m_i_vr_enabled;

	XPLMDataRef m_i_fse_flying;

	XPLMDataRef m_f_weight_max;
	XPLMDataRef m_f_payload;
	XPLMDataRef m_f_weight_total;

	XPLMDataRef m_i_on_ground;
	XPLMDataRef m_ia_engines_running;
	XPLMDataRef m_f_park_brake;

	XPLMDataRef m_f_climb_rate;

	XPLMDataRef m_i_sim_ground_speed;

	XPLMDataRef m_i_battery_on;

	XPLMDataRef m_fa_prop_ratio;

	XPLMDataRef m_f_pitch;
	XPLMDataRef m_f_roll;

	XPLMDataRef m_i_jett_is_slung;

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
	int m_li_vr_enabled;

	int m_li_fse_flying;

	float m_lf_weight_max;
	float m_lf_payload;
	float m_lf_weight_total;

	int m_li_on_ground;
	int m_lia_engines_running[2];
	float m_lf_park_brake;

	float m_lf_climb_rate;

	float m_li_sim_ground_speed;

	int m_li_battery_on;

	float m_lfa_prop_ratio[1];

	float m_lf_pitch;
	float m_lf_roll;

	int m_li_jett_is_slung;
	int m_li_jett_is_slung_old;

public:
	HRM_PlugIn();
	~HRM_PlugIn();

	static std::string CreateTimeString(float time_float);

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
	void MissionStartFlight1();
	void MissionStartFlight2();
	void MissionFinish();
	void MissionReset();
	void MissionCancel();

	void FSERegister();
	void FSELogin();
	bool FSEIsConnected();
	void FSEStartFlight();
	bool FSEIsFlying();
	bool FSECanFinish();
	void FSEFinishFlight();


	void CreateFlightPlan();
	void ReadCustomICAOs();
	void SaveCustomICAOs();
	void AddCustomICAO();

	void ReadFSEAirports();
	void ReadWaypoints(std::vector<HRM_Waypoint *> &waypoint_vector, std::string file_name);
	
	void SaveConfig();
	void ReadConfig();
	
	void SaveMissions();
	void ReadMissions();

	void ReadDataFast();
	void ReadDataSlow();

	void PluginKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey, void * inRefcon, int losingFocus);
	int PluginMouseClickCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void * inRefcon);

	float PluginFlightLoopCallback(float elapsedMe, float elapsedSim, int counter, void * refcon);
};

