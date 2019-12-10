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
#include "HRM.h"
//#include "MyIvyConfiguration.h"
#include "HRMImguiWidget.h"
#include "HRM_Mission.h"


#define HRM_MESSAGE_SOUND_1 0x85747400


class HRM_PlugIn
{
public:

	std::string m_ds = "";

	std::string m_system_path = "";
	std::string m_scenery_file = "";
	std::string m_config_path = "";
	std::string m_fms_file = "";
	std::string m_gfp_path = "";
	std::string m_gfp_file = "hrm.gfp";
	std::string m_wpt_path = "";
	std::string m_wpt_file = "user.wpt";

	XPLMCommandRef myCmdRefToggleControlWindow = NULL;


	XPLMPluginID m_ivy_id = XPLM_NO_PLUGIN_ID;

	//int m_scenery_number = 1;

	std::shared_ptr<HRMImguiWidget> imguiPtr;
	bool m_window_visible = false;

	std::vector<HRM_Airport *> m_fse_airports;
	std::vector<HRM_Airport *> m_custom_hospitals;

	std::vector<HRM_Mission *> m_street_missions;
	std::vector<HRM_Mission *> m_urban_missions;
	std::vector<HRM_Mission *> m_sar_missions;
	std::vector<HRM_Mission *> m_sling_missions;

	std::vector<HRM_Mission*> m_street_fire_missions;
	std::vector<HRM_Mission*> m_urban_fire_missions;
	std::vector<HRM_Mission*> m_sar_fire_missions;
	std::vector<HRM_Mission*> m_sling_fire_missions;

	std::vector<HRM_Waypoint *> m_street_waypoints;
	std::vector<HRM_Waypoint *> m_urban_waypoints;
	std::vector<HRM_Waypoint *> m_sar_waypoints;
	std::vector<HRM_Waypoint *> m_sling_waypoints;

	std::vector<std::string> m_path_vector;
	std::string m_global_path;

	std::string m_scenery_name = "";
	
	int m_scenery_number = 0;
	int m_scenery_count = 1;
	std::vector<std::string> m_scenery_names;

	int m_global_path_index = 0;

	int m_processing_time = 0;

	bool m_street_enable = true;
	bool m_urban_enable = false;
	bool m_sar_enable = false;
	bool m_sling_enable = false;
	bool m_fire_enable = false;


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

	XPLMPluginID m_HSL_id = XPLM_NO_PLUGIN_ID;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// Mission Settings

	// Saved

	int m_flight_plan_format = HRM::FPL_XP11;
	float m_cm_collective_min = 0.1f;
	int m_cm_airac_cycle = 1907;

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

	bool m_cm_estmimated_wp = false;
	int m_cm_estimated_radius_m = 500;

	float m_cm_sling_say_distance = 100;
	float m_cm_sling_say_time = 3.5;

	std::string m_cm_hospital_icao = "";
	std::string m_cm_departure_icao = "";

	int m_difficutly = HRM::Normal;
	float m_position_calc_rate = 0.5;
	float m_patient_countdown_value = 30;
	float m_hospital_countdown_value = 10;

	int m_sling_load_plugin = HRM::HSL;
	float m_sling_load_distance = 3.0f;
	float m_sling_load_time_min = 1.0f;

	float m_HSL_treshold = 50;
	float m_HSL_weight_empty = 15;
	float m_HSL_weight_full = 100;
	float m_HSL_size_empty = 0;
	float m_HSL_size_full = 15;

	float m_HSL_offset = 0.75;

	bool m_course_limit_enable = false;
	int m_course_limit_start = 0;
	int m_course_limit_stop = 360;

	

	std::string m_HSL_object_path = "./Custom Scenery/CDB-Library/Flora_Fauna/Birds_pets/fauna_birds_bird7.obj";

	
	

	// Not Saved

	bool m_cm_creation_failed = false;
	bool m_cm_no_waypoint_found = false;
	bool m_cm_cancelling = false;
	bool m_cm_not_on_ground = false;
	bool m_custom_icao_exists = false;
	bool m_HSL_not_found = false;
	bool m_cm_patient_sight_said = false;

	
	
	float m_cm_say_timer = 0;
	int m_cm_say_state = 0;
	float m_cm_distance_h_max = 0;
	
	bool m_HSL_found = false;
	bool m_HSL_reload_position_file = false;


	

	// 412 SAR
	bool m_412sar_found = false;
	bool m_412sar_not_found = false;
	XPLMCommandRef m_patient_off = NULL;
	XPLMCommandRef m_patient_ground = NULL;
	XPLMCommandRef m_patient_transit = NULL;
	XPLMCommandRef m_patient_board = NULL;
	XPLMCommandRef m_operator_on = NULL;
	XPLMCommandRef m_operator_off = NULL;

	XPLMDataRef m_f_412_hook_x = NULL;
	XPLMDataRef m_f_412_hook_y = NULL;
	XPLMDataRef m_f_412_hook_z = NULL;
	XPLMDataRef m_f_412_hook_cable_extended = NULL;
	XPLMDataRef m_f_412_hook_cable_direction = NULL;

