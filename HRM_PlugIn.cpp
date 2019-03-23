#include "HRM_PlugIn.h"



HRM_PlugIn::HRM_PlugIn() :
	m_street_missions(),
	m_urban_missions(),
	m_sar_missions(),
	m_sling_missions(),
	m_street_waypoints(),
	m_urban_waypoints(),
	m_sar_waypoints(),
	m_sling_waypoints(),
	m_fse_airports()

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

	m_scenery_file = m_system_path + m_ds + "Resources" + m_ds + "plugins" + m_ds + "HRM" + m_ds + "scenery";
	m_config_path = m_system_path + m_ds + "Resources" + m_ds + "plugins" + m_ds + "HRM" + m_ds;


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

	ReadWaypoints(m_street_waypoints, "street");
	ReadWaypoints(m_urban_waypoints, "urban");
	ReadWaypoints(m_sar_waypoints, "sar");
	ReadWaypoints(m_sling_waypoints, "sling");

	ReadFSEAirports();

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
	m_cm_no_waypoint_found = false;

	mp_cm_waypoint = NULL;
	mp_cm_mission = NULL;

	if (m_mission_scenario_lat == HRM::coord_invalid) return;
	if (m_mission_scenario_long == HRM::coord_invalid) return;
	if (m_mission_hospital_lat == HRM::coord_invalid) return;
	if (m_mission_hospital_long == HRM::coord_invalid) return;

	// Get the random mission type and the corresponding waypoint vector. Bit more complicated since some might not be enabled

	int type_count = 0;

	if (m_street_enable) type_count++;
	if (m_urban_enable) type_count++;
	if (m_sar_enable) type_count++;
	if (m_sling_enable) type_count++;

	if (type_count == 0)
	{
		m_cm_no_waypoint_found = true;
		return;
	}

	int random_type = rand() % type_count;

	std::vector<HRM_Waypoint *> *p_waypoint_vector = NULL;
	std::vector<HRM_Mission *> *p_mission_vector = NULL;

	if ((m_street_enable) && (random_type == 0))
	{
		p_waypoint_vector = &m_street_waypoints;
		p_mission_vector = &m_street_missions;
	}
	else if (!m_street_enable)	random_type++;

	if ((m_urban_enable) && (random_type == 1))
	{
		p_waypoint_vector = &m_urban_waypoints;
		p_mission_vector = &m_urban_missions;
	}
	else if (!m_urban_enable)	random_type++;

	if ((m_sar_enable) && (random_type == 2))
	{
		p_waypoint_vector = &m_sar_waypoints;
		p_mission_vector = &m_sar_missions;
	}
	else if (!m_sar_enable)	random_type++;

	if ((m_sling_enable) && (random_type == 0))
	{
		p_waypoint_vector = &m_sling_waypoints;
		p_mission_vector = &m_sling_missions;
	}
	else if (!m_sling_enable)	random_type++;
		
	if ((p_waypoint_vector == NULL) || (p_mission_vector == NULL))
	{
		m_cm_no_waypoint_found = true;
		return;
	}

	if ((p_waypoint_vector->size() == 0) || (p_mission_vector->size() == 0))
	{
		m_cm_no_waypoint_found = true;
		return;
	}

	std::vector<HRM_Waypoint *> considered_waypoints;

	
	// Check for all waypoints that are within the set distance 

	if (m_cm_enable_fse == false)
	{
		// MIN-MAX Distance 
		for (auto p_waypoint : *p_waypoint_vector)
		{
			double distance = abs(calc_distance_nm(m_mission_scenario_lat, m_mission_scenario_long, p_waypoint->latitude, p_waypoint->longitude));

			if ((distance >= m_cm_min_distance) && (distance <= m_cm_max_distance))
			{
				considered_waypoints.push_back(p_waypoint);
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
		for (auto p_waypoint : *p_waypoint_vector)
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

	// When no Waypoint in Range, Set Error and Return
	if (considered_waypoints.size() == 0)
	{
		m_cm_no_waypoint_found = true;
		return;
	}

	// Find Random Mission and Waypoint

	int random_waypoint = rand() % considered_waypoints.size();
	int random_mission = rand() % p_mission_vector->size();

	mp_cm_waypoint = considered_waypoints.at(random_waypoint);
	mp_cm_mission = p_mission_vector->at(random_mission);

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

	// TODO: Create Flightplan

	pHRM->m_mission_state = HRM::State_Plan_Flight;

	m_cm_creation_failed = false;
	m_cm_no_waypoint_found = false;
}

void HRM_PlugIn::MissionStart()
{
	pHRM->m_mission_state = HRM::State_Create_Mission;

}

void HRM_PlugIn::MissionReset()
{
}

void HRM_PlugIn::MissionCancel()
{
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

void HRM_PlugIn::ReadWaypoints(std::vector<HRM_Waypoint*>& waypoint_vector, std::string file_name)
{
	for (int scenery_number = 1; scenery_number <= HRM::max_scenery; scenery_number++)
	{
		try
		{
			std::ifstream waypoint_file(m_config_path + file_name + "_" + std::to_string(scenery_number) + ".fms");
			std::string line_string;

			bool odd_waypoint = true;
			HRM_Waypoint *p_waypoint = new HRM_Waypoint();
			


			while (std::getline(waypoint_file, line_string))
			{
				std::stringstream line_stream(line_string);

				std::string dummy_string;
				double dummy_double;
				
				int line_code = 0;
				line_stream >> line_code;

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
						// Position Waypoint
						if (odd_waypoint == true)
						{
							p_waypoint->latitude = way_lat;
							p_waypoint->longitude = way_long;

							odd_waypoint = false;
						}
						// Direction Waypoint
						else
						{
							p_waypoint->lat_heading = way_lat;
							p_waypoint->long_heading = way_long;

							waypoint_vector.push_back(p_waypoint);

							p_waypoint = new HRM_Waypoint();
							odd_waypoint = true;

						}
					}
				}

				
			}

			delete p_waypoint;

		}
		catch (...)
		{

		}

	}

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
			//return;
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

