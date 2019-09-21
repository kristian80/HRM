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

#include "HRM_PlugIn.h"
#include <chrono>


HRM_PlugIn::HRM_PlugIn() :
	m_street_missions(),
	m_urban_missions(),
	m_sar_missions(),
	m_sling_missions(),
	m_street_waypoints(),
	m_urban_waypoints(),
	m_sar_waypoints(),
	m_sling_waypoints(),
	m_path_vector(),
	m_fse_airports()

{

	m_path_vector.push_back("BaseMesh");
	m_path_vector.push_back("Ortho4XP");
	m_path_vector.push_back("ZonePhoto");
	m_path_vector.push_back("Forkboy");
	m_path_vector.push_back("ORBX");

	m_global_path = m_path_vector[0];
	m_global_path_index = 0;
	
}


HRM_PlugIn::~HRM_PlugIn()
{
	
}

std::string HRM_PlugIn::CreateTimeString(float time_float)
{
	int time_int = (int) time_float;

	int seconds = time_int % 60;
	time_int -= seconds;

	int minutes = time_int / 60;

	std::string time_string = "";
	if (minutes < 10) time_string += "0";
	time_string += std::to_string(minutes) + ":";

	if (seconds < 10) time_string += "0";
	time_string += std::to_string(seconds);

	return time_string;
}



void HRM_PlugIn::PluginStart()
{
	

	m_ds = XPLMGetDirectorySeparator();

	char buffer[2048];
	XPLMGetSystemPath(buffer);
	m_system_path = buffer;

	
	m_config_path = m_system_path +  "Resources" + m_ds + "plugins" + m_ds + "HRM" + m_ds;
	m_scenery_file = m_config_path + "scenery";

	m_fms_file = m_system_path + "Output" + m_ds + "FMS plans" + m_ds + "HRM.fms";

	m_xslingload_ini_path = m_system_path + "Resources" + m_ds + "plugins" + m_ds + "xslingload" + m_ds + "xslingload.ini";
	m_xslingload_apt_path = m_system_path + "Resources" + m_ds + "plugins" + m_ds + "xslingload" + m_ds + "apt.dat";

	m_gfp_path = "C:\\ProgramData\\Garmin\\Trainers\\Databases\\FPLN";
	m_wpt_path = "C:\\ProgramData\\Garmin\\Trainers\\Databases\\FPLN";


	srand(time(NULL));

	ReadConfig();

	

	XPLMRegisterFlightLoopCallback(WrapFlightLoopCallback, 0.1f, 0);

	// Menu;
	m_PluginMenu = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "HRM", 0, 1);
	m_PluginMenuID = XPLMCreateMenu("HRM", XPLMFindPluginsMenu(), m_PluginMenu, WrapMenuHandler, 0);
	XPLMAppendMenuItem(m_PluginMenuID, "Toggle Control Window", (void *) "Item1", 1);

	myCmdRefToggleControlWindow = XPLMCreateCommand("HRM/ToogleControlWindow", "Toggle Control Window");
	XPLMRegisterCommandHandler(myCmdRefToggleControlWindow, WrapToggleControlWindowCallback, 0, 0);

	int left, top, right, bot;
	XPLMGetScreenBoundsGlobal(&left, &top, &right, &bot);
	

	int width = 500;
	int height = 500;
	int left_pad = 175;
	int top_pad = 75;
	int x = left + left_pad;
	int y = top - top_pad;
	// WindowDecoration decorate
	// WindowDecorationNone = 0
	// WindowDecorationRoundRectangle = 1
	// WindowDecorationSelfDecorated = 2
	// WindowDecorationSelfDecoratedResizable = 3
	int decorate = 1;
	imguiPtr = std::make_shared<HRMImguiWidget>(this, 
		                                        300, 
												550, 
												300 + 430, 
												550 - 500, 
												decorate);  // calls constructor
	//imguiPtr-> // calls ImgWindow::init from the base class which in turn virtually calls the overridden function
	
	//XPLMCreateProbe()

	imguiPtr->configureImguiContext();

	HRMDebugString("Lookup Datarefs");

	m_s_acf_descrip = XPLMFindDataRef("sim/aircraft/view/acf_descrip");

	m_d_local_x = XPLMFindDataRef("sim/flightmodel/position/local_x");
	m_d_local_y = XPLMFindDataRef("sim/flightmodel/position/local_y");
	m_d_local_z = XPLMFindDataRef("sim/flightmodel/position/local_z");

	m_f_local_phi = XPLMFindDataRef("sim/flightmodel/position/phi");
	m_f_local_psi = XPLMFindDataRef("sim/flightmodel/position/psi");
	m_f_local_theta = XPLMFindDataRef("sim/flightmodel/position/theta");

	m_d_latitude = XPLMFindDataRef("sim/flightmodel/position/latitude");
	m_d_longitude = XPLMFindDataRef("sim/flightmodel/position/longitude");
	m_f_heading = XPLMFindDataRef("sim/flightmodel/position/true_psi");

	m_f_g_normal = XPLMFindDataRef("sim/flightmodel/forces/g_nrml");
	m_f_g_forward = XPLMFindDataRef("sim/flightmodel/forces/g_axil");
	m_f_g_side = XPLMFindDataRef("sim/flightmodel/forces/g_side");

	m_i_paused = XPLMFindDataRef("sim/time/paused");
	m_i_replay = XPLMFindDataRef("sim/operation/prefs/replay_mode");
	m_i_vr_enabled = XPLMFindDataRef("sim/graphics/VR/enabled");

	m_i_fse_flying = XPLMFindDataRef("fse/status/flying");
	
	m_f_weight_max = XPLMFindDataRef("sim/aircraft/weight/acf_m_max");
	m_f_payload = XPLMFindDataRef("sim/flightmodel/weight/m_fixed");
	m_f_weight_total = XPLMFindDataRef("sim/flightmodel/weight/m_total");

	m_i_on_ground = XPLMFindDataRef("sim/flightmodel/failures/onground_any");
	m_ia_engines_running = XPLMFindDataRef("sim/flightmodel/engine/ENGN_running");
	m_f_park_brake = XPLMFindDataRef("sim/cockpit2/controls/parking_brake_ratio");

	m_ia_custom_slider_on = XPLMFindDataRef("sim/cockpit2/switches/custom_slider_on");
	m_fa_custom_slider_ratio = XPLMFindDataRef("sim/flightmodel2/misc/custom_slider_ratio");

	m_f_climb_rate = XPLMFindDataRef("sim/flightmodel/position/vh_ind_fpm");

	m_i_sim_ground_speed = XPLMFindDataRef("sim/time/ground_speed");

	m_i_battery_on = XPLMFindDataRef("sim/cockpit/electrical/battery_on");

	m_fa_prop_ratio = XPLMFindDataRef("sim/cockpit2/engine/actuators/prop_ratio");

	m_f_pitch = XPLMFindDataRef("sim/flightmodel/position/theta");
	m_f_roll = XPLMFindDataRef("sim/flightmodel/position/phi");

	m_f_jett_weight = XPLMFindDataRef("sim/flightmodel/weight/m_jettison");
	CountMissions();
	ReadMissions();

	ReadCustomWaypoints(m_street_waypoints, "street");
	ReadCustomWaypoints(m_urban_waypoints, "urban");
	ReadCustomWaypoints(m_sar_waypoints, "sar");
	ReadCustomWaypoints(m_sling_waypoints, "sling");

	ReadFSEAirports();
	ReadCustomICAOs();

	m_ivy_id = XPLMFindPluginBySignature("k80.Ivy");

	m_initialized = true;
}



void HRM_PlugIn::PluginStop()
{

	
	if (m_initialized == false) return;

	

	XPLMUnregisterFlightLoopCallback(WrapFlightLoopCallback, 0);

	XPLMDestroyMenu(m_PluginMenuID);

	SaveConfig();


	

}

void HRM_PlugIn::PluginEnable()
{
	m_plugin_enabled = 1;
}

void HRM_PlugIn::PluginDisable()
{
	
	m_plugin_enabled = 0;
}

void HRM_PlugIn::UpdatePosition()
{
	m_ld_latitude = XPLMGetDatad(m_d_latitude);
	m_ld_longitude = XPLMGetDatad(m_d_longitude);
	m_lf_heading = XPLMGetDataf(m_f_heading);

	for (auto p_mission : m_street_missions)	p_mission->SetPosition(m_ld_latitude, m_ld_longitude, m_lf_heading);
	for (auto p_mission : m_urban_missions)	p_mission->SetPosition(m_ld_latitude, m_ld_longitude, m_lf_heading);
	for (auto p_mission : m_sar_missions)	p_mission->SetPosition(m_ld_latitude, m_ld_longitude, m_lf_heading);
	for (auto p_mission : m_sling_missions)	p_mission->SetPosition(m_ld_latitude, m_ld_longitude, m_lf_heading);
}



void HRM_PlugIn::PluginReceiveMessage(XPLMPluginID inFromWho, int inMessage, void * inParam)
{
	HRMDebugString("Message Received");

	if (m_initialized == false) return;

	if (inFromWho == XPLM_PLUGIN_XPLANE)
	{
		if (inMessage == XPLM_MSG_PLANE_LOADED)
		{
			HRMDebugString("Plane loaded");

			m_aircraft_loaded = 1;
			
		}
		if (inMessage == XPLM_MSG_AIRPORT_LOADED)
		{
			HRMDebugString("Airport loaded");
		}
		

		if (inMessage == XPLM_MSG_SCENERY_LOADED)
		{
			HRMDebugString("Scenery loaded");
			

		}
	}
}



void HRM_PlugIn::PluginMenuHandler(void * in_menu_ref, void * inItemRef)
{
	HRMDebugString("Menu Handler");

	if (m_initialized == false) return;

	if (strcmp((char *) inItemRef, "Item1") == NULL)
	{
		if (!imguiPtr->GetVisible())
		{
			imguiPtr->Visible(true);

			//CreateLogbookWidget(int(m_ivyConfig->m_log_window_pos_x), int(m_ivyConfig->m_log_window_pos_y), int(m_ivyConfig->m_log_window_width), int(m_ivyConfig->m_log_window_height));
			//m_MenuLogbookShow = 1;
		}
		else
		{
			imguiPtr->Visible(false);

			//m_MenuLogbookShow = 0;
			//XPDestroyWidget(m_LogbookWidget, 1);
		}
	}
}

void HRM_PlugIn::ConfigureXSlingload()
{
	/*std::ifstream read_ini_file(m_xslingload_ini_path);
	std::string line_string;
	std::vector<std::string> ini_lines;

	while (std::getline(read_ini_file, line_string)) ini_lines.push_back(line_string);
	read_ini_file.close();

	std::ofstream write_ini_file(m_xslingload_ini_path);

	write_ini_file << m_xslingload_object_path << "$" << m_xslingload_size_empty << "$" << m_xslingload_weight_empty << "$" << m_xslingload_size_full << "$" << m_xslingload_weight_full << "$" << m_xslingload_offset << std::endl;

	for (int index = 1; index < ini_lines.size(); index++)
		write_ini_file << ini_lines[index] << std::endl;

	write_ini_file.close();*/

	

	std::ofstream write_apt_file(m_xslingload_apt_path);
	write_apt_file.precision(9);
	write_apt_file << "15 " << mp_cm_waypoint->latitude << " " << mp_cm_waypoint->longitude << " " << mp_cm_mission->m_lf_heading << " X-Hoist position" << std::endl;
	write_apt_file.close();
	
	m_xslingload_reload_position_file = true;

}