	XPLMDataRef m_f_412_ext_lat = NULL;
	XPLMDataRef m_f_412_ext_long = NULL;
	XPLMDataRef m_i_412_ext_set = NULL;

	// HSL

	HRM_Object* mp_HSL_slingload_object = NULL;

	XPLMCommandRef m_HSL_enable = NULL;
	XPLMCommandRef m_HSL_disable = NULL;
	XPLMCommandRef m_HSL_connect = NULL;
	XPLMCommandRef m_HSL_release = NULL;
	XPLMCommandRef m_HSL_place_coordinates = NULL;
	XPLMCommandRef m_HSL_place_ground = NULL;
	XPLMCommandRef m_HSL_update_objects = NULL;

	XPLMCommandRef m_HSL_place_fire_coordinates = NULL;

	XPLMDataRef m_d_HSL_latitude = NULL;
	XPLMDataRef m_d_HSL_longitude = NULL;

	XPLMDataRef m_f_HSL_rope_length = NULL;

	XPLMDataRef m_i_HSL_cargo_is_bambi = NULL;
	XPLMDataRef m_i_HSL_cargo_connected = NULL;
	XPLMDataRef m_i_HSL_cargo_instanced_drawing = NULL;

	XPLMDataRef m_f_HSL_cargo_height = NULL;
	XPLMDataRef m_f_HSL_cargo_mass = NULL;
	XPLMDataRef m_f_HSL_cargo_friction_glide = NULL;
	XPLMDataRef m_f_HSL_cargo_friction_static = NULL;

	XPLMDataRef m_fa_HSL_cargo_vector_position = NULL;
	XPLMDataRef m_fa_HSL_cargo_vector_size = NULL;
	XPLMDataRef m_fa_HSL_cargo_vector_cw = NULL;
	XPLMDataRef m_fa_HSL_cargo_vector_hook_pos = NULL;
	XPLMDataRef m_fa_HSL_cargo_offset = NULL;

	XPLMDataRef m_ba_HSL_cargo_path = NULL;

	// HSL Fire

	XPLMDataRef m_f_HSL_fire_radius = NULL;
	XPLMDataRef m_f_HSL_fire_strength_start = NULL;
	XPLMDataRef m_f_HSL_fire_strength_max = NULL;
	XPLMDataRef m_f_HSL_fire_strength_inc = NULL;
	

	XPLMDataRef m_d_HSL_fire_set_lat = NULL;
	XPLMDataRef m_d_HSL_fire_set_lon = NULL;
	XPLMDataRef m_f_HSL_fire_set_elev = NULL;

	XPLMDataRef m_f_HSL_fire_count = NULL;
	XPLMDataRef m_fa_HSL_fire_strength = NULL;

	XPLMDataRef m_i_HSL_fire_create_failed = NULL;
	XPLMDataRef m_i_HSL_fire_update_positions = NULL;

	XPLMDataRef m_i_HSL_fire_remove = NULL;

	/*float m_lf_HSL_fire_radius = 0;
	float m_lf_HSL_fire_strength_start = 0;
	float m_lf_HSL_fire_strenght_max = 0;
	float m_lf_HSL_fire_strength_inc = 0;


	double m_ld_HSL_fire_set_lat = 0;
	double m_ld_HSL_fire_set_lon = 0;
	float m_lf_HSL_fire_set_elev = 0;*/

	float m_lf_HSL_fire_count = 0;
	float m_lfa_HSL_fire_strength[MAX_FIRES];

	int m_li_HSL_fire_create_failed = 0;

	float m_fire_time = 0;
	
	//int m_li_HSL_fire_update_positions = 0;

	/*
	/*strcpy(buffer, "RescueX/objects/Bergwacht_Luftrettungssack.obj");
	XPLMDataRef test = XPLMFindDataRef("HSL/RopeObjectPath");

	XPLMSetDatab(test, buffer, 0, 2048);*/

	int m_li_HSL_cargo_connected = 0;
	float m_lfa_HSL_cargo_vector_position[3] = { 0, 0, 0 };
	float m_lfa_HSL_cargo_vector_hook_pos[3] = { 0, 0, 0 };
	float m_lf_HSL_rope_length = 0;
	


	float m_lf_412_hook_x = NULL;
	float m_lf_412_hook_y = NULL;
	float m_lf_412_hook_z = NULL;
	float m_lf_412_hook_cable_extended = NULL;
	float m_lf_sling_load_hook_cable_direction = NULL;

	float m_lf_412_ext_lat = NULL;
	float m_lf_412_ext_long = NULL;
	int m_li_412_ext_set = NULL;

	double m_sling_load_meter_lat = 1;
	double m_sling_load_meter_long = 1;

