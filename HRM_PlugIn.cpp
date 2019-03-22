#include "HRM_PlugIn.h"



HRM_PlugIn::HRM_PlugIn() :
	m_street_missions(),
	m_urban_missions(),
	m_sar_missions(),
	m_sling_missions(),
	m_street_waypoints(),
	m_urban_waypoints(),
	m_sar_waypoints(),
	m_sling_waypoints()

{
	
}


HRM_PlugIn::~HRM_PlugIn()
{
	
}



void HRM_PlugIn::PluginStart()
{
	

	m_ds = XPLMGetDirectorySeparator();

	char buffer[2048];
	XPLMGetSystemPath(buffer);
	m_system_path = buffer;

	m_scenery_file = m_system_path + m_ds + "Resources" + m_ds + "plugins" + m_ds + "HRM_MissionEditor" + m_ds + "scenery";


	srand(time(NULL));

	

	XPLMRegisterFlightLoopCallback(WrapFlightLoopCallback, 0.1, 0);

	// Menu;
	m_PluginMenu = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "HRM", 0, 1);
	m_PluginMenuID = XPLMCreateMenu("HRM", XPLMFindPluginsMenu(), m_PluginMenu, WrapMenuHandler, 0);
	XPLMAppendMenuItem(m_PluginMenuID, "Toggle Control Window", "Item1", 1);

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
												550 - 400, 
												decorate);  // calls constructor
	//imguiPtr-> // calls ImgWindow::init from the base class which in turn virtually calls the overridden function
	
	//XPLMCreateProbe()

	imguiPtr->configureImguiContext();

	HRMDebugString("Lookup Datarefs");

	m_d_latitude = XPLMFindDataRef("sim/flightmodel/position/latitude");
	m_d_longitude = XPLMFindDataRef("sim/flightmodel/position/longitude");
	m_f_heading = XPLMFindDataRef("sim/flightmodel/position/psi");

	m_f_g_normal = XPLMFindDataRef("sim/flightmodel/forces/g_nrml");
	m_f_g_forward = XPLMFindDataRef("sim/flightmodel/forces/g_axil");
	m_f_g_side = XPLMFindDataRef("sim/flightmodel/forces/g_side");

	m_i_paused = XPLMFindDataRef("sim/time/paused");
	m_i_replay = XPLMFindDataRef("sim/operation/prefs/replay_mode");

	m_i_fse_flying = XPLMFindDataRef("fse/status/flying");
	
	m_f_weight_max = XPLMFindDataRef("sim/aircraft/weight/acf_m_max");
	m_f_payload = XPLMFindDataRef("sim/flightmodel/weight/m_fixed");
	m_f_weight_total = XPLMFindDataRef("sim/flightmodel/weight/m_total");

	m_i_on_ground = XPLMFindDataRef("sim/flightmodel/failures/onground_any");
	m_fa_engines_running = XPLMFindDataRef("sim/flightmodel/engine/ENGN_running");
	m_f_park_brake = XPLMFindDataRef("sim/cockpit2/controls/parking_brake_ratio");

	m_f_climb_rate = XPLMFindDataRef("sim/flightmodel/position/vh_ind_fpm");

	m_i_sim_ground_speed = XPLMFindDataRef("sim/time/ground_speed");

	m_i_battery_on = XPLMFindDataRef("sim/cockpit/electrical/battery_on");

	m_fa_prop_ratio = XPLMFindDataRef("sim/cockpit2/engine/actuators/prop_ratio");

	m_f_pitch = XPLMFindDataRef("sim/flightmodel/position/theta");
	m_f_roll = XPLMFindDataRef("sim/flightmodel/position/phi");

	ReadMissions();

	m_initialized = true;
}