void HRM_PlugIn::MissionCreate()
{
	
	bool suitable_waypoint_found = false;

	m_cm_creation_failed = true;
	m_cm_no_waypoint_found = false;

	if ((m_cm_enable_fse == true) && (m_cm_autoconnect_fse == true))
	{
		if (FSEIsConnected() == false) FSELogin();
	}

	mp_cm_waypoint = NULL;
	mp_cm_mission = NULL;

	if (m_mission_scenario_lat == HRM::coord_invalid) return;
	if (m_mission_scenario_long == HRM::coord_invalid) return;
	if (m_mission_hospital_lat == HRM::coord_invalid) return;
	if (m_mission_hospital_long == HRM::coord_invalid) return;

	if (m_flight_plan_format == HRM::FPL_GTN)
	{
		if (m_mission_departure_lat == HRM::coord_invalid) return;
		if (m_mission_departure_long == HRM::coord_invalid) return;
	}

	// Get the random mission type and the corresponding waypoint vector. Bit more complicated since some might not be enabled

	int type_count = 0;
	std::vector<HRM_Waypoint *> considered_waypoints;
	std::vector<HRM_Waypoint *> *p_waypoint_vector = NULL;
	std::vector<HRM_Mission *> *p_mission_vector = NULL;

	std::vector<HRM_Waypoint *> global_waypoints;

	//std::vector

	bool disable_type = false;
	int random_type = 0;

	while ((suitable_waypoint_found == false) && (m_street_enable || m_urban_enable || m_sar_enable || m_sling_enable))
	{
		if (disable_type == true) 
		{
			if (random_type == 0) m_street_enable = false;
			if (random_type == 1) m_urban_enable = false;
			if (random_type == 2) m_sar_enable = false;
			if (random_type == 3) m_sling_enable = false;
		}

		for (auto p_wp : global_waypoints)
		{
			delete p_wp;
		}
		global_waypoints.clear();

		type_count = 0;
		disable_type = true;

		if (m_street_enable) type_count++;
		if (m_urban_enable) type_count++;
		if (m_sar_enable) type_count++;
		if (m_sling_enable) type_count++;

		if (type_count == 0)
		{
			m_cm_no_waypoint_found = true;
			return;
		}

		random_type = rand() % type_count;

		p_waypoint_vector = NULL;
		p_mission_vector = NULL;

		
		if (p_waypoint_vector == NULL)
		{
			if ((m_street_enable) && (random_type == 0))
			{
				ReadGlobalWaypoints(global_waypoints, "street");

				p_waypoint_vector = &m_street_waypoints;
				p_mission_vector = &m_street_missions;
			}
			else if (!m_street_enable)	random_type++;
		}

		if (p_waypoint_vector == NULL)
		{
			if ((m_urban_enable) && (random_type == 1))
			{
				ReadGlobalWaypoints(global_waypoints, "urban");

				p_waypoint_vector = &m_urban_waypoints;
				p_mission_vector = &m_urban_missions;
			}
			else if (!m_urban_enable)	random_type++;
		}

		if (p_waypoint_vector == NULL)
		{
			if ((m_sar_enable) && (random_type == 2))
			{
				ReadGlobalWaypoints(global_waypoints, "sar");

				p_waypoint_vector = &m_sar_waypoints;
				p_mission_vector = &m_sar_missions;
			}
			else if (!m_sar_enable)	random_type++;
		}

		if (p_waypoint_vector == NULL)
		{
			if ((m_sling_enable) && (random_type == 3))
			{
				ReadGlobalWaypoints(global_waypoints, "sling");

				p_waypoint_vector = &m_sling_waypoints;
				p_mission_vector = &m_sling_missions;
			}
			else if (!m_sling_enable)	random_type++;
		}

		if ((p_waypoint_vector == NULL) || (p_mission_vector == NULL)) continue;
		if (((global_waypoints.size() == 0) && (p_waypoint_vector->size() == 0)) || (p_mission_vector->size() == 0)) continue;


		
		considered_waypoints.clear();
		FindWaypoint(p_waypoint_vector, global_waypoints, considered_waypoints);

		

		// When no Waypoint in Range, Set Error and Return
		if (considered_waypoints.size() > 0) suitable_waypoint_found = true;

	}

	if (suitable_waypoint_found == false)
	{
		m_cm_no_waypoint_found = true;
		return;
	}

	// Find Random Mission and Waypoint

	int max = RAND_MAX;

	static long last_random_number = 0;
	int new_random_number = rand() + last_random_number;
	last_random_number = new_random_number;

	int random_waypoint = new_random_number % considered_waypoints.size();
	int random_mission = rand() % p_mission_vector->size();


	mp_cm_waypoint = considered_waypoints.at(random_waypoint);
	mp_cm_mission = p_mission_vector->at(random_mission);

	for (auto p_wp : global_waypoints)
	{
		if (p_wp != mp_cm_waypoint)  delete p_wp;
	}

	// Calculate the heading of the waypoint

	double meter_latitude = 0;
	double meter_longitude = 0;

	HRM_Object::GetDegreesPerMeter(mp_cm_waypoint->latitude, mp_cm_waypoint->longitude, meter_latitude, meter_longitude);

	double waypoint_meter_lat = (mp_cm_waypoint->lat_heading - mp_cm_waypoint->latitude) / meter_latitude;
	double waypoint_meter_long = (mp_cm_waypoint->long_heading - mp_cm_waypoint->longitude) / meter_longitude;

	double heading = atan2(waypoint_meter_long, waypoint_meter_lat) * 180 / M_PI;

	mp_cm_mission->SetPosition(mp_cm_waypoint->latitude, mp_cm_waypoint->longitude, heading);
	mp_cm_mission->DrawMission();

	// TODO: Remove all Waypoints outside range!!!!
	// TODO: FSE Airports

	CreateFlightPlan();
	
	// Sling Load
	if (mp_cm_mission->IsSlingLoad() == true)
	{

		if (pHRM->m_sling_load_plugin == HRM::XSlingload)
		{
			ConfigureXSlingload();
		}
		else if (pHRM->m_sling_load_plugin == HRM::AB412)
		{
			XPLMSetDataf(m_f_412_ext_lat, mp_cm_waypoint->latitude);
			XPLMSetDataf(m_f_412_ext_long, mp_cm_waypoint->longitude);
			XPLMSetDatai(m_i_412_ext_set, 1);
			m_412_patient_status = HRM::Patient_Off;
			XPLMCommandOnce(m_patient_ground);
			m_412_patient_loading_time = 0;

			m_412_meter_lat = calc_distance_m(mp_cm_waypoint->latitude, mp_cm_waypoint->longitude, mp_cm_waypoint->latitude + 1, mp_cm_waypoint->longitude);
			m_412_meter_long = calc_distance_m(mp_cm_waypoint->latitude, mp_cm_waypoint->longitude, mp_cm_waypoint->latitude, mp_cm_waypoint->longitude+ 1);

			XPLMLookupObjects("RescueX/people/Bergwacht/Bergwacht_Luftretter2.obj", 0, 0, load_cb, &m_412_obj_ref);
			const char * drefs[] = { NULL, NULL };
			m_412_inst_ref = XPLMCreateInstance(m_412_obj_ref, drefs);

			m_cm_distance_h_max = sqrt((m_sling_load_distance * m_sling_load_distance) / 3);

		}
	}
	// Say new mission
	IvyPlaySound(1, -1, -1);

	pHRM->m_mission_state = HRM::State_Plan_Flight;

	m_cm_creation_failed = false;
	m_cm_no_waypoint_found = false;
}

bool HRM_PlugIn::FindWaypoint(std::vector<HRM_Waypoint*>* p_waypoint_vector, std::vector<HRM_Waypoint*> &global_waypoint_vector,  std::vector<HRM_Waypoint*>& considered_waypoints)
{
	bool waypoint_found = false;

	std::vector<HRM_Waypoint*> all_waypoints;

	for (auto p_wp : *p_waypoint_vector) all_waypoints.push_back(p_wp);
	for (auto p_wp : global_waypoint_vector) all_waypoints.push_back(p_wp);




	// Check for all waypoints that are within the set distance 

	if (m_cm_enable_fse == false)
	{
		// MIN-MAX Distance 
		for (auto p_waypoint : all_waypoints)
		{
			double distance = abs(calc_distance_nm(m_mission_scenario_lat, m_mission_scenario_long, p_waypoint->latitude, p_waypoint->longitude));

			if ((distance >= m_cm_min_distance) && (distance <= m_cm_max_distance))
			{
				if (CheckWaypointAngle(*p_waypoint) == true) considered_waypoints.push_back(p_waypoint);
			}

		}
	}
	else
	{
		// First reduce the numbers of airports to compute
		std::vector<HRM_Airport *> reduced_airports;

		for (auto p_airport : m_fse_airports)
		{
			double distance = abs(calc_distance_nm(m_mission_scenario_lat, m_mission_scenario_long, p_airport->latitude, p_airport->longitude));

			if (distance <= m_cm_fse_airport_radius)
			{
				reduced_airports.push_back(p_airport);
			}
		}

		// Reduce number of waypoints to compute
		std::vector<HRM_Waypoint *> reduced_waypoints;
		// MIN-MAX Distance 
		for (auto p_waypoint : all_waypoints)
		{
			double distance = abs(calc_distance_nm(m_mission_scenario_lat, m_mission_scenario_long, p_waypoint->latitude, p_waypoint->longitude));

			if (distance <= m_cm_fse_airport_radius)
			{
				reduced_waypoints.push_back(p_waypoint);
			}

		}

		// Now the number of computation = reduced_waypoints.size() * considered_airports.size(). Still enough
		for (auto p_waypoint : reduced_waypoints)
		{
			HRM_Airport* p_closest_airport = NULL;
			double closest_distance = 10000;

			for (auto p_airport : reduced_airports)
			{
				double distance = abs(calc_distance_nm(p_airport->latitude, p_airport->longitude, p_waypoint->latitude, p_waypoint->longitude));

				if (distance < closest_distance)
				{
					p_closest_airport = p_airport;
					closest_distance = distance;
				}
			}

			// If the closest airport is the selected airport, consider this waypoint for mission creation
			if ((p_closest_airport != NULL) && (m_cm_scenario_icao.compare(p_closest_airport->icao) == 0))
			{
				considered_waypoints.push_back(p_waypoint);
			}
		}



	}


	return waypoint_found;
}

void HRM_PlugIn::MissionStart()
{
	m_cm_not_on_ground = false;

	if (m_li_on_ground == 0)
	{
		m_cm_not_on_ground = true;
		return;
	}

	if ((m_cm_enable_fse == true) && (m_cm_autoconnect_fse == true))
	{
		FSEStartFlight();
	}
	else
	{
		// Say flight planning finished
		IvyPlaySound(2, -1, -1);
	}

	if ((m_li_battery_on == 0) && (m_lia_engines_running[0] == 0) && (m_lia_engines_running[1] == 0))
	{
		if (m_difficutly == HRM::Hard) m_mission_preflight_countdown = HRM::preflight_time_hard;
		else if (m_difficutly == HRM::Normal) m_mission_preflight_countdown = HRM::preflight_time_normal;
		else m_mission_preflight_countdown = HRM::preflight_time_easy;

		pHRM->m_mission_state = HRM::State_Pre_Flight;
	}
	else
	{
		m_mission_preflight_countdown = 0;
		MissionStartFlight1();
	}




	

}

void HRM_PlugIn::MissionStartFlight1()
{
	float distance = abs(calc_distance_nm(m_ld_latitude, m_ld_longitude, mp_cm_waypoint->latitude, mp_cm_waypoint->longitude));

	m_mission_flight1_distance = distance;
	
	m_mission_flight1_countdown = m_mission_preflight_countdown > 0 ? m_mission_preflight_countdown : 0;

	if (m_difficutly == HRM::Hard) 
		m_mission_flight1_countdown +=	(2 * HRM::flight_time_up_down_hard) + 
									(distance * HRM::flight_time_per_nm_hard) + 
									(m_cm_estmimated_wp? HRM::flight_time_search_hard:0) + 
									(mp_cm_mission->IsSlingLoad()?HRM::flight_time_sling_hard:0);
	else if (m_difficutly == HRM::Normal) 
		m_mission_flight1_countdown +=	(2 * HRM::flight_time_up_down_normal) +
									(distance * HRM::flight_time_per_nm_normal) +
									(m_cm_estmimated_wp ? HRM::flight_time_search_normal : 0) +
									(mp_cm_mission->IsSlingLoad() ? HRM::flight_time_sling_normal : 0);
	else m_mission_flight1_countdown +=	(2 * HRM::flight_time_up_down_easy) +
									(distance * HRM::flight_time_per_nm_easy) +
									(m_cm_estmimated_wp ? HRM::flight_time_search_easy : 0) +
									(mp_cm_mission->IsSlingLoad() ? HRM::flight_time_sling_easy : 0);
	if (m_adjust_payload==true)
	{
		XPLMSetDataf(m_f_payload, m_crew_weight + m_ems_equippment_weight);
	}

	// Sling Load
	if (mp_cm_mission->IsSlingLoad() == true)
	{

		if (pHRM->m_sling_load_plugin == HRM::AB412)
		{
			if (m_patient_ground != NULL) XPLMCommandOnce(m_patient_ground);
		}
	}

	pHRM->m_mission_state = HRM::State_Flight_1;
}