	float m_sling_load_patient_distance = 0;
	float m_sling_load_patient_distance_side = 0;
	float m_sling_load_patient_distance_forward = 0;
	float m_sling_load_patient_distance_alt = 0;
	float m_sling_load_patient_heading = 0;

	double m_sling_load_patient_local_x = 0;
	double m_sling_load_patient_local_y = 0;
	double m_sling_load_patient_local_z = 0;

	double m_412_patient_lat = 0;
	double m_412_patient_long = 0;
	double m_412_patient_elev = 0;

	float m_sling_load_patient_loading_time = 0;

	float m_412_correct_lat = 0;
	float m_412_correct_long = 0;

	float m_412_correct_lat_m = 0;
	float m_412_correct_long_m = 0;

	bool m_412_crew_on = false;

	XPLMProbeRef m_412_probe = NULL;
	XPLMObjectRef m_412_obj_ref = NULL;
	XPLMInstanceRef m_412_inst_ref = NULL;

	HRM::AB412_Patient_Status_Type m_412_patient_status = HRM::Patient_Off;

	



	HRM_Waypoint *mp_cm_waypoint = NULL;
	HRM_Mission *mp_cm_mission = NULL;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// Mission Status

	HRM::Mission_State m_mission_state = HRM::State_Create_Mission;
	float m_mission_time = 0;
	bool m_mission_time_failed = false;

	bool m_mission_departure_icao_found = false;
	bool m_mission_scenario_icao_found = false;
	bool m_mission_hospital_icao_found = false;

	std::string m_mission_departure_icao_name = "";
	std::string m_mission_scenario_icao_name = "";
	std::string m_mission_hospital_icao_name = "";

	float m_mission_departure_lat = HRM::coord_invalid;
	float m_mission_departure_long = HRM::coord_invalid;

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

	XPLMDataRef m_s_acf_descrip;

	XPLMDataRef m_d_local_x;
	XPLMDataRef m_d_local_y;
	XPLMDataRef m_d_local_z;

	XPLMDataRef m_f_local_phi;
	XPLMDataRef m_f_local_psi;
	XPLMDataRef m_f_local_theta;
	

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
	XPLMDataRef m_ia_custom_slider_on;
	XPLMDataRef m_fa_custom_slider_ratio;

	XPLMDataRef m_f_climb_rate;

	XPLMDataRef m_i_sim_ground_speed;

	XPLMDataRef m_i_battery_on;

	XPLMDataRef m_fa_prop_ratio;

	XPLMDataRef m_f_pitch;
	XPLMDataRef m_f_roll;

	XPLMDataRef m_f_jett_weight;

	/////////////////////////////////////////////////////////////////////////////////
	// Dataref Variables

	std::string m_ls_acf_descrip = "";

	double m_ld_local_x;
	double m_ld_local_y;
	double m_ld_local_z;

	float m_lf_local_phi;
	float m_lf_local_psi;
	float m_lf_local_theta;

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
	int m_lia_custom_slider_on[24];
	float m_lfa_custom_slider_ratio[24];
	float m_lf_park_brake;

	float m_lf_climb_rate;

	int m_li_sim_ground_speed;

	int m_li_battery_on;

	float m_lfa_prop_ratio[1];

	float m_lf_pitch;
	float m_lf_roll;

	float m_lf_jett_weight = 0;
	float m_lf_jett_weight_old = 0;

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

	void ConfigureHSL();
	void ConfigureFire();
	void MissionCreate();
	bool FindWaypoint(std::vector<HRM_Waypoint*>* p_waypoint_vector, std::vector<HRM_Waypoint*> &global_waypoint_vector, std::vector<HRM_Waypoint*>& considered_waypoints);
	void MissionStart();
	void MissionStartFlight1();
	void MissionStartFlight2();
	void MissionCalcGFPoints();
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

	void Check412SAR();

	void IvyPlaySound(int sound_before, int say_value, int sound_after);
	


	void CreateFlightPlan();
	void ReadCustomICAOs();
	void SaveCustomICAOs();
	void AddCustomICAO();

	void ReadFSEAirports();
	void ReadCustomWaypoints(std::vector<HRM_Waypoint *> &waypoint_vector, std::string file_name);
	void ReadGlobalWaypoints(std::vector<HRM_Waypoint *> &waypoint_vector, std::string file_name);
	void ReadWaypointFile(std::vector<HRM_Waypoint *> &waypoint_vector, std::string file_name);
	bool CheckWaypointAngle(HRM_Waypoint& waypoint);
	
	void SaveConfig();
	void ReadConfig();
	
	void SaveMissions();
	void ReadMissions();
	void CountMissions();

	void ReadDataFast();
	void ReadDataSlow();

	void PluginKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey, void * inRefcon, int losingFocus);
	int PluginMouseClickCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void * inRefcon);

	float PluginFlightLoopCallback(float elapsedMe, float elapsedSim, int counter, void * refcon);
	int ToggleControlWindowCallback(XPLMCommandRef cmd, XPLMCommandPhase phase, void* refcon);
};