void HRM_PlugIn::PluginStop()
{

	
	if (m_initialized == false) return;

	XPLMUnregisterFlightLoopCallback(WrapFlightLoopCallback, 0);

	XPLMDestroyMenu(m_PluginMenuID);


	//m_ivyConfig->WriteConfig();

	//if (m_ivyConfig) delete m_ivyConfig;

	

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

void HRM_PlugIn::MissionCreate()
{
	m_cm_creation_failed = true;

	if (m_mission_scenario_lat == HRM::coord_invalid) return;
	if (m_mission_scenario_long == HRM::coord_invalid) return;
	if (m_mission_hospital_lat == HRM::coord_invalid) return;
	if (m_mission_hospital_long == HRM::coord_invalid) return;

	m_cm_creation_failed = false;
	m_cm_no_waypoint_found = false;
}

void HRM_PlugIn::MissionStart()
{
}

void HRM_PlugIn::MissionReset()
{
}

void HRM_PlugIn::MissionCancel()
{
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
	for (int scenery_number = 1; scenery_number <= HRM::max_scenery; scenery_number++)
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
			return;
		}
		if (file_found)
		{
			int mission_counter = 0;
			bool mission_created = true;

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
	m_li_paused = XPLMGetDatai(m_i_paused);
	m_li_replay = XPLMGetDatai(m_i_replay);

	m_li_fse_flying = XPLMGetDatai(m_i_fse_flying);

	m_lf_weight_max = XPLMGetDataf(m_f_weight_max);
	m_lf_payload = XPLMGetDataf(m_f_payload);
	m_lf_weight_total = XPLMGetDataf(m_f_weight_total);

	m_li_on_ground = XPLMGetDatai(m_i_on_ground);
	XPLMGetDatavf(m_fa_engines_running, m_lfa_engines_running, 0, 2);
	m_lf_park_brake = XPLMGetDataf(m_f_park_brake);

	m_lf_climb_rate = XPLMGetDataf(m_f_climb_rate);

	m_li_sim_ground_speed = XPLMGetDatai(m_i_sim_ground_speed);

	m_li_battery_on = XPLMGetDatai(m_i_battery_on);

	XPLMGetDatavf(m_fa_prop_ratio, m_lfa_prop_ratio, 0, 1);
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

	ReadDataFast();

	if ((m_li_paused == 0) && (m_li_replay == 0))
	{
		m_time_delta += elapsedMe;

		// Fast Computations

		// End of Fast Computations

		if (m_time_delta >= m_position_calc_rate)
		{
			ReadDataSlow();

			// Slow Computations

			
			if (m_mission_state == HRM::State_Create_Mission)
			{
				if (pHRM->m_cm_use_position == HRM::Scenairo_Aircraft)
				{
					m_mission_scenario_lat = m_ld_latitude;
					m_mission_scenario_long = m_ld_longitude;
				}
				// Search for Scenario ICAO
				else
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
								XPLMGetNavAidInfo(nav_scenario, &nav_type_scenario, &m_mission_scenario_lat, &m_mission_scenario_long, NULL, NULL, NULL, buffer, NULL, NULL);

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

				// Search for Hospital ICAO

				bool hospital_search_finished = false;
				bool hospital_navaid_found = false;

				XPLMNavRef nav_hospital = XPLM_NAV_NOT_FOUND;

				if (m_cm_hospital_icao.size() > 2)
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
			else if (m_mission_state == HRM::State_Plan_Flight)
			{

			}
			else if (m_mission_state == HRM::State_Pre_Flight)
			{

			}
			else if (m_mission_state == HRM::State_Flight_1)
			{

			}
			else if (m_mission_state == HRM::State_At_Patient)
			{

			}
			else if (m_mission_state == HRM::State_Patient_Loaded)
			{

			}
			else if (m_mission_state == HRM::State_Flight_2)
			{

			}
			else if (m_mission_state == HRM::State_Mission_Finished)
			{

			}
			else if (m_mission_state == HRM::State_Mission_Cancelled)
			{

			}

			// End of Slow Computations

			m_time_delta = 0;
		}
	}

	return m_data_rate;
}