void HRM_PlugIn::MissionStartFlight2()
{

	float distance = abs(calc_distance_nm(m_ld_latitude, m_ld_longitude, m_mission_hospital_lat, m_mission_hospital_long));

	m_mission_flight2_distance = distance;

	if (m_difficutly == HRM::Hard)
		m_mission_flight2_countdown =	(2 * HRM::flight_time_up_down_hard) +
										(distance * HRM::flight_time_per_nm_hard) +
										(m_cm_estmimated_wp ? HRM::flight_time_search_hard : 0) +
										(mp_cm_mission->IsSlingLoad() ? HRM::flight_time_sling_hard : 0);
	else if (m_difficutly == HRM::Normal)
		m_mission_flight2_countdown =	(2 * HRM::flight_time_up_down_normal) +
										(distance * HRM::flight_time_per_nm_normal) +
										(m_cm_estmimated_wp ? HRM::flight_time_search_normal : 0) +
										(mp_cm_mission->IsSlingLoad() ? HRM::flight_time_sling_normal : 0);
	else m_mission_flight2_countdown =	(2 * HRM::flight_time_up_down_easy) +
										(distance * HRM::flight_time_per_nm_easy) +
										(m_cm_estmimated_wp ? HRM::flight_time_search_easy : 0) +
										(mp_cm_mission->IsSlingLoad() ? HRM::flight_time_sling_easy : 0);

	if (m_adjust_payload == true)
	{
		XPLMSetDataf(m_f_payload, m_crew_weight + m_ems_equippment_weight + m_patient_weight);
	}

	if ((mp_cm_mission->IsSlingLoad() == true) && (m_sling_load_plugin == HRM::AB412))
	{
		XPLMCommandOnce(m_patient_transit);
		m_412_patient_status = HRM::Patient_Transit;
		// Patient loaded
		IvyPlaySound(12, -1, -1);
	}


	pHRM->m_mission_state = HRM::State_Flight_2;
}

void HRM_PlugIn::MissionCalcGFPoints()
{
	float g_force_seconds = 0;
	m_mission_points_g_force = 0;

	g_force_seconds += m_mission_gf_low * HRM::eval_g_low_factor;
	g_force_seconds += m_mission_gf_med * HRM::eval_g_med_factor;
	g_force_seconds += m_mission_gf_high * HRM::eval_g_high_factor;

	g_force_seconds += m_mission_gs_low * HRM::eval_g_low_factor;
	g_force_seconds += m_mission_gs_med * HRM::eval_g_med_factor;
	g_force_seconds += m_mission_gs_high * HRM::eval_g_high_factor;

	g_force_seconds += m_mission_gv_pos_low * HRM::eval_g_low_factor;
	g_force_seconds += m_mission_gv_pos_med * HRM::eval_g_med_factor;
	g_force_seconds += m_mission_gv_pos_high * HRM::eval_g_high_factor;

	g_force_seconds += m_mission_gv_neg_low * HRM::eval_g_low_factor;
	g_force_seconds += m_mission_gv_neg_med * HRM::eval_g_med_factor;
	g_force_seconds += m_mission_gv_neg_high * HRM::eval_g_high_factor;

	if (m_mission_time_failed == false)
		m_mission_points_g_force = ((float)HRM::points_g_flight2) / (1.f + (g_force_seconds / HRM::eval_g_total_factor));
}

void HRM_PlugIn::MissionFinish()
{
	m_mission_flight1_avg_speed = m_mission_flight1_distance / (m_mission_flight1_time / 3600);
	m_mission_flight2_avg_speed = m_mission_flight2_distance / (m_mission_flight2_time / 3600);

	MissionCalcGFPoints();

	m_mission_points_total = 0;
	m_mission_points_flight1 = 0;
	m_mission_points_flight2 = 0;
	m_mission_points_difficulty = 0;
	m_mission_points_search_range = 0;


	// If the Mission Time failed, you do not get any points
	if (m_mission_time_failed == false)
	{
		m_mission_points_flight1 = std::min(((float) m_mission_flight1_avg_speed) / HRM::eval_flight1_nominal_speed, 1.f) * ((float) HRM::points_speed_flight1);
		m_mission_points_flight2 = std::min(((float)m_mission_flight2_avg_speed) / HRM::eval_flight2_nominal_speed, 1.f) * ((float)HRM::points_speed_flight2);

		

		if (m_difficutly == HRM::Hard) m_mission_points_difficulty = HRM::points_hard;
		else if (m_difficutly == HRM::Normal) m_mission_points_difficulty = HRM::points_normal;
		else m_mission_points_difficulty = HRM::points_easy;

		if (m_cm_estmimated_wp == true)
		{
			m_mission_points_search_range = (HRM::points_search_range * m_cm_estimated_radius_m) / HRM::search_range_max;
		}

		m_mission_points_total = m_mission_points_flight1 + m_mission_points_flight2 + m_mission_points_g_force + m_mission_points_difficulty + m_mission_points_search_range;

	}

	
	// Missiong finished
	IvyPlaySound(8, m_mission_points_total, 9);

	

	// points = points_g_flight2 / ((1 + seconds ) * eval_g_total_factor)


	pHRM->m_mission_state = HRM::State_Mission_Finished;
}

void HRM_PlugIn::MissionReset()
{
	if (mp_cm_mission != NULL) mp_cm_mission->RemoveMission();

	m_mission_flight1_avg_speed = 0;
	m_mission_flight2_avg_speed = 0;
	m_mission_points_total = 0;

	m_mission_preflight_countdown = 0;
	m_mission_flight1_countdown = 0;
	m_mission_flight2_countdown = 0;
	m_mission_at_patient_countdown = 0;
	m_mission_at_hospital_countdown = 0;

	m_mission_flight1_time = 0;
	m_mission_flight2_time = 0;

	m_mission_gf_low = 0;
	m_mission_gf_med = 0;
	m_mission_gf_high = 0;

	m_mission_gs_low = 0;
	m_mission_gs_med = 0;
	m_mission_gs_high = 0;

	m_mission_gv_pos_low = 0;
	m_mission_gv_pos_med = 0;
	m_mission_gv_pos_high = 0;

	m_mission_gv_neg_low = 0;
	m_mission_gv_neg_med = 0;
	m_mission_gv_neg_high = 0;

	m_cm_creation_failed = false;
	m_cm_no_waypoint_found = false;
	m_cm_cancelling = false;
	m_cm_not_on_ground = false;
	m_xslingload_not_found = false;
	m_xslingload_found = false;
	m_xslingload_reload_position_file = false;
	m_412sar_found = false;
	m_cm_patient_sight_said = false;

	mp_cm_waypoint = NULL;
	mp_cm_mission = NULL;

	m_mission_time_failed = false;

	m_mission_points_total = 0;
	m_mission_points_flight1 = 0;
	m_mission_points_flight2 = 0;
	m_mission_points_g_force = 0;
	m_mission_points_difficulty = 0;
	m_mission_points_search_range = 0;

	m_412_patient_status = HRM::Patient_Off;
	m_412_crew_on = false;
	if (m_operator_off != NULL) XPLMCommandOnce(m_operator_off);
	if (m_patient_off != NULL) XPLMCommandOnce(m_patient_off);

	m_412_patient_local_x = 0;
	m_412_patient_local_y = 0;
	m_412_patient_local_z = 0;

	m_412_patient_lat = 0;
	m_412_patient_long = 0;
	m_412_patient_elev = 0;

	m_412_patient_loading_time = 0;

	m_cm_say_timer = 0;
	m_cm_say_state = 0;
}



void HRM_PlugIn::MissionCancel()
{
	MissionReset();
	pHRM->m_mission_state = HRM::State_Create_Mission;

	if (m_patient_off != NULL) XPLMCommandOnce(m_patient_off);
}

void HRM_PlugIn::FSERegister()
{
	m_fse_dr_is_connected = XPLMFindDataRef("fse/status/connected");
	m_fse_dr_is_flying = XPLMFindDataRef("fse/status/flying");
	m_fse_dr_can_end_flight = XPLMFindDataRef("fse/status/canendflight");

	m_fse_command_login = XPLMFindCommand("fse/server/connect");
	m_fse_command_start = XPLMFindCommand("fse/flight/start");
	m_fse_command_end = XPLMFindCommand("fse/flight/finish");

}

void HRM_PlugIn::FSELogin()
{
	XPLMCommandOnce(m_fse_command_login);
}

bool HRM_PlugIn::FSEIsConnected()
{
	if (m_fse_dr_is_connected != NULL)
	{
		if (m_fse_li_is_connected > 0) return true;
	}
	return false;
}

void HRM_PlugIn::FSEStartFlight()
{
	XPLMCommandOnce(m_fse_command_start);
}

bool HRM_PlugIn::FSEIsFlying()
{
	if (m_fse_dr_is_flying != NULL)
	{
		if (m_fse_li_is_flying > 0) return true;
	}
	return false;
}

bool HRM_PlugIn::FSECanFinish()
{
	if (m_fse_dr_can_end_flight != NULL)
	{
		if (m_fse_li_can_end_flight > 0) return true;
	}
	return false;
}

void HRM_PlugIn::FSEFinishFlight()
{
	XPLMCommandOnce(m_fse_command_end);
}

void HRM_PlugIn::Check412SAR()
{
}

void HRM_PlugIn::IvyPlaySound(int sound_before, int say_value, int sound_after)
{
	static HRM_Sound hrm_sound;
	if (m_ivy_id != XPLM_NO_PLUGIN_ID)
	{
		
		hrm_sound.sound_before = sound_before;
		hrm_sound.say_value = say_value;
		hrm_sound.sound_after = sound_after;

		XPLMSendMessageToPlugin(m_ivy_id, HRM_MESSAGE_SOUND_1, &hrm_sound);
	}
}

void HRM_PlugIn::CreateFlightPlan()
{
	if (m_flight_plan_format == HRM::FPL_XP11)
	{
		std::ofstream fms_file;
		fms_file.open(m_fms_file, std::ios::out);

		if (fms_file.is_open())
		{
			fms_file << "I" << std::endl;
			fms_file << "1100 Version" << std::endl;
			fms_file << "CYCLE " << m_cm_airac_cycle << std::endl;
			fms_file << "DEP " << std::endl;
			fms_file << "DES " << std::endl;
			fms_file << "NUMENR 3" << std::endl;

			fms_file.precision(9);

			fms_file << "28 " << "START DEP 25000.000000 " << m_ld_latitude << " " << m_ld_longitude << std::endl;

			double wp_latitude = mp_cm_waypoint->latitude;
			double wp_longitude = mp_cm_waypoint->longitude;

			if (m_cm_estmimated_wp == true)
			{
				double deviation_lat_meter = (rand() % m_cm_estimated_radius_m) - (m_cm_estimated_radius_m / 2);
				double deviation_long_meter = (rand() % m_cm_estimated_radius_m) - (m_cm_estimated_radius_m / 2);

				double meter_latitude = 0;
				double meter_longitude = 0;

				HRM_Object::GetDegreesPerMeter(mp_cm_waypoint->latitude, mp_cm_waypoint->longitude, meter_latitude, meter_longitude);

				wp_latitude += deviation_lat_meter * meter_latitude;
				wp_longitude += deviation_long_meter * meter_longitude;
			}

			fms_file << "28 " << "SCENE DEP 25000.000000 " << wp_latitude << " " << wp_longitude << std::endl;

			fms_file << "28 " << "HOSP DEP 25000.000000 " << m_mission_hospital_lat << " " << m_mission_hospital_long << std::endl;


			fms_file.close();
		}
		else
		{
			HRMDebugString("Could not write FMS file");
		}
	}
	else if (m_flight_plan_format == HRM::FPL_XP10)
	{
		std::ofstream fms_file;
		fms_file.open(m_fms_file, std::ios::out);

		if (fms_file.is_open())
		{
			fms_file << "I" << std::endl;
			fms_file << "3 version" << std::endl;
			//fms_file << "CYCLE " << m_cm_airac_cycle << std::endl;
			fms_file << "DEP " << std::endl;
			fms_file << "DES " << std::endl;
			//fms_file << "NUMENR 3" << std::endl;

			fms_file.precision(9);

			fms_file << "1 " << "START 25000.000000 " << m_ld_latitude << " " << m_ld_longitude << std::endl;

			double wp_latitude = mp_cm_waypoint->latitude;
			double wp_longitude = mp_cm_waypoint->longitude;

			if (m_cm_estmimated_wp == true)
			{
				double deviation_lat_meter = (rand() % m_cm_estimated_radius_m) - (m_cm_estimated_radius_m / 2);
				double deviation_long_meter = (rand() % m_cm_estimated_radius_m) - (m_cm_estimated_radius_m / 2);

				double meter_latitude = 0;
				double meter_longitude = 0;

				HRM_Object::GetDegreesPerMeter(mp_cm_waypoint->latitude, mp_cm_waypoint->longitude, meter_latitude, meter_longitude);

				wp_latitude += deviation_lat_meter * meter_latitude;
				wp_longitude += deviation_long_meter * meter_longitude;
			}

			fms_file << "1 " << "SCENE 25000.000000 " << wp_latitude << " " << wp_longitude << std::endl;

			fms_file << "1 " << "HOSP 25000.000000 " << m_mission_hospital_lat << " " << m_mission_hospital_long << std::endl;


			fms_file.close();
		}
		else
		{
			HRMDebugString("Could not write FMS file");
		}
	}
	else if (m_flight_plan_format == HRM::FPL_GTN)
	{
		double wp_latitude = mp_cm_waypoint->latitude;
		double wp_longitude = mp_cm_waypoint->longitude;

		if (m_cm_estmimated_wp == true)
		{
			double deviation_lat_meter = (rand() % m_cm_estimated_radius_m) - (m_cm_estimated_radius_m / 2);
			double deviation_long_meter = (rand() % m_cm_estimated_radius_m) - (m_cm_estimated_radius_m / 2);

			double meter_latitude = 0;
			double meter_longitude = 0;

			HRM_Object::GetDegreesPerMeter(mp_cm_waypoint->latitude, mp_cm_waypoint->longitude, meter_latitude, meter_longitude);

			wp_latitude += deviation_lat_meter * meter_latitude;
			wp_longitude += deviation_long_meter * meter_longitude;
		}


		std::string gtn_file_name = m_gfp_path + m_ds + m_gfp_file;

		std::ofstream fms_file;
		fms_file.open(gtn_file_name, std::ios::out);

		if (fms_file.is_open())
		{
			fms_file << "FPN/RI:F:" << m_cm_departure_icao << ":F:";
			
			if (wp_latitude >= 0) fms_file << "N";
			else fms_file << "S";
			std::string temp = std::to_string(abs(wp_latitude));
			if (abs(wp_latitude) < 10) temp = "0" + temp;
			
			boost::replace_all(temp, ".", "");
			fms_file << temp;
			//fms_file << temp.substr(0, 2);
			//if (temp.size() > 3) fms_file << temp.substr(3, temp.size() - 3);

			if (wp_longitude >= 0) fms_file << "E";
			else fms_file << "W";
			temp = std::to_string(abs(wp_longitude));
			if (abs(wp_longitude) < 100) temp = "0" + temp;
			if (abs(wp_longitude) < 10) temp = "0" + temp;
			
			
			boost::replace_all(temp, ".", "");
			fms_file << temp;
			//fms_file << temp.substr(0, 3);
			//if (temp.size() > 3) fms_file << temp.substr(4, temp.size() - 4);


			fms_file << ":F:" << m_cm_hospital_icao << std::endl;
			fms_file.close();
		}
		else
		{
			HRMDebugString("Could not write GFP file");
		}


		std::string wpt_file_name = m_wpt_path + m_ds + m_wpt_file;
		std::ifstream wpt_file_in;
		std::string out_buffer = "";
		std::string line_string = "";

		bool departure_found = false;
		bool scenario_found = false;
		bool hospital_found = false;

		wpt_file_in.open(wpt_file_name, std::ios::in);

		while (std::getline(wpt_file_in, line_string))
		{
			if ((line_string.find(m_cm_departure_icao)) != std::string::npos)
			{
				out_buffer += m_cm_departure_icao + ",," + std::to_string(m_mission_departure_lat) + "," + std::to_string(m_mission_departure_long) + "\n";
				departure_found = true;
				if (m_cm_departure_icao.compare(m_cm_hospital_icao) == 0)
					hospital_found = true;
			}
			else if ((line_string.find(m_cm_hospital_icao)) != std::string::npos)
			{
				out_buffer += m_cm_hospital_icao + ",," + std::to_string(m_mission_hospital_lat) + "," + std::to_string(m_mission_hospital_long) + "\n";
				hospital_found = true;
			}
			else if ((line_string.find("HRMSCENE")) != std::string::npos)
			{
				out_buffer += "HRMSCENE,," + std::to_string(wp_latitude) + "," + std::to_string(wp_longitude) + "\n";
				scenario_found = true;
			}
			else
			{
				out_buffer += line_string + "\n";
			}
		}

		wpt_file_in.close();

		if (departure_found == false)
		{
			out_buffer += m_cm_departure_icao + ",," + std::to_string(m_mission_departure_lat) + "," + std::to_string(m_mission_departure_long) + "\n";
		}

		if (hospital_found == false)
		{
			out_buffer += m_cm_hospital_icao + ",," + std::to_string(m_mission_hospital_lat) + "," + std::to_string(m_mission_hospital_long) + "\n";
		}

		if (scenario_found  == false)
		{
			out_buffer += "HRMSCENE,," + std::to_string(wp_latitude) + "," + std::to_string(wp_longitude) + "\n";
		}

		std::ofstream wpt_file_out;

		wpt_file_out.open(wpt_file_name, std::ios::out);

		if (wpt_file_out.is_open())
		{
			wpt_file_out << out_buffer;
			wpt_file_out.close();
		}
		else
		{
			HRMDebugString("Could not write user.wpt file");
		}

	}

}

void HRM_PlugIn::ReadCustomICAOs()
{
	try
	{
		std::ifstream waypoint_file(m_config_path + "custom_hospitals.fms");
		std::string line_string;

		bool odd_waypoint = true;
		HRM_Airport *p_airport = new HRM_Airport();

		double lat_correct = 0;
		double long_correct = 0;



		while (std::getline(waypoint_file, line_string))
		{
			std::stringstream line_stream(line_string);
			std::string airport_icao = "";
			std::string dummy_string;
			double dummy_double;

			int line_code = 0;
			line_stream >> line_code;

			if ((line_stream) && (line_code == 28))
			{
				line_stream >> airport_icao; // Waypoint Name
				line_stream >> dummy_string; // Waypoint Special
				line_stream >> dummy_double; // Waypoint Altitude

				double way_lat = HRM::coord_invalid;
				double way_long = HRM::coord_invalid;

				line_stream >> way_lat;
				line_stream >> way_long;

				if ((way_lat != HRM::coord_invalid) && (way_lat != HRM::coord_invalid))
				{
					p_airport->icao = airport_icao;
					p_airport->latitude = way_lat;
					p_airport->longitude = way_long;
					p_airport->name = "Custom Hospital";

					m_custom_hospitals.push_back(p_airport);
					p_airport = new HRM_Airport();
					
				}
			}


		}

		delete p_airport;

	}
	catch (...)
	{
		HRMDebugString("Could not read custom hospital file");
	}
}

void HRM_PlugIn::SaveCustomICAOs()
{
	std::ofstream fms_file;
	fms_file.open(m_config_path + "custom_hospitals.fms", std::ios::out);

	if (fms_file.is_open())
	{
		fms_file << "I" << std::endl;
		fms_file << "1100 Version" << std::endl;
		fms_file << "CYCLE " << m_cm_airac_cycle << std::endl;
		fms_file << "DEP " << std::endl;
		fms_file << "DES " << std::endl;
		fms_file << "NUMENR 3" << std::endl;

		fms_file.precision(9);

		for (auto p_airport : m_custom_hospitals)
		{
			fms_file << "28 " << p_airport->icao <<" DEP 25000.000000 " << p_airport->latitude << " " << p_airport->longitude << std::endl;
		}

		fms_file.close();
	}
	else
	{
		HRMDebugString("Could not write custom hospital file");
	}
}

void HRM_PlugIn::AddCustomICAO()
{
	m_custom_icao_exists = false;

	if (m_custom_icao.size() == 0)
	{
		return;
	}

	for (auto p_airport : m_custom_hospitals)
	{
		if (p_airport->icao.compare(m_custom_icao) == 0)
		{
			p_airport->latitude = m_ld_latitude;
			p_airport->longitude = m_ld_longitude;
			p_airport->name = "Custom Hospital";

			m_custom_icao_exists = true;
			return;
		}
	}

	HRM_Airport *p_airport = new HRM_Airport();

	p_airport->icao = m_custom_icao;
	p_airport->latitude = m_ld_latitude;
	p_airport->longitude = m_ld_longitude;
	p_airport->name = "Custom Hospital";

	m_custom_hospitals.push_back(p_airport);

	SaveCustomICAOs();
	
}

void HRM_PlugIn::ReadFSEAirports()
{
	std::string fse_line;
	std::ifstream fse_file(m_config_path + "icaodata.csv");
	std::string fse_delimiter = ",";
	size_t pos = 0;

	try
	{

		if (fse_file.is_open())
		{
			HRMDebugString("icaodata.csv opened successfully");

			while (!fse_file.eof())
			{
				getline(fse_file, fse_line);


				std::string apt_icao = "";
				std::string apt_lat = "";
				std::string apt_long = "";
				std::string apt_name = "";
				std::string apt_type = "";
				std::string apt_size = "";
				double apt_longitude = 0;
				double apt_latitude = 0;
				int pos_long = 0;
				int pos_lat = 0;

				//ICAO
				if ((pos = fse_line.find(fse_delimiter)) != std::string::npos)
				{
					apt_icao = fse_line.substr(0, pos);
					fse_line.erase(0, pos + fse_delimiter.length());
				}

				// Check for header line
				if ((apt_icao.compare("icao") != 0) && (pos != std::string::npos))
				{
					// Get Latitude
					if ((pos = fse_line.find(fse_delimiter)) != std::string::npos)
					{
						apt_lat = fse_line.substr(0, pos);
						apt_latitude = std::stof(apt_lat);
						pos_lat = 90 + ((int)apt_latitude);
						fse_line.erase(0, pos + fse_delimiter.length());
					}

					// Get Longitude
					if ((pos = fse_line.find(fse_delimiter)) != std::string::npos)
					{
						apt_long = fse_line.substr(0, pos);
						apt_longitude = std::stof(apt_long);
						pos_long = 180 + ((int)apt_longitude);
						fse_line.erase(0, pos + fse_delimiter.length());
					}

					// Get Type
					if ((pos = fse_line.find(fse_delimiter)) != std::string::npos)
					{
						apt_type = fse_line.substr(0, pos);
						fse_line.erase(0, pos + fse_delimiter.length());
					}

					// Get Size
					if ((pos = fse_line.find(fse_delimiter)) != std::string::npos)
					{
						apt_size = fse_line.substr(0, pos);
						fse_line.erase(0, pos + fse_delimiter.length());
					}

					// Get Name
					if ((pos = fse_line.find(fse_delimiter)) != std::string::npos)
					{
						apt_name = fse_line.substr(0, pos);
						fse_line.erase(0, pos + fse_delimiter.length());
					}

					HRM_Airport *p_airport = new HRM_Airport();

					p_airport->icao = apt_icao;
					p_airport->name = apt_name;
					p_airport->longitude = apt_longitude;
					p_airport->latitude = apt_latitude;

					m_fse_airports.push_back(p_airport);
				}


			}




			HRMDebugString("Finished loading icaodata.csv");
			fse_file.close();
		}
		else
		{
			HRMDebugString("Could not find icaodata.csv");
		}
	}
	catch (...)
	{
		HRMDebugString("Error reading icaodata.csv");
	}
}

void HRM_PlugIn::ReadCustomWaypoints(std::vector<HRM_Waypoint*>& waypoint_vector, std::string file_name)
{
	for (int scenery_number = 1; scenery_number <= HRM::max_scenery; scenery_number++)
	{
		ReadWaypointFile(waypoint_vector, m_config_path + file_name + "_" + std::to_string(scenery_number) + ".fms");

	}

}

void HRM_PlugIn::ReadGlobalWaypoints(std::vector<HRM_Waypoint*>& waypoint_vector,  std::string file_name)
{
	int latitude = (int)m_ld_latitude;
	int longitude = (int)m_ld_longitude;

	

	for (int lat_index = latitude - 4; lat_index < (latitude + 4); lat_index++)
	{
		for (int long_index = longitude - 4; long_index < (longitude + 4); long_index++)
		{
			std::string waypoint_filename = file_name + "_";

			waypoint_filename += lat_index < 0 ? "-" : "+";
			if ((lat_index < 10) && (lat_index > -10)) waypoint_filename += "0";
			waypoint_filename += std::to_string(abs(lat_index));

			waypoint_filename += long_index < 0 ? "-" : "+";
			if ((long_index < 100) && (long_index > -100)) waypoint_filename += "0";
			if ((long_index < 10) && (long_index > -10)) waypoint_filename += "0";
			waypoint_filename += std::to_string(abs(long_index));

			ReadWaypointFile(waypoint_vector, m_config_path + m_global_path + m_ds + waypoint_filename + ".fms");
		}
	}

	

}


void HRM_PlugIn::ReadWaypointFile(std::vector<HRM_Waypoint*>& waypoint_vector, std::string file_name)
{
	try
	{
		std::ifstream waypoint_file(file_name);
		std::string line_string;

		bool odd_waypoint = true;
		HRM_Waypoint *p_waypoint = new HRM_Waypoint();

		double lat_correct = 0;
		double long_correct = 0;

		XPLMProbeRef probe = XPLMCreateProbe(xplm_ProbeY);

		while (std::getline(waypoint_file, line_string))
		{
			std::stringstream line_stream(line_string);

			std::string dummy_string;
			double dummy_double;

			int line_code = 0;
			line_stream >> line_code;

			// This is my custom code to correct x-plane deviation of this specific area
			if ((line_stream) && (line_code == 12345))
			{
				line_stream >> lat_correct;
				line_stream >> long_correct;
			}

			if ((line_stream) && (line_code == 28))
			{
				line_stream >> dummy_string; // Waypoint Name
				line_stream >> dummy_string; // Waypoint Special
				line_stream >> dummy_double; // Waypoint Altitude

				double way_lat = HRM::coord_invalid;
				double way_long = HRM::coord_invalid;

				line_stream >> way_lat;
				line_stream >> way_long;

				if ((way_lat != HRM::coord_invalid) && (way_lat != HRM::coord_invalid))
				{
					// Offset Correction

					double meter_latitude = 0;
					double meter_longitude = 0;

					HRM_Object::GetDegreesPerMeter(way_lat, way_long, meter_latitude, meter_longitude);

					// Position Waypoint
					if (odd_waypoint == true)
					{
						p_waypoint->latitude = way_lat + (meter_latitude * lat_correct);
						p_waypoint->longitude = way_long + (meter_longitude * long_correct);

						odd_waypoint = false;
					}
					// Direction Waypoint
					else
					{
						p_waypoint->lat_heading = way_lat + (meter_latitude * lat_correct);
						p_waypoint->long_heading = way_long + (meter_longitude * long_correct);

						// Check for Water
						double zero_x, zero_y, zero_z;
						XPLMWorldToLocal(p_waypoint->latitude, p_waypoint->longitude, 0, &zero_x, &zero_y, &zero_z);

						double local_x = 0, local_y = 0, local_z = 0;
						local_x = zero_x;
						local_z = zero_z;

						XPLMProbeInfo_t info;
						info.structSize = sizeof(info);
						XPLMProbeResult result = XPLMProbeTerrainXYZ(probe, zero_x, zero_y, zero_z, &info);

						double local_long;
						double local_lat;
						double local_alt;

						XPLMLocalToWorld(info.locationX, info.locationY, info.locationZ, &local_lat, &local_long, &local_alt);
						XPLMWorldToLocal(p_waypoint->latitude, p_waypoint->longitude, local_alt, &zero_x, &zero_y, &zero_z); // incorporate elevation 
						XPLMProbeTerrainXYZ(probe, zero_x, zero_y, zero_z, &info);
						
						// If no water
						if (info.is_wet == 0)
							waypoint_vector.push_back(p_waypoint);

						p_waypoint = new HRM_Waypoint();
						odd_waypoint = true;

					}
				}
			}


		}
		XPLMDestroyProbe(probe);

		delete p_waypoint;

	}
	catch (...)
	{

	}
}

bool HRM_PlugIn::CheckWaypointAngle(HRM_Waypoint& waypoint)
{
	if (m_course_limit_enable == false) return true;

	double lat_diff = waypoint.latitude - m_ld_latitude;
	double lon_diff = waypoint.longitude - m_ld_longitude;

	float angle = (float) (atan2(lon_diff, lat_diff) * 180.0f / M_PI);

	if (angle < 0.0f) angle += 360.0f;

	if ((m_course_limit_start <= m_course_limit_stop) &&(angle >= m_course_limit_start) && (angle <= m_course_limit_stop)) return true;
	if ((m_course_limit_start > m_course_limit_stop) && ((angle >= m_course_limit_start) || (angle <= m_course_limit_stop))) return true;

	return false;
}

void HRM_PlugIn::SaveConfig()
{
	boost::property_tree::ptree pt;

	//pt.push_back("HRM.", );


	
	pt.put("HRM.gfp_file_path", m_gfp_path);
	pt.put("HRM.user_wpt_file_path", m_wpt_path);

	pt.put("HRM.global_path_index", m_global_path_index);

	pt.put("HRM.scenery_index", m_scenery_number);

	pt.put("HRM.flight_plan_format", m_flight_plan_format);

	pt.put("HRM.collective_down_threshold", m_cm_collective_min);

	pt.put("HRM.airac_cycle", m_cm_airac_cycle);
	pt.put("HRM.patient_weight_kg", m_patient_weight);
	pt.put("HRM.helicopter_crew_weight_kg", m_crew_weight);
	pt.put("HRM.ems_equipment_weight_kg", m_ems_equippment_weight);
	pt.put("HRM.adjust_payload_enable", m_adjust_payload);

	pt.put("HRM.street_enable", m_street_enable);
	pt.put("HRM.urban_enable", m_urban_enable);
	pt.put("HRM.sar_enable", m_sar_enable);
	pt.put("HRM.sling_enable", m_sling_enable);

	pt.put("HRM.scenario_position_type", (int) m_cm_use_position);
	pt.put("HRM.scenario_min_distance_nm", m_cm_min_distance);
	pt.put("HRM.scenario_max_distance_nm", m_cm_max_distance);

	pt.put("HRM.course_limit_enable", m_course_limit_enable);
	pt.put("HRM.course_limit_start", m_course_limit_start);
	pt.put("HRM.course_limit_stop", m_course_limit_stop);

	pt.put("HRM.departure_icao", m_cm_departure_icao);
	pt.put("HRM.scenario_icao", m_cm_scenario_icao);
	pt.put("HRM.hospital_icao", m_cm_hospital_icao);
	pt.put("HRM.panic_call_enabled", m_cm_estmimated_wp);
	pt.put("HRM.panic_call_search_area_m", m_cm_estimated_radius_m);
	pt.put("HRM.fse_enabled", m_cm_enable_fse);
	pt.put("HRM.fse_autoconnect_enabled", m_cm_autoconnect_fse);
	pt.put("HRM.fse_max_airport_radius_nm", m_cm_fse_airport_radius);
	pt.put("HRM.difficulty_index", m_difficutly);
	pt.put("HRM.pos_calc_rate_s", m_position_calc_rate);
	pt.put("HRM.pickup_countdown_s", m_patient_countdown_value);
	pt.put("HRM.hospital_delivery_countdown_s", m_hospital_countdown_value);

	
	pt.put("HRM.sling_load_plugin", m_sling_load_plugin);

	pt.put("HRM.sling_load_distance", m_sling_load_distance);
	pt.put("HRM.sling_load_time_min", m_sling_load_time_min);
	pt.put("HRM.sling_say_distance", m_cm_sling_say_distance);
	pt.put("HRM.sling_say_time", m_cm_sling_say_time);


	pt.put("HRM.xslingload_treshold", m_xslingload_treshold);
	pt.put("HRM.xslingload_weight_empty", m_xslingload_weight_empty);
	pt.put("HRM.xslingload_weight_full", m_xslingload_weight_full);
	pt.put("HRM.xslingload_size_empty", m_xslingload_size_empty);
	pt.put("HRM.xslingload_size_full", m_xslingload_size_full);
	pt.put("HRM.xslingload_offset", m_xslingload_offset);
	pt.put("HRM.xslingload_object_path", m_xslingload_object_path);
	
	boost::property_tree::ini_parser::write_ini(m_config_path + "HRM.ini", pt);
}

void HRM_PlugIn::ReadConfig()
{
	boost::property_tree::ptree pt;
	try
	{
		boost::property_tree::ini_parser::read_ini(m_config_path + "HRM.ini", pt);
	}
	catch (...)
	{
		HRMDebugString("Could not read config file");
		return;
	}

	try { m_global_path_index = pt.get<int>("HRM.global_path_index"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_scenery_number = pt.get<int>("HRM.scenery_index"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	if (m_global_path_index < m_path_vector.size())
	{
		m_global_path = m_path_vector[m_global_path_index];
	}

	try { m_flight_plan_format = pt.get<int>("HRM.flight_plan_format"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_cm_collective_min = pt.get<float>("HRM.collective_down_threshold"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_cm_airac_cycle = pt.get<int>("HRM.airac_cycle"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_patient_weight = pt.get<float>("HRM.patient_weight_kg"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_crew_weight = pt.get<float>("HRM.helicopter_crew_weight_kg"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_ems_equippment_weight = pt.get<float>("HRM.ems_equipment_weight_kg"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_adjust_payload = pt.get<bool>("HRM.adjust_payload_enable"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_street_enable = pt.get<bool>("HRM.street_enable"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_urban_enable = pt.get<bool>("HRM.urban_enable"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_sar_enable = pt.get<bool>("HRM.sar_enable"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_sling_enable = pt.get<bool>("HRM.sling_enable"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_cm_use_position = (HRM::Scenario_Position) pt.get<int>("HRM.scenario_position_type"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_cm_min_distance = pt.get<int>("HRM.scenario_min_distance_nm"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_cm_max_distance = pt.get<int>("HRM.scenario_max_distance_nm"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_course_limit_enable = pt.get<bool>("HRM.course_limit_enable"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_course_limit_start = pt.get<int>("HRM.course_limit_start"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_course_limit_stop = pt.get<int>("HRM.course_limit_stop"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }


	try { m_cm_departure_icao = pt.get<std::string>("HRM.departure_icao"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_cm_scenario_icao = pt.get<std::string>("HRM.scenario_icao"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_cm_hospital_icao = pt.get<std::string>("HRM.hospital_icao"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_cm_estmimated_wp = pt.get<bool>("HRM.panic_call_enabled"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_cm_estimated_radius_m = pt.get<int>("HRM.panic_call_search_area_m"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_cm_enable_fse = pt.get<bool>("HRM.fse_enabled"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_cm_autoconnect_fse = pt.get<bool>("HRM.fse_autoconnect_enabled"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_cm_fse_airport_radius = pt.get<double>("HRM.fse_max_airport_radius_nm"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_difficutly = pt.get<int>("HRM.difficulty_index"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_position_calc_rate = pt.get<float>("HRM.pos_calc_rate_s"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_patient_countdown_value = pt.get<float>("HRM.pickup_countdown_s"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_hospital_countdown_value = pt.get<float>("HRM.hospital_delivery_countdown_s"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_xslingload_treshold = pt.get<float>("HRM.xslingload_treshold"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_sling_load_plugin = pt.get<int>("HRM.sling_load_plugin"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_sling_load_distance = pt.get<float>("HRM.sling_load_distance"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_sling_load_time_min = pt.get<float>("HRM.sling_load_time_min"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_cm_sling_say_distance = pt.get<float>("HRM.sling_say_distance"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_cm_sling_say_time = pt.get<float>("HRM.sling_say_time"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_xslingload_weight_empty = pt.get<float>("HRM.xslingload_weight_empty"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_xslingload_weight_full = pt.get<float>("HRM.xslingload_weight_full"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_xslingload_size_empty = pt.get<float>("HRM.xslingload_size_empty"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_xslingload_size_full = pt.get<float>("HRM.xslingload_size_full"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_xslingload_offset = pt.get<float>("HRM.xslingload_offset"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_xslingload_object_path = pt.get<std::string>("HRM.xslingload_object_path"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_gfp_path = pt.get<std::string>("HRM.gfp_file_path"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

	try { m_wpt_path = pt.get<std::string>("HRM.user_wpt_file_path"); }
	catch (...) { HRMDebugString("Ini File: Entry not found."); }

}

void HRM_PlugIn::SaveMissions()
{

	/*boost::property_tree::xml_writer_settings<char> settings(' ', 4);

	boost::property_tree::ptree pt;
	int mission_counter = 0;

	for (auto p_mission : m_street_missions)	p_mission->SaveMission(pt, mission_counter);
	for (auto p_mission : m_urban_missions)		p_mission->SaveMission(pt, mission_counter);
	for (auto p_mission : m_sar_missions)		p_mission->SaveMission(pt, mission_counter);
	for (auto p_mission : m_sling_missions)		p_mission->SaveMission(pt, mission_counter);

	boost::property_tree::write_xml(m_scenery_file + "_" + std::to_string(m_scenery_number) + ".xml", pt, std::locale(), settings);*/
}

void HRM_PlugIn::ReadMissions()
{
	m_street_missions.clear();
	m_urban_missions.clear();
	m_sar_missions.clear();
	m_sling_missions.clear();

	int scenery_number = m_scenery_number + 1;

	//for (int scenery_number = 1; scenery_number <= HRM::max_scenery; scenery_number++)
	{
		bool file_found = true;
		boost::property_tree::ptree pt;
		try
		{
			boost::property_tree::read_xml(m_scenery_file + "_" + std::to_string(scenery_number) + ".xml", pt);
		}
		catch (...)
		{
			file_found = false;
			//return;
		}
		if (file_found)
		{
			int mission_counter = 0;
			bool mission_created = true;

			try { m_scenery_name = pt.get<std::string>("scenery.name"); }
			catch (...) {}

			while (mission_created)
			{
				HRM_Mission *p_mission = new HRM_Mission();

				mission_created = p_mission->ReadMission(pt, mission_counter);

				if (mission_created)
				{
					if (p_mission->m_mission_type == 0)				m_street_missions.push_back(p_mission);
					else if (p_mission->m_mission_type == 1)		m_urban_missions.push_back(p_mission);
					else if (p_mission->m_mission_type == 2)		m_sar_missions.push_back(p_mission);
					else if (p_mission->m_mission_type == 3)		m_sling_missions.push_back(p_mission);
					else delete p_mission;
				}
				else delete p_mission;

			}
		}
	}

}

void HRM_PlugIn::CountMissions()
{
	bool file_found = true;
	m_scenery_count = 0;
	std::string current_name = "";
	for (int scenery_number = 1; (scenery_number <= HRM::max_scenery) && (file_found == true); scenery_number++)
	{
		
		boost::property_tree::ptree pt;
		try
		{
			boost::property_tree::read_xml(m_scenery_file + "_" + std::to_string(scenery_number) + ".xml", pt);
		}
		catch (...)
		{
			file_found = false;
			//return;
		}

		try { current_name = pt.get<std::string>("scenery.name"); }
		catch (...) { 
			file_found = false; 
		}

		if (file_found)
		{
			m_scenery_count++;
			m_scenery_names.push_back(current_name);
		}
	}

	if (m_scenery_number >= m_scenery_names.size())
		m_scenery_number = 0;
}

void HRM_PlugIn::ReadDataFast()
{
	m_ld_latitude = XPLMGetDatad(m_d_latitude);
	m_ld_longitude = XPLMGetDatad(m_d_longitude);
	m_lf_heading = XPLMGetDataf(m_f_heading);

	m_lf_g_normal = XPLMGetDataf(m_f_g_normal);
	m_lf_g_forward = XPLMGetDataf(m_f_g_forward);
	m_lf_g_side = XPLMGetDataf(m_f_g_side);

	m_lf_pitch = XPLMGetDataf(m_f_pitch);
	m_lf_roll = XPLMGetDataf(m_f_roll);

	m_li_paused = XPLMGetDatai(m_i_paused);
	m_li_replay = XPLMGetDatai(m_i_replay);
}

void HRM_PlugIn::ReadDataSlow()
{

	m_li_vr_enabled = XPLMGetDatai(m_i_vr_enabled);


	char lba_acf_descrip[1024];
	XPLMGetDatab(m_s_acf_descrip, lba_acf_descrip, 0, 240);
	m_ls_acf_descrip = lba_acf_descrip;

	if (m_fse_dr_is_connected != NULL)		m_fse_li_is_connected	= XPLMGetDatai(m_fse_dr_is_connected);
	if (m_fse_dr_is_flying != NULL)			m_fse_li_is_flying		= XPLMGetDatai(m_fse_dr_is_flying);
	if (m_fse_dr_can_end_flight != NULL)	m_fse_li_can_end_flight = XPLMGetDatai(m_fse_dr_can_end_flight);

	//m_li_fse_flying = XPLMGetDatai(m_i_fse_flying);

	m_ld_local_x = XPLMGetDatad(m_d_local_x);
	m_ld_local_y = XPLMGetDatad(m_d_local_y);
	m_ld_local_z = XPLMGetDatad(m_d_local_z);

	m_lf_local_phi = XPLMGetDataf(m_f_local_phi)* M_PI / 180.0f;
	m_lf_local_psi = XPLMGetDataf(m_f_local_psi)* M_PI / 180.0f;
	m_lf_local_theta = XPLMGetDataf(m_f_local_theta)* M_PI / 180.0f;

	m_lf_weight_max = XPLMGetDataf(m_f_weight_max);
	m_lf_payload = XPLMGetDataf(m_f_payload);
	m_lf_weight_total = XPLMGetDataf(m_f_weight_total);

	m_li_on_ground = XPLMGetDatai(m_i_on_ground);
	XPLMGetDatavi(m_ia_engines_running, m_lia_engines_running, 0, 2);
	XPLMGetDatavi(m_ia_custom_slider_on, m_lia_custom_slider_on, 0, 24);
	XPLMGetDatavf(m_fa_custom_slider_ratio, m_lfa_custom_slider_ratio, 0, 24);
	m_lf_park_brake = XPLMGetDataf(m_f_park_brake);

	m_lf_climb_rate = XPLMGetDataf(m_f_climb_rate);

	m_li_sim_ground_speed = XPLMGetDatai(m_i_sim_ground_speed);

	m_li_battery_on = XPLMGetDatai(m_i_battery_on);

	m_lf_jett_weight = XPLMGetDataf(m_f_jett_weight);

	XPLMGetDatavf(m_fa_prop_ratio, m_lfa_prop_ratio, 0, 1);


	m_lf_412_hook_x = XPLMGetDataf(m_f_412_hook_x);
	m_lf_412_hook_y = XPLMGetDataf(m_f_412_hook_y);
	m_lf_412_hook_z = XPLMGetDataf(m_f_412_hook_z);
	m_lf_412_hook_cable_extended = XPLMGetDataf(m_f_412_hook_cable_extended);
	m_lf_412_hook_cable_direction = XPLMGetDataf(m_f_412_hook_cable_direction);


}


void HRM_PlugIn::PluginKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey, void * inRefcon, int losingFocus)
{
}

int HRM_PlugIn::PluginMouseClickCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void * inRefcon)
{
	return 0;
}



float HRM_PlugIn::PluginFlightLoopCallback(float elapsedMe, float elapsedSim, int counter, void * refcon)
{
	
	//if (mp_current_mission != NULL) mp_current_mission->SetObjectPosition();

	bool measure_time = false;

	auto time_start = std::chrono::steady_clock::now(); 

	ReadDataFast();

	if ((m_li_paused == 0) && (m_li_replay == 0))
	{
		m_time_delta += elapsedMe;

		// Fast Computations

		if ((m_mission_state == HRM::State_Flight_2))
		{
			if		(abs(pHRM->m_lf_g_forward) > HRM::threshold_gf_high)		m_mission_gf_high += elapsedMe * m_li_sim_ground_speed;
			else if (abs(pHRM->m_lf_g_forward) > HRM::threshold_gf_med)			m_mission_gf_med += elapsedMe * m_li_sim_ground_speed;
			else if (abs(pHRM->m_lf_g_forward) > HRM::threshold_gf_low)			m_mission_gf_low += elapsedMe * m_li_sim_ground_speed;

			if		(abs(pHRM->m_lf_g_side) > HRM::threshold_gs_high)			m_mission_gs_high += elapsedMe * m_li_sim_ground_speed;
			else if (abs(pHRM->m_lf_g_side) > HRM::threshold_gs_med)			m_mission_gs_med += elapsedMe * m_li_sim_ground_speed;
			else if (abs(pHRM->m_lf_g_side) > HRM::threshold_gs_low)			m_mission_gs_low += elapsedMe * m_li_sim_ground_speed;

			if		(abs(pHRM->m_lf_g_normal) > HRM::threshold_gv_pos_high)		m_mission_gv_pos_high += elapsedMe * m_li_sim_ground_speed;
			else if (abs(pHRM->m_lf_g_normal) > HRM::threshold_gv_pos_med)		m_mission_gv_pos_med += elapsedMe * m_li_sim_ground_speed;
			else if (abs(pHRM->m_lf_g_normal) > HRM::threshold_gv_pos_low)		m_mission_gv_pos_low += elapsedMe * m_li_sim_ground_speed;

			if		(abs(pHRM->m_lf_g_normal) < HRM::threshold_gv_neg_high)		m_mission_gv_neg_high += elapsedMe * m_li_sim_ground_speed;
			else if (abs(pHRM->m_lf_g_normal) < HRM::threshold_gv_neg_med)		m_mission_gv_neg_med += elapsedMe * m_li_sim_ground_speed;
			else if (abs(pHRM->m_lf_g_normal) < HRM::threshold_gv_neg_low)		m_mission_gv_neg_low += elapsedMe * m_li_sim_ground_speed;
		}

		// End of Fast Computations

		if (m_time_delta >= m_position_calc_rate)
		{
			measure_time = true;
			
			// If not done yet, check for FSEconomy

			if ((m_cm_enable_fse == true) && (m_fse_dr_is_connected == NULL)) FSERegister();

			ReadDataSlow();

			// Slow Computations

			
			if ((m_mission_state == HRM::State_Create_Mission) && (m_window_visible == true))
			{
				if ((m_sling_enable == true) && (m_sling_load_plugin == HRM::XSlingload) && (m_xslingload_found == false))
				{
					std::ifstream check_file(m_xslingload_ini_path);

					if (check_file.good() == true)
					{
						m_xslingload_found = true;
						m_xslingload_not_found = false;
					}
					else
					{
						m_sling_enable = false;
						m_xslingload_not_found = true;
					}
					check_file.close();
				}

				if ((m_sling_enable == true) && (m_sling_load_plugin == HRM::AB412) && (m_412sar_found == false))
				{
					if (m_patient_off == NULL)		m_patient_off =			XPLMFindCommand("412/buttons/PATIENT_off");
					if (m_patient_ground == NULL)	m_patient_ground =		XPLMFindCommand("412/buttons/PATIENT_on_ground");
					if (m_patient_transit == NULL)	m_patient_transit =		XPLMFindCommand("412/buttons/PATIENT_in_transit");
					if (m_patient_board == NULL)	m_patient_board =		XPLMFindCommand("412/buttons/PATIENT_on_board");
					if (m_operator_on == NULL)		m_operator_on =			XPLMFindCommand("412/buttons/CREW_on");
					if (m_operator_off == NULL)		m_operator_off =		XPLMFindCommand("412/buttons/CREW_off");


					if (m_f_412_hook_x == NULL) m_f_412_hook_x = XPLMFindDataRef("412/winch/hook_x");
					if (m_f_412_hook_y == NULL) m_f_412_hook_y = XPLMFindDataRef("412/winch/hook_y");
					if (m_f_412_hook_z == NULL) m_f_412_hook_z = XPLMFindDataRef("412/winch/hook_z");

					if (m_f_412_hook_cable_extended == NULL) m_f_412_hook_cable_extended = XPLMFindDataRef("412/winch/cable_extendend_m");
					if (m_lf_412_hook_cable_direction == NULL) m_f_412_hook_cable_direction =  XPLMFindDataRef("412/winch/cable_direction");

					if (m_f_412_ext_lat == NULL)	m_f_412_ext_lat =	XPLMFindDataRef("412/external/lat");
					if (m_f_412_ext_long == NULL)	m_f_412_ext_long =	XPLMFindDataRef("412/external/lon");
					if (m_i_412_ext_set == NULL)	m_i_412_ext_set =	XPLMFindDataRef("412/external/set");


					if (m_patient_off != NULL)
					{
						m_412sar_found = true;
						m_412sar_not_found = false;
					}
					else
					{
						m_412sar_found = false;
						m_412sar_not_found = true;
					}
					
				}

				// Find GTN Airport
				if (m_flight_plan_format == HRM::FPL_GTN)
				{
					bool departure_search_finished = false;
					bool departure_navaid_found = false;

					XPLMNavRef nav_departure = XPLM_NAV_NOT_FOUND;

					if (m_cm_departure_icao.size() > 2)
					{
						nav_departure = XPLMFindFirstNavAidOfType(xplm_Nav_Airport);

						XPLMNavType nav_type_departure = xplm_Nav_Airport;


						for (int index = 0; (departure_search_finished == false) && (departure_navaid_found == false); index++)
						{
							if (index == 0) nav_departure = XPLMFindFirstNavAidOfType(xplm_Nav_Airport);
							else nav_departure = XPLMGetNextNavAid(nav_departure);

							if (nav_departure != XPLM_NAV_NOT_FOUND)
							{
								char buffer[2048];

								float lat_s;
								float long_s;
								XPLMGetNavAidInfo(nav_departure, &nav_type_departure, &lat_s, &long_s, NULL, NULL, NULL, buffer, NULL, NULL);

								m_mission_departure_lat = lat_s;
								m_mission_departure_long = long_s;

								if (nav_type_departure != xplm_Nav_Airport)			departure_search_finished = true;
								else if (m_cm_departure_icao.compare(buffer) == 0)	departure_navaid_found = true;

							}
							else
							{
								departure_search_finished = true;
							}
						}
					}

					if ((nav_departure != XPLM_NAV_NOT_FOUND) && (departure_navaid_found == true))
					{
						char buffer[2048];
						XPLMGetNavAidInfo(nav_departure, NULL, &m_mission_departure_lat, &m_mission_departure_long, NULL, NULL, NULL, NULL, buffer, NULL);
						m_mission_departure_icao_name = buffer;
						m_mission_departure_icao_found = true;
					}
					else
					{
						m_mission_departure_icao_found = false;
						m_mission_departure_icao_name = "";
						m_mission_departure_lat = HRM::coord_invalid;
						m_mission_departure_long = HRM::coord_invalid;
					}
				}


				// ICAO Computation
				if (pHRM->m_cm_use_position == HRM::Scenairo_Aircraft)
				{
					m_mission_scenario_lat = m_ld_latitude;
					m_mission_scenario_long = m_ld_longitude;
				}
				// Search for Scenario ICAO
				else
				{
					


					// Find X-Plane Airport 
					if (m_cm_enable_fse == false)
					{
						
						bool scenario_search_finished = false;
						bool scenario_navaid_found = false;

						XPLMNavRef nav_scenario = XPLM_NAV_NOT_FOUND;

						if (m_cm_scenario_icao.size() > 2)
						{
							nav_scenario = XPLMFindFirstNavAidOfType(xplm_Nav_Airport);

							XPLMNavType nav_type_scenario = xplm_Nav_Airport;


							for (int index = 0; (scenario_search_finished == false) && (scenario_navaid_found == false); index++)
							{
								if (index == 0) nav_scenario = XPLMFindFirstNavAidOfType(xplm_Nav_Airport);
								else nav_scenario = XPLMGetNextNavAid(nav_scenario);

								if (nav_scenario != XPLM_NAV_NOT_FOUND)
								{
									char buffer[2048];

									float lat_s;
									float long_s;
									XPLMGetNavAidInfo(nav_scenario, &nav_type_scenario, &lat_s, &long_s, NULL, NULL, NULL, buffer, NULL, NULL);

									m_mission_scenario_lat = lat_s;
									m_mission_scenario_long = long_s;

									if (nav_type_scenario != xplm_Nav_Airport)			scenario_search_finished = true;
									else if (m_cm_scenario_icao.compare(buffer) == 0)	scenario_navaid_found = true;

								}
								else
								{
									scenario_search_finished = true;
								}
							}
						}

						if ((nav_scenario != XPLM_NAV_NOT_FOUND) && (scenario_navaid_found == true))
						{
							char buffer[2048];
							XPLMGetNavAidInfo(nav_scenario, NULL, &m_mission_scenario_lat, &m_mission_scenario_long, NULL, NULL, NULL, NULL, buffer, NULL);
							m_mission_scenario_icao_name = buffer;
							m_mission_scenario_icao_found = true;
						}
						else
						{
							m_mission_scenario_icao_found = false;
							m_mission_scenario_icao_name = "";
							m_mission_scenario_lat = HRM::coord_invalid;
							m_mission_scenario_long = HRM::coord_invalid;
						}
					}
					// Find FSEconomy Airport
					else
					{
						m_mission_scenario_icao_found = false;
						m_mission_scenario_icao_name = "";
						m_mission_scenario_lat = HRM::coord_invalid;
						m_mission_scenario_long = HRM::coord_invalid;

						for (auto p_airport : m_fse_airports)
						{
							if (m_cm_scenario_icao.compare(p_airport->icao) == 0)
							{
								m_mission_scenario_icao_found = true;
								m_mission_scenario_icao_name = p_airport->name;
								m_mission_scenario_lat = p_airport->latitude;
								m_mission_scenario_long = p_airport->longitude;
							}
						}
					}
				}

				// Search for Hospital ICAO

				bool hospital_search_finished = false;
				bool hospital_navaid_found = false;
				bool custom_hospital_found = false;

				XPLMNavRef nav_hospital = XPLM_NAV_NOT_FOUND;

				if (m_cm_hospital_icao.size() > 2)
				{
					

					// Custom Hospitals
					if (m_custom_hospitals.size() > 0)
					{
						for (auto p_airport : m_custom_hospitals)
						{
							if (p_airport->icao.compare(m_cm_hospital_icao) == 0)
							{
								m_mission_hospital_lat = p_airport->latitude;
								m_mission_hospital_long = p_airport->longitude;
								m_mission_hospital_icao_name = p_airport->name;
								m_mission_hospital_icao_found = true;

								custom_hospital_found = true;
							}
						}
					}

					// X-Plane Hospitals
					if (custom_hospital_found == false)
					{
						nav_hospital = XPLMFindFirstNavAidOfType(xplm_Nav_Airport);

						XPLMNavType nav_type_hospital = xplm_Nav_Airport;


						for (int index = 0; (hospital_search_finished == false) && (hospital_navaid_found == false); index++)
						{
							if (index == 0) nav_hospital = XPLMFindFirstNavAidOfType(xplm_Nav_Airport);
							else nav_hospital = XPLMGetNextNavAid(nav_hospital);

							if (nav_hospital != XPLM_NAV_NOT_FOUND)
							{
								char buffer[2048];
								XPLMGetNavAidInfo(nav_hospital, &nav_type_hospital, &m_mission_hospital_lat, &m_mission_hospital_long, NULL, NULL, NULL, buffer, NULL, NULL);

								if (nav_type_hospital != xplm_Nav_Airport)			hospital_search_finished = true;
								else if (m_cm_hospital_icao.compare(buffer) == 0)	hospital_navaid_found = true;

							}
							else
							{
								hospital_search_finished = true;
							}
						}
					}
				}

				if (custom_hospital_found == false)
				{

					if ((nav_hospital != XPLM_NAV_NOT_FOUND) && (hospital_navaid_found == true))
					{
						char buffer[2048];
						XPLMGetNavAidInfo(nav_hospital, NULL, &m_mission_hospital_lat, &m_mission_hospital_long, NULL, NULL, NULL, NULL, buffer, NULL);
						m_mission_hospital_icao_name = buffer;
						m_mission_hospital_icao_found = true;
					}
					else
					{
						m_mission_hospital_icao_found = false;
						m_mission_hospital_icao_name = "";
						m_mission_hospital_lat = HRM::coord_invalid;
						m_mission_hospital_long = HRM::coord_invalid;
					}
				}

				


			}
			else if (m_mission_state == HRM::State_Plan_Flight)
			{

			}
			else if (m_mission_state == HRM::State_Pre_Flight)
			{

				m_mission_preflight_countdown -= m_time_delta;
				if (m_mission_preflight_countdown <= 0)
				{
					m_mission_preflight_countdown = 0;
					MissionStartFlight1();
				}
				if (m_li_on_ground == 0)
				{
					MissionStartFlight1();
				}
			}
			else if (m_mission_state == HRM::State_Flight_1)
			{
				mp_cm_mission->SetObjectPosition();

				if (m_li_on_ground == 0)
				{
					m_mission_flight1_countdown -= m_time_delta * m_li_sim_ground_speed;
					m_mission_flight1_time += m_time_delta * m_li_sim_ground_speed;

					if (m_mission_flight1_countdown <= 0)
					{
						if (m_mission_time_failed == false) IvyPlaySound(10, -1, -1); // Mission Failed

						m_mission_flight1_countdown = 0;
						m_mission_time_failed = true;

						
						
					}

					if (mp_cm_mission->IsSlingLoad() == true)
					{
						if (calc_distance_m(m_ld_latitude, m_ld_longitude, mp_cm_waypoint->latitude, mp_cm_waypoint->longitude) < HRM::pickup_max_distance)
						{
							if ((m_lf_jett_weight > m_xslingload_treshold) && (m_lf_jett_weight_old < m_xslingload_treshold))
							{
								m_mission_at_patient_countdown = m_patient_countdown_value;
								m_mission_state = HRM::State_At_Patient;
							}
						}

					}
				}
				else
				{
					if (mp_cm_mission->IsSlingLoad() == false)
					{
						// If within 100m and collective down or fse can finish
						if (calc_distance_m(m_ld_latitude, m_ld_longitude, mp_cm_waypoint->latitude, mp_cm_waypoint->longitude) < HRM::pickup_max_distance)
						{



							if ((m_lfa_prop_ratio[0] < m_cm_collective_min) && (m_cm_enable_fse == true) && (m_cm_autoconnect_fse == true) && ((FSECanFinish() == true) || (FSEIsFlying() == false)))
							{
								FSEFinishFlight();

								m_mission_at_patient_countdown = m_patient_countdown_value;
								m_mission_state = HRM::State_At_Patient;
								// Load Patient
								IvyPlaySound(5, -1, -1);
							}
							else if ((m_lfa_prop_ratio[0] < m_cm_collective_min) && ((m_cm_autoconnect_fse == false) || (m_cm_enable_fse == false)))
							{
								m_mission_at_patient_countdown = m_patient_countdown_value;
								m_mission_state = HRM::State_At_Patient;
								// Load Patient
								IvyPlaySound(5, -1, -1);
							}




						}
					}
				}
					
				if (mp_cm_mission->IsSlingLoad() == true)
				{

					if (m_sling_load_plugin == HRM::AB412)
					{

						m_412_patient_distance = calc_distance_m(m_ld_latitude, m_ld_longitude, mp_cm_waypoint->latitude, mp_cm_waypoint->longitude);

						if (m_412_probe == NULL) m_412_probe = XPLMCreateProbe(xplm_ProbeY);

						if (m_412_patient_status == HRM::Patient_Off)
						{
							XPLMCommandOnce(m_patient_ground);
							m_412_patient_status = HRM::Patient_Ground;
						}

						if (m_412_patient_distance > 100)
						{
							static bool set_position = true;
							// Set 412 patient position
							if (set_position == true) XPLMSetDatai(m_i_412_ext_set, 1);
							else XPLMCommandOnce(m_patient_ground);
							set_position = !set_position;

						}
						

						if ((m_412_patient_distance > 300) || ((m_412_patient_local_x == 0) && (m_412_patient_local_y == 0) && (m_412_patient_local_z == 0)))
						{
							// Set 412 patient position
							XPLMSetDatai(m_i_412_ext_set, 1);


							// Calculate patient coordinates
							double zero_x, zero_y, zero_z;
							XPLMWorldToLocal(mp_cm_waypoint->latitude, mp_cm_waypoint->longitude, 0, &zero_x, &zero_y, &zero_z);

							double local_x = 0, local_y = 0, local_z = 0;

							local_x = zero_x;
							local_z = zero_z;


							XPLMProbeInfo_t info;
							info.structSize = sizeof(info);

							XPLMProbeResult result = XPLMProbeTerrainXYZ(m_412_probe, zero_x, zero_y, zero_z, &info);

							m_412_patient_local_x = info.locationX;
							m_412_patient_local_y = info.locationY;
							m_412_patient_local_z = info.locationZ;

							XPLMLocalToWorld(m_412_patient_local_x, m_412_patient_local_y, m_412_patient_local_z, &m_412_patient_lat, &m_412_patient_long, &m_412_patient_elev);


						}
						// When close to patient
						else
						{
							// Set Doors
							if ((m_lfa_custom_slider_ratio[1] < 1.0f) && (m_lfa_custom_slider_ratio[3] < 1.0f))
							{
								int door_pos = 1;

								XPLMSetDatavi(m_ia_custom_slider_on, &door_pos, 1, 1);
								XPLMSetDatavi(m_ia_custom_slider_on, &door_pos, 3, 1);
							}
							//Set Operator
							else if (m_412_crew_on == false)
							{
								XPLMCommandOnce(m_operator_on);
								// Operator in Position
								IvyPlaySound(11, -1, -1);
								m_412_crew_on = true;
							}

							// Calculate Winch Position in OpenGL Coordinates
							double x_phi = m_lf_412_hook_x * cos(m_lf_local_phi) + m_lf_412_hook_y * sin(m_lf_local_phi);
							double y_phi = m_lf_412_hook_y * cos(m_lf_local_phi) - m_lf_412_hook_x * sin(m_lf_local_phi);
							double z_phi = m_lf_412_hook_z + 0.3f;
							double x_the = x_phi;
							double y_the = y_phi * cos(m_lf_local_theta) - z_phi * sin(m_lf_local_theta);
							double z_the = z_phi * cos(m_lf_local_theta) + y_phi * sin(m_lf_local_theta);
							double x_winch_offset = x_the * cos(m_lf_local_psi) - z_the * sin(m_lf_local_psi);
							double y_winch_offset = y_the;
							double z_winch_offset = z_the * cos(m_lf_local_psi) + x_the * sin(m_lf_local_psi);

							double x_winch = x_winch_offset + m_ld_local_x;
							double y_winch = y_winch_offset + m_ld_local_y;
							double z_winch = z_winch_offset + m_ld_local_z;

							double op_lat = 0;
							double op_long = 0;
							double op_elev = 0;

							// Calcualte Operator Position in OpenGL and World Coodinates
							XPLMLocalToWorld(x_winch, y_winch, z_winch, &op_lat, &op_long, &op_elev);
							op_elev -= m_lf_412_hook_cable_extended + 1.2;
							XPLMWorldToLocal(op_lat, op_long, op_elev, &x_winch, &y_winch, &z_winch);

							
							// Calculate heading of our patient relative to our aircraft
							float delta_lat = m_412_meter_lat * (op_lat - m_412_patient_lat);
							float delta_long = m_412_meter_long * (op_long - m_412_patient_long);
							float heading = atan2(delta_long, delta_lat) * 180.0f / M_PI;
							m_412_patient_heading = heading - m_lf_heading -180;

							if (m_412_patient_heading < 0) m_412_patient_heading += 360.0f;
							if (m_412_patient_heading < 0) m_412_patient_heading += 360.0f;

							double patient_offset_x = abs(m_412_patient_local_x - x_winch);
							double patient_offset_y = abs(m_412_patient_local_y - y_winch);
							double patient_offset_z = abs(m_412_patient_local_z - z_winch);

							double patient_distance_h = sqrt((patient_offset_x * patient_offset_x) + (patient_offset_z * patient_offset_z));
							
							m_412_patient_distance = sqrt((patient_offset_x * patient_offset_x) + (patient_offset_y * patient_offset_y) + (patient_offset_z * patient_offset_z));

							// Calculate offset of our operator relative to the patient in aircraft coordinates
							m_412_patient_distance_side = patient_distance_h * sin(m_412_patient_heading * M_PI / 180.0f);
							m_412_patient_distance_forward = patient_distance_h * cos(m_412_patient_heading * M_PI / 180.0f);
							m_412_patient_distance_alt = op_elev - m_412_patient_elev;


							// Speech output
							if ((m_412_patient_distance <= m_cm_sling_say_distance) && (m_412_crew_on == true))
							{
								m_cm_say_timer += m_time_delta;

								if (m_cm_say_timer >= m_cm_sling_say_time)
								{
									//m_cm_say_state

									int forward = (int)m_412_patient_distance_forward;
									int side = (int)m_412_patient_distance_side;
									int alt = (int)m_412_patient_distance_alt;



									if (patient_distance_h > m_cm_distance_h_max)
									{
										if (m_cm_say_state == 0)
										{
											if (forward > 0) IvyPlaySound(-1, forward, 13);
											else if (forward < 0) IvyPlaySound(-1, abs(forward), 15);
											else IvyPlaySound(-1, -1, 14);
										}
										else if (m_cm_say_state == 1)
										{
											if (side > 0) IvyPlaySound(-1, side, 18);
											else if (side < 0) IvyPlaySound(-1, abs(side), 16);
											else IvyPlaySound(-1, -1, 17);
										}
									}
									else if (m_412_patient_distance > m_sling_load_distance)
									{
										if (m_412_patient_distance_alt > 0)	IvyPlaySound(-1, abs(alt), 19);
										else if (m_412_patient_distance_alt < 0)	IvyPlaySound(-1, abs(alt), 34);
										else	IvyPlaySound(-1, abs(alt), 20);
									}



									m_cm_say_timer = 0;
									m_cm_say_state++;
									if (m_cm_say_state > 1) m_cm_say_state = 0;

								}
							}


							if (m_412_patient_distance <= m_sling_load_distance)
							{
								if (m_412_patient_loading_time == 0)
								{
									if (m_lf_412_hook_cable_direction != 0)
									{
										// Say stop winch
										IvyPlaySound(20, -1, -1);
									}
									else
									{
										// Say loading patient
										IvyPlaySound(21, -1, -1);
									}
								}
								m_412_patient_loading_time += m_time_delta;

								if (m_412_patient_loading_time >= m_sling_load_time_min)
								{
									m_mission_state = HRM::State_At_Patient;
								}
							}
							else
							{
								if (m_412_patient_loading_time != 0)
								{
									// Loading patient failed
									IvyPlaySound(22, -1, -1);
								}

								m_412_patient_loading_time = 0;
							}


						}

						/*
						

						//if (m_412_patient_distance < 50)
						{

							float delta_lat = -1 * m_412_meter_lat * (m_ld_latitude - mp_cm_waypoint->latitude);
							float delta_long = m_412_meter_long * (m_ld_longitude - mp_cm_waypoint->longitude);


							float heading = atan2(delta_long, delta_lat) * 180.0f / M_PI;

							m_412_patient_heading = heading - m_lf_heading;

							if (m_412_patient_heading < 0) m_412_patient_heading += 360.0f;

							m_412_patient_distance_side = m_412_patient_distance * sin(m_412_patient_heading * M_PI / 180.0f);
							m_412_patient_distance_forward = m_412_patient_distance * cos(m_412_patient_heading * M_PI / 180.0f);


							m_412_patient_distance_alt = 0;
						}


						// Winch Position in OpenGL Coordinates

						double x_phi = m_lf_412_hook_x * cos(m_lf_local_phi) + m_lf_412_hook_y * sin(m_lf_local_phi);
						double y_phi = m_lf_412_hook_y * cos(m_lf_local_phi) - m_lf_412_hook_x * sin(m_lf_local_phi);
						double z_phi = m_lf_412_hook_z + 0.3f;
						double x_the = x_phi;
						double y_the = y_phi * cos(m_lf_local_theta) - z_phi * sin(m_lf_local_theta);
						double z_the = z_phi * cos(m_lf_local_theta) + y_phi * sin(m_lf_local_theta);
						double x_winch_offset = x_the * cos(m_lf_local_psi) - z_the * sin(m_lf_local_psi);
						double y_winch_offset = y_the;
						double z_winch_offset = z_the * cos(m_lf_local_psi) + x_the * sin(m_lf_local_psi);

						double x_winch = x_winch_offset + m_ld_local_x;
						double y_winch = y_winch_offset + m_ld_local_y;
						double z_winch = z_winch_offset + m_ld_local_z;

						double op_lat = 0;
						double op_long = 0;
						double op_elev = 0;


						XPLMLocalToWorld(x_winch, y_winch, z_winch, &op_lat, &op_long, &op_elev);
						op_elev -= m_lf_412_hook_cable_extended + 1.2;
						XPLMWorldToLocal(op_lat, op_long, op_elev, &x_winch, &y_winch, &z_winch);

						// Debug: Draw Object
						XPLMDrawInfo_t		dr;
						dr.structSize = sizeof(dr);
						dr.x = x_winch;
						dr.y = y_winch;
						dr.z = z_winch;
						dr.pitch = 0;
						dr.heading = 0;
						dr.roll = 0;

						static float m_angle = 0;
						XPLMInstanceSetPosition(m_412_inst_ref, &dr, &m_angle);


						// open doors
						int door_pos = 1;

						XPLMSetDatavi(m_ia_custom_slider_on, &door_pos, 1, 1);
						XPLMSetDatavi(m_ia_custom_slider_on, &door_pos, 3, 1);*/


					}




				}
			}
			else if (m_mission_state == HRM::State_At_Patient)
			{
				// for sling load: acf_jett_is_slung
				// create is_slingload for mission

				if (mp_cm_mission->IsSlingLoad() == true)
				{
					if (mp_cm_mission != NULL) mp_cm_mission->RemovePatients();
					MissionStartFlight2();
				}

				else if (m_li_on_ground == 1)
				{
					m_mission_at_patient_countdown -= m_time_delta;
					if (m_mission_at_patient_countdown <= 0)
					{
						if ((m_cm_enable_fse == true) && (m_cm_autoconnect_fse == true))
						{
							FSEStartFlight();
						}

						if (mp_cm_mission != NULL) mp_cm_mission->RemovePatients();
						MissionStartFlight2();
						// Patient on board
						IvyPlaySound(6, -1, -1);
					}
				}
				else
				{
					// Loading Patient failed
					IvyPlaySound(22, -1, -1);
					m_mission_state = HRM::State_Flight_1;
				}

			}
			
			else if (m_mission_state == HRM::State_Flight_2)
			{
				if (m_li_on_ground == 0)
				{
					MissionCalcGFPoints();
					m_mission_flight2_countdown -= m_time_delta * m_li_sim_ground_speed;
					m_mission_flight2_time += m_time_delta * m_li_sim_ground_speed;

					if (m_mission_flight2_countdown <= 0) 
					{
						if (m_mission_time_failed == false) IvyPlaySound(10, -1, -1); // Mission Failed

						m_mission_flight2_countdown = 0;
						m_mission_time_failed = true;
						
					}
				}
				else
				{
					// If within 100m and collective down
					if (calc_distance_m(m_ld_latitude, m_ld_longitude, m_mission_hospital_lat, m_mission_hospital_long) < HRM::hospital_max_distance)
					{
						// Deactivated because x-slingload lets you pick up stuff
						// dropping sling load at hospital automatically finished mission
						/*if ((mp_cm_mission->IsSlingLoad() == true) &&(m_li_jett_is_slung == 0) && (m_li_jett_is_slung_old > 0))
						{
							MissionFinish();
						}
						else */
						// FSE Autoconnect
						//
						if ((m_lfa_prop_ratio[0] < m_cm_collective_min) && (m_cm_enable_fse == true) && (m_cm_autoconnect_fse == true) && ((FSECanFinish() == true) || (FSEIsFlying() == false)))
						{
							FSEFinishFlight();

							m_mission_at_hospital_countdown = m_hospital_countdown_value;
							m_mission_state = HRM::State_At_Hospital;
							
						}

						// Normal Mode
						else if ((m_lfa_prop_ratio[0] < m_cm_collective_min) && ((m_cm_autoconnect_fse == false) || (m_cm_enable_fse == false)))
						{

							m_mission_at_hospital_countdown = m_hospital_countdown_value;
							m_mission_state = HRM::State_At_Hospital;

							// Unloading Patient
							IvyPlaySound(7, -1, -1);
						}
					}
				}

				if ((mp_cm_mission->IsSlingLoad() == true) && (m_sling_load_plugin == HRM::AB412))
				{
					if ((m_lf_412_hook_cable_extended == 0) && (m_412_patient_status == HRM::Patient_Transit))
					{
						XPLMCommandOnce(m_patient_board);
						m_412_patient_status = HRM::Patient_OnBoard;

						XPLMCommandOnce(m_operator_off);
						IvyPlaySound(6, -1, -1);
						m_412_crew_on = false;

						int door_pos = 0;
						XPLMSetDatavi(m_ia_custom_slider_on, &door_pos, 1, 1);
						XPLMSetDatavi(m_ia_custom_slider_on, &door_pos, 3, 1);
					}
					
				}

			}
			else if (m_mission_state == HRM::State_At_Hospital)
			{
				if (m_li_on_ground == 1)
				{
					m_mission_at_hospital_countdown -= m_time_delta;

					if (m_mission_at_hospital_countdown <= 0)
					{
						m_mission_at_hospital_countdown = 0;
						MissionFinish();
					}
				}
				else
				{
					m_mission_state = HRM::State_Flight_2;
				}
			
			}
			else if (m_mission_state == HRM::State_Mission_Finished)
			{
				
			}
			else if (m_mission_state == HRM::State_Mission_Cancelled)
			{
				
			}

			// End of Slow Computations

			m_lf_jett_weight_old = m_lf_jett_weight;


			m_time_delta = 0;
		}
	}

	if (measure_time == true)
	{
		static int time_counter = 0;

		
			

		auto time_end = std::chrono::steady_clock::now();
		int time_value = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start).count();

		if ((time_counter++) > 10)
		{
			m_processing_time = time_value;
			time_counter = 0;
		}
		else
		{
			m_processing_time = std::max(m_processing_time, time_value);
		}
	}

	return m_data_rate;
}

int HRM_PlugIn::ToggleControlWindowCallback(XPLMCommandRef cmd, XPLMCommandPhase phase, void* refcon)
{
	if (phase == xplm_CommandBegin)
	{
		if (!imguiPtr->GetVisible())
		{
			imguiPtr->Visible(true);
		}
		else
		{
			imguiPtr->Visible(false);
		}
	}
	return 1;
}

