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

#include "HRMImguiWidget.h"
#include "HRM_PlugIn.h"
#include "HRM_Mission.h"
#include <boost/range/adaptor/reversed.hpp>

//#include "MyIvyConfigAircraft.h"

#include <iomanip> // setprecision
#include <sstream> // stringstream

#include <misc/cpp/imgui_stdlib.h>


HRMImguiWidget::HRMImguiWidget(HRM_PlugIn *pHRMNew, int left, int top, int right, int bot, int decoration) :
	ImgWindow(left, top, right, bot, decoration)
{
	pHRM = pHRMNew;
	SetWindowTitle("Helicopter Rescue Missions");
	SetVisible(false);
	//configureImguiContext();

	pHRM->m_window_visible = false;
}


HRMImguiWidget::~HRMImguiWidget()
{
}

void HRMImguiWidget::configureImguiContext()
{
	//ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontDefault();
	/*font2 = io.Fonts->AddFontFromFileTTF("./Resources/fonts/DejaVuSans.ttf", 8.0f);
	font3 = io.Fonts->AddFontFromFileTTF("./Resources/fonts/DejaVuSans.ttf", 10.0f);
	font4 = io.Fonts->AddFontFromFileTTF("./Resources/fonts/DejaVuSans.ttf", 12.0f);
	font5 = io.Fonts->AddFontFromFileTTF("./Resources/fonts/DejaVuSansMono.ttf", 8.0f);
	font6 = io.Fonts->AddFontFromFileTTF("./Resources/fonts/DejaVuSansMono.ttf", 16.0f);
	font7 = io.Fonts->AddFontFromFileTTF("./Resources/fonts/DejaVuSansMono.ttf", 18.0f);
	font8 = io.Fonts->AddFontFromFileTTF("./Resources/fonts/Inconsolata.ttf", 8.0f);
	font9 = io.Fonts->AddFontFromFileTTF("./Resources/fonts/Inconsolata.ttf", 22.0f);
	font10 = io.Fonts->AddFontFromFileTTF("./Resources/fonts/Inconsolata.ttf", 24.0f);*/

}

void HRMImguiWidget::Visible(bool visible)
{
	pHRM->m_window_visible = visible;
	SetVisible(visible);
	if (visible)
	{
		//output_dh = XPLMGetDataf(pHRM->m_f_decision_height);

		selected_radio = 0;

		/*for (int index = 0; index < pHRM->m_ivyConfig->m_audio_dirs.size(); index++)
		{
			if (pHRM->m_ivyConfig->m_audio_dirs[index].compare(pHRM->m_ivyConfig->m_mp3_dir) == 0)
				selected_radio = index;
		}*/
	}
}

void HRMImguiWidget::buildInterface()
{
	win_width = ImGui::GetWindowWidth();
	win_height = ImGui::GetWindowHeight();

	ImVec4 color_red = ImColor(255, 0, 0);
	ImVec4 color_orange = ImColor(255, 125, 0);
	ImVec4 color_yellow = ImColor(255, 255, 0);
	ImVec4 color_green = ImColor(0, 255, 0);

	//static int radio_path = 0;

	ImGui::Text(std::to_string(pHRM->m_processing_time).c_str());

	if (pHRM->m_mission_state == HRM::State_Create_Mission)
	{
		
		ImGui::Columns(2, 0, true);

		ImGui::SetColumnWidth(-1, 180);

		ImGui::Text("Difficulty:"); 
		if (ImGui::RadioButton("Easy", pHRM->m_difficutly == HRM::Easy))		pHRM->m_difficutly = HRM::Easy;
		if (ImGui::RadioButton("Normal", pHRM->m_difficutly == HRM::Normal))	pHRM->m_difficutly = HRM::Normal;
		if (ImGui::RadioButton("Hard", pHRM->m_difficutly == HRM::Hard))		pHRM->m_difficutly = HRM::Hard;

		ImGui::Checkbox("Street Accidents", &(pHRM->m_street_enable));
		ImGui::Checkbox("Urban Accidents", &(pHRM->m_urban_enable));
		ImGui::Checkbox("Search and Rescue", &(pHRM->m_sar_enable));
		ImGui::Checkbox("Sling Line", &(pHRM->m_sling_enable));

		//if (pHRM->m_sling_enable == true)
		{
			if (ImGui::RadioButton("X-Slingload", pHRM->m_sling_load_plugin == HRM::XSlingload))	pHRM->m_sling_load_plugin = HRM::XSlingload;
			if (ImGui::RadioButton("AB 412", pHRM->m_sling_load_plugin == HRM::AB412))				pHRM->m_sling_load_plugin = HRM::AB412;

		}

		if ((pHRM->m_xslingload_not_found == true) && (pHRM->m_sling_load_plugin == HRM::XSlingload))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			ImGui::Text("X-Slingload not found");
			ImGui::PopStyleColor();
		}

		if ((pHRM->m_412sar_not_found == true) && (pHRM->m_sling_load_plugin == HRM::AB412))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			ImGui::Text("AB 412 not found");
			ImGui::PopStyleColor();
		}

		if (pHRM->m_sling_load_plugin == HRM::AB412)
		{
			ImGui::PushItemWidth(80);
			ImGui::Text("Patient Pickup");
			ImGui::SliderFloat("Distance [m]", &(pHRM->m_sling_load_distance),0.1,10,"%.1f");
			ImGui::SliderFloat("Time [s]    ", &(pHRM->m_sling_load_time_min), 0.0, 10, "%.1f");
			ImGui::PopItemWidth();
		}

		ImGui::Text("FPL Format:");
		if (ImGui::RadioButton("XP11", pHRM->m_flight_plan_format == HRM::FPL_XP11))		pHRM->m_flight_plan_format = HRM::FPL_XP11;
		if (ImGui::RadioButton("XP10", pHRM->m_flight_plan_format == HRM::FPL_XP10))		pHRM->m_flight_plan_format = HRM::FPL_XP10;
		if (ImGui::RadioButton("GTN GFP", pHRM->m_flight_plan_format == HRM::FPL_GTN))		pHRM->m_flight_plan_format = HRM::FPL_GTN;

		

		ImGui::Checkbox("Adjust Payload", &(pHRM->m_adjust_payload));

		ImGui::NextColumn();

		ImGui::Text("Scenario Position:");

		if (ImGui::RadioButton("Aircraft Position", pHRM->m_cm_use_position == HRM::Scenairo_Aircraft))
			pHRM->m_cm_use_position = HRM::Scenairo_Aircraft;


		if (ImGui::RadioButton("Use ICAO", pHRM->m_cm_use_position == HRM::Scenario_ICAO))
			pHRM->m_cm_use_position = HRM::Scenario_ICAO;

		if (pHRM->m_cm_use_position == HRM::Scenario_ICAO)
		{

			//ImGui::SameLine();
			ImGui::PushItemWidth(100);
			ImGui::InputText("Scenario ICAO", &(pHRM->m_cm_scenario_icao));
			ImGui::PopItemWidth();

			if (pHRM->m_cm_use_position == HRM::Scenario_ICAO)
			{
				if (pHRM->m_mission_scenario_icao_found == true)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, color_green);
					ImGui::Text(pHRM->m_mission_scenario_icao_name.c_str());
					ImGui::PopStyleColor();
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Text, color_red);
					ImGui::Text("ICAO NOT Found");
					ImGui::PopStyleColor();
				}

			}

			ImGui::Checkbox("FSEconomy", &(pHRM->m_cm_enable_fse));
			ImGui::Checkbox("FSE Autoconnect", &(pHRM->m_cm_autoconnect_fse));
		}
		else
		{
			pHRM->m_cm_enable_fse = false;
		}

		if (pHRM->m_cm_enable_fse == false)
		{
			ImGui::PushItemWidth(100);
			ImGui::InputInt("Min Distance [nm]", &(pHRM->m_cm_min_distance), 1, 1);
			//ImGui::SameLine();
			ImGui::InputInt("Max Distance [nm]", &(pHRM->m_cm_max_distance), 1, 1);
			ImGui::PopItemWidth();
		}

		ImGui::Checkbox("Panic Call", &(pHRM->m_cm_estmimated_wp));

		if (pHRM->m_cm_estmimated_wp == true)
		{
			ImGui::PushItemWidth(100);
			ImGui::SliderInt("Search Range [m]", &(pHRM->m_cm_estimated_radius_m), HRM::search_range_min, HRM::search_range_max);
			ImGui::PopItemWidth();
		}
		

		ImGui::PushItemWidth(100);
		ImGui::InputText("Hospital ICAO", &(pHRM->m_cm_hospital_icao));
		ImGui::PopItemWidth();

		if (pHRM->m_mission_hospital_icao_found == true)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_green);
			ImGui::Text(pHRM->m_mission_hospital_icao_name.c_str());
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			ImGui::Text("ICAO NOT Found");
			ImGui::PopStyleColor();
		}

		/*ImGui::Columns(1, 0, true);
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Columns(2, 0, true);*/

		ImGui::Text("Global Waypoint Folder:");

		int column = 1;

		for (int index = 0; index < pHRM->m_path_vector.size(); index++)
		{
			std::string folder_name = pHRM->m_path_vector[index];

			/*if ((index >= (pHRM->m_path_vector.size() / 2)) && (column == 1))
			{
				ImGui::NextColumn();
				column = 2;
				ImGui::Text("   ");
			}*/
			if (ImGui::RadioButton(folder_name.c_str(), pHRM->m_global_path_index == index))
			{
				pHRM->m_global_path_index = index;
				pHRM->m_global_path = folder_name;
			}

		}

		ImGui::Columns(1, 0, true);

		if (pHRM->m_flight_plan_format == HRM::FPL_GTN)
		{
			ImGui::Spacing();
			ImGui::InputText("Dep ICAO", &(pHRM->m_cm_departure_icao));
			if (pHRM->m_mission_departure_icao_found == true)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, color_green);
				ImGui::Text(pHRM->m_mission_departure_icao_name.c_str());
				ImGui::PopStyleColor();
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, color_red);
				ImGui::Text("ICAO NOT Found");
				ImGui::PopStyleColor();
			}


			ImGui::InputText("GFP Path", &(pHRM->m_gfp_path));
			ImGui::InputText("WPT Path", &(pHRM->m_wpt_path));
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::PushItemWidth(50);
		ImGui::InputText("ICAO", &(pHRM->m_custom_icao));
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("Save ACF Location as Hospital", ImVec2(230, 20)))
		{
			pHRM->AddCustomICAO();
		}

		if (pHRM->m_custom_icao_exists == true)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_yellow);
			ImGui::Text("ICAO position updated");
			ImGui::PopStyleColor();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		//ImGui::PushItemWidth(100);
		if (ImGui::Button("Create Mission", ImVec2(410, 20)))
		{
			pHRM->MissionCreate();
		}

		if (pHRM->m_cm_creation_failed)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			ImGui::Text("Could Not Create Mission");
			ImGui::PopStyleColor();
		}

		if (pHRM->m_cm_no_waypoint_found)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			ImGui::Text("No Waypoint Found");
			ImGui::PopStyleColor();
		}
		//ImGui::PopItemWidth();
		

	}
	else if (pHRM->m_mission_state == HRM::State_Plan_Flight)
	{
		ImGui::Text("Mission Status: Flight Planning");
		ImGui::Separator();

		ImGui::PushStyleColor(ImGuiCol_Text, color_green);
		ImGui::TextWrapped("You have now time for detailed flight planning.");
		ImGui::TextWrapped("The mission flight plan was saved under following path:");
		if (pHRM->m_flight_plan_format == HRM::FPL_GTN)		ImGui::TextWrapped(pHRM->m_gfp_path.c_str());
		else												ImGui::TextWrapped(pHRM->m_fms_file.c_str());
		ImGui::TextWrapped("Feel free to load it into your flight planning software to create a more detailed route.");
		ImGui::PopStyleColor();

		

		ImGui::Separator();

		ImGui::Text("Mission Name:");
		ImGui::TextWrapped(pHRM->mp_cm_mission->m_name.c_str());
		ImGui::Separator();
		ImGui::Text("Mission Objective:");
		ImGui::TextWrapped(pHRM->mp_cm_mission->m_start_text.c_str());  // Bei 50 abschneiden
		ImGui::Separator();
		if (pHRM->m_cm_estmimated_wp)
		{
			ImGui::TextWrapped("The calling party was in panic and only gave us a rough estimate for the position");
			ImGui::Separator();
		}
		//ImGui::Text(std::to_string(pHRM->mp_cm_mission->m_ld_latitude).c_str());
		//ImGui::Text(std::to_string(pHRM->mp_cm_mission->m_ld_longitude).c_str());
		//ImGui::Text(std::to_string(pHRM->mp_cm_mission->m_lf_heading).c_str());

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (pHRM->m_xslingload_reload_position_file == true)
		{

			ImGui::PushStyleColor(ImGuiCol_Text, color_yellow);
			ImGui::TextWrapped("IMPORTANT:");
			ImGui::TextWrapped("You need to manually re-initialize X-Slingload Position File. Do this as close to the rescue scene as possible.");
			ImGui::TextWrapped("Plugins->X-Hoist->Init Sling Loading");
			ImGui::PopStyleColor();

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

		}

		if (ImGui::Button("Start Mission", ImVec2(410, 20)))
		{
			pHRM->m_cm_cancelling = false;
			pHRM->MissionStart();
		}

		if (pHRM->m_cm_not_on_ground == true)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			ImGui::Text("Aircraft already Airborne");
			ImGui::PopStyleColor();
		}
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		if (pHRM->m_cm_cancelling == false)
		{
			if (ImGui::Button("Cancel Mission", ImVec2(410, 20)))
			{
				pHRM->m_cm_cancelling = true;
			}
		}
		else
		{
			if (ImGui::Button("Are you sure?", ImVec2(200, 20)))
			{
				pHRM->MissionCancel();
				pHRM->m_cm_cancelling = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Continue", ImVec2(200, 20)))
			{
				pHRM->m_cm_cancelling = false;
			}
		}

		


	}
	else if (pHRM->m_mission_state == HRM::State_Pre_Flight)
	{
		ImGui::Text("Mission Status: Aircraft Startup");
		
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Mission Objective:");
		ImGui::TextWrapped(pHRM->mp_cm_mission->m_start_text.c_str());  // Bei 50 abschneiden
		ImGui::Separator();
		if (pHRM->m_cm_estmimated_wp)
		{
			ImGui::TextWrapped("The calling party was in panic and only gave us a rough estimate for the position");
			ImGui::Separator();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();


		if (pHRM->m_mission_preflight_countdown < 15) ImGui::PushStyleColor(ImGuiCol_Text, color_red);
		else if (pHRM->m_mission_preflight_countdown < 30) ImGui::PushStyleColor(ImGuiCol_Text, color_yellow);
		else ImGui::PushStyleColor(ImGuiCol_Text, color_green);

		ImGui::Text("Pre-Flight Time Remaining: ");
		ImGui::SameLine();
		ImGui::Text(HRM_PlugIn::CreateTimeString(pHRM->m_mission_preflight_countdown).c_str());

		ImGui::PopStyleColor();

		// Cancel Button

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		if (pHRM->m_cm_cancelling == false)
		{
			if (ImGui::Button("Cancel Mission", ImVec2(410, 20)))
			{
				pHRM->m_cm_cancelling = true;
			}
		}
		else
		{
			if (ImGui::Button("Are you sure?", ImVec2(200, 20)))
			{
				pHRM->MissionCancel();
				pHRM->m_cm_cancelling = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Continue", ImVec2(200, 20)))
			{
				pHRM->m_cm_cancelling = false;
			}
		}

	}
	else if (pHRM->m_mission_state == HRM::State_Flight_1)
	{
		ImGui::Text("Mission Status: Flight to Patient");


		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Mission Objective:");
		if (pHRM->m_mission_time_failed == false)
		{
			ImGui::TextWrapped(pHRM->mp_cm_mission->m_start_text.c_str());  
			ImGui::Separator();
			if (pHRM->m_cm_estmimated_wp)
			{
				ImGui::TextWrapped("The calling party was in panic and only gave us a rough estimate for the position");
				ImGui::Separator();
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

		
			if (pHRM->m_mission_flight1_countdown < 15) ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			else if (pHRM->m_mission_flight1_countdown < 30) ImGui::PushStyleColor(ImGuiCol_Text, color_yellow);
			else ImGui::PushStyleColor(ImGuiCol_Text, color_green);

			ImGui::Text("Flight Time Remaining: ");
			ImGui::SameLine();
			ImGui::Text(HRM_PlugIn::CreateTimeString(pHRM->m_mission_flight1_countdown).c_str());

			ImGui::PopStyleColor();

			if (pHRM->mp_cm_mission->IsSlingLoad() == true)
			{
				if (pHRM->m_sling_load_plugin == HRM::AB412)
				{
					if (pHRM->m_412_patient_distance <= pHRM->m_cm_sling_say_distance)
					{
						ImGui::Spacing();
						ImGui::Spacing();
																							ImGui::Text("                         Alt: %+3.1f", pHRM->m_412_patient_distance_alt);
						ImGui::Spacing();
						ImGui::Spacing();
						if (pHRM->m_412_patient_distance_forward > 0)						ImGui::Text("                %3.1f", pHRM->m_412_patient_distance_forward);
						else																ImGui::Text("                -----");
						ImGui::Spacing();
						if (pHRM->m_412_patient_distance_side < 0)							ImGui::Text("         %3.1f         -----", abs(pHRM->m_412_patient_distance_side));
						if (pHRM->m_412_patient_distance_side >= 0)							ImGui::Text("         -----         %3.1f", abs(pHRM->m_412_patient_distance_side));
						ImGui::Spacing();
						if (pHRM->m_412_patient_distance_forward < 0)						ImGui::Text("                %3.1f", abs(pHRM->m_412_patient_distance_forward));
						else																ImGui::Text("                -----");
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Text("Dist:    %3.1f", pHRM->m_412_patient_distance);
						ImGui::Text("Time     %3.1f", pHRM->m_412_patient_loading_time);
					}

					/*ImGui::Text("Heading: %3.1f", pHRM->m_412_patient_heading);
					ImGui::Text("Dist:    %3.1f", pHRM->m_412_patient_distance);
					ImGui::Text("Forward: %3.1f", pHRM->m_412_patient_distance_forward);
					ImGui::Text("Side:    %3.1f", pHRM->m_412_patient_distance_side);
					ImGui::Text("Alt:     %3.1f", pHRM->m_412_patient_distance_alt);
					ImGui::Text("Time     %3.1f", pHRM->m_412_patient_loading_time);*/

					
				}
			}
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			ImGui::TextWrapped(pHRM->mp_cm_mission->m_failed_arr_text.c_str());
			ImGui::PopStyleColor();

			ImGui::Separator();
			if (pHRM->m_cm_estmimated_wp)
			{
				ImGui::TextWrapped("The calling party was in panic and only gave us a rough estimate for the position");
				ImGui::Separator();
			}
		}

		// Cancel Button

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		if (pHRM->m_cm_cancelling == false)
		{
			if (ImGui::Button("Cancel Mission", ImVec2(410, 20)))
			{
				pHRM->m_cm_cancelling = true;
			}
		}
		else
		{
			if (ImGui::Button("Are you sure?", ImVec2(200, 20)))
			{
				pHRM->MissionCancel();
				pHRM->m_cm_cancelling = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Continue", ImVec2(200, 20)))
			{
				pHRM->m_cm_cancelling = false;
			}
		}

		// m_mission_time_failed
		// patient not picked up
	}
	else if (pHRM->m_mission_state == HRM::State_At_Patient)
	{
		// Cancel Button

		ImGui::Text("Mission Status: Patient Pickup");

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::PushStyleColor(ImGuiCol_Text, color_green);
		ImGui::TextWrapped("Our medical crew is picking up the patient. Stay put!");
		ImGui::PopStyleColor();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Pickup Time Remaining: ");
		ImGui::SameLine();
		ImGui::Text(HRM_PlugIn::CreateTimeString(pHRM->m_mission_at_patient_countdown).c_str());

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		if (pHRM->m_cm_cancelling == false)
		{
			if (ImGui::Button("Cancel Mission", ImVec2(410, 20)))
			{
				pHRM->m_cm_cancelling = true;
			}
		}
		else
		{
			if (ImGui::Button("Are you sure?", ImVec2(200, 20)))
			{
				pHRM->MissionCancel();
				pHRM->m_cm_cancelling = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Continue", ImVec2(200, 20)))
			{
				pHRM->m_cm_cancelling = false;
			}
		}
	}
	else if (pHRM->m_mission_state == HRM::State_Flight_2)
	{
		ImGui::Text("Mission Status: Flight to Hospital");

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Mission Objective:");
		if (pHRM->m_mission_time_failed == false)
		{
			ImGui::TextWrapped(pHRM->mp_cm_mission->m_pickup_text.c_str());

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();


			if (pHRM->m_mission_flight2_countdown < 15) ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			else if (pHRM->m_mission_flight2_countdown < 30) ImGui::PushStyleColor(ImGuiCol_Text, color_yellow);
			else ImGui::PushStyleColor(ImGuiCol_Text, color_green);

			ImGui::Text("Flight Time Remaining: ");
			ImGui::SameLine();
			ImGui::Text(HRM_PlugIn::CreateTimeString(pHRM->m_mission_flight2_countdown).c_str());

			ImGui::PopStyleColor();

			ImGui::Separator();

			ImGui::Text("Patient Comfort Level: ");

			if		(abs(pHRM->m_lf_g_forward) < HRM::threshold_gf_low)		ImGui::PushStyleColor(ImGuiCol_Text, color_green);
			else if (abs(pHRM->m_lf_g_forward) < HRM::threshold_gf_med)		ImGui::PushStyleColor(ImGuiCol_Text, color_yellow);
			else if (abs(pHRM->m_lf_g_forward) < HRM::threshold_gf_high)	ImGui::PushStyleColor(ImGuiCol_Text, color_orange);
			else															ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			ImGui::Text("Forces Forward/Backward");
			ImGui::PopStyleColor();

			if		(abs(pHRM->m_lf_g_side) < HRM::threshold_gs_low)		ImGui::PushStyleColor(ImGuiCol_Text, color_green);
			else if (abs(pHRM->m_lf_g_side) < HRM::threshold_gs_med)		ImGui::PushStyleColor(ImGuiCol_Text, color_yellow);
			else if (abs(pHRM->m_lf_g_side) < HRM::threshold_gs_high)		ImGui::PushStyleColor(ImGuiCol_Text, color_orange);
			else															ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			ImGui::Text("Forces Sideways");
			ImGui::PopStyleColor();

			if		((pHRM->m_lf_g_normal) < HRM::threshold_gv_pos_low)		ImGui::PushStyleColor(ImGuiCol_Text, color_green);
			else if ((pHRM->m_lf_g_normal) < HRM::threshold_gv_pos_med)		ImGui::PushStyleColor(ImGuiCol_Text, color_yellow);
			else if ((pHRM->m_lf_g_normal) < HRM::threshold_gv_pos_high)	ImGui::PushStyleColor(ImGuiCol_Text, color_orange);
			else															ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			ImGui::Text("Forces Downwards");
			ImGui::PopStyleColor();

			if		((pHRM->m_lf_g_normal) > HRM::threshold_gv_neg_low)		ImGui::PushStyleColor(ImGuiCol_Text, color_green);
			else if ((pHRM->m_lf_g_normal) > HRM::threshold_gv_neg_med)		ImGui::PushStyleColor(ImGuiCol_Text, color_yellow);
			else if ((pHRM->m_lf_g_normal) > HRM::threshold_gv_neg_high)	ImGui::PushStyleColor(ImGuiCol_Text, color_orange);
			else															ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			ImGui::Text("Forces Upwards");
			ImGui::PopStyleColor();

		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			ImGui::TextWrapped(pHRM->mp_cm_mission->m_failed_hosp_text.c_str());
			ImGui::PopStyleColor();
		}

		// Cancel Button

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		if (pHRM->m_cm_cancelling == false)
		{
			if (ImGui::Button("Cancel Mission", ImVec2(410, 20)))
			{
				pHRM->m_cm_cancelling = true;
			}
		}
		else
		{
			if (ImGui::Button("Are you sure?", ImVec2(200, 20)))
			{
				pHRM->MissionCancel();
				pHRM->m_cm_cancelling = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Continue", ImVec2(200, 20)))
			{
				pHRM->m_cm_cancelling = false;
			}
		}
	}
	else if (pHRM->m_mission_state == HRM::State_At_Hospital)
	{

		ImGui::Text("Mission Status: Unloading Patient");

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::PushStyleColor(ImGuiCol_Text, color_green);
		ImGui::TextWrapped("Our medical crew unloading the patient. Stay put!");
		ImGui::PopStyleColor();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Unloading Time Remaining: ");
		ImGui::SameLine();
		ImGui::Text(HRM_PlugIn::CreateTimeString(pHRM->m_mission_at_hospital_countdown).c_str());

		// Cancel Button

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		if (pHRM->m_cm_cancelling == false)
		{
			if (ImGui::Button("Cancel Mission", ImVec2(410, 20)))
			{
				pHRM->m_cm_cancelling = true;
			}
		}
		else
		{
			if (ImGui::Button("Are you sure?", ImVec2(200, 20)))
			{
				pHRM->MissionCancel();
				pHRM->m_cm_cancelling = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Continue", ImVec2(200, 20)))
			{
				pHRM->m_cm_cancelling = false;
			}
		}
	}
	else if (pHRM->m_mission_state == HRM::State_Mission_Finished)
	{
		ImGui::Text("Mission Status: Finished");

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (pHRM->m_mission_time_failed == false)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_green);
			ImGui::TextWrapped(pHRM->mp_cm_mission->m_end_text.c_str());
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			ImGui::TextWrapped("You did not make it in time.");
			ImGui::PopStyleColor();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::PushItemWidth(150);
			
		ImGui::Text("Flight1 Time:");
		ImGui::SameLine();
		ImGui::Text(HRM_PlugIn::CreateTimeString(pHRM->m_mission_flight1_time).c_str());
			
		ImGui::Text("Flight2 Time:");
		ImGui::SameLine();
		ImGui::Text(HRM_PlugIn::CreateTimeString(pHRM->m_mission_flight2_time).c_str());

		ImGui::Text("Flight1 Avg Speed [kt]:");
		ImGui::SameLine();
		ImGui::Text(std::to_string(pHRM->m_mission_flight1_avg_speed).c_str());

		ImGui::Text("Flight2 Avg Speed [kt]:");
		ImGui::SameLine();
		ImGui::Text(std::to_string(pHRM->m_mission_flight2_avg_speed).c_str());

		ImGui::PopItemWidth();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Difficulty Level:");
		ImGui::SameLine();
		ImGui::SetCursorPosX(250);
		ImGui::Text("%3i/%3i", pHRM->m_mission_points_difficulty, HRM::points_difficulty);

		ImGui::Text("Search Range:");
		ImGui::SameLine();
		ImGui::SetCursorPosX(250);
		ImGui::Text("%3i/%3i", pHRM->m_mission_points_search_range, HRM::points_search_range);

		ImGui::Text("Flight1 Speed:");
		ImGui::SameLine();
		ImGui::SetCursorPosX(250);
		ImGui::Text("%3i/%3i", pHRM->m_mission_points_flight1, HRM::points_speed_flight1);

		ImGui::Text("Flight2 Speed:");
		ImGui::SameLine();
		ImGui::SetCursorPosX(250);
		ImGui::Text("%3i/%3i", pHRM->m_mission_points_flight2, HRM::points_speed_flight2);

		ImGui::Text("Patient Comfort Level:");
		ImGui::SameLine();
		ImGui::SetCursorPosX(250);
		ImGui::Text("%3i/%3i", pHRM->m_mission_points_g_force, HRM::points_g_flight2);

		ImGui::Text("Total Points:");
		ImGui::SameLine();
		ImGui::SetCursorPosX(250);
		ImGui::Text("%3i/%3i", pHRM->m_mission_points_total, 100);

		////////////////////////////
		/*

		ImGui::Text("Difficulty Level:");
		ImGui::SameLine();
		ImGui::Text(std::to_string(pHRM->m_mission_points_difficulty).c_str());
		ImGui::SameLine();
		ImGui::Text("/");
		ImGui::SameLine();
		ImGui::Text(std::to_string(HRM::points_difficulty).c_str());

		ImGui::Text("Flight1 Speed:");
		ImGui::SameLine();
		ImGui::Text(std::to_string(pHRM->m_mission_points_flight1).c_str());
		ImGui::SameLine();
		ImGui::Text("/ ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(HRM::points_speed_flight1).c_str());
			
		ImGui::Text("Flight2 Speed:");
		ImGui::SameLine();
		ImGui::Text(std::to_string(pHRM->m_mission_points_flight2).c_str());
		ImGui::SameLine();
		ImGui::Text("/ ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(HRM::points_speed_flight2).c_str());

		ImGui::Text("Passenger Comfort Level:");
		ImGui::SameLine();
		ImGui::Text(std::to_string(pHRM->m_mission_points_g_force).c_str());
		ImGui::SameLine();
		ImGui::Text("/ ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(HRM::points_g_flight2).c_str());

		ImGui::Text("Total Points:");
		ImGui::SameLine();
		ImGui::Text(std::to_string(pHRM->m_mission_points_total).c_str());
		ImGui::SameLine();
		ImGui::Text("/100");*/


		

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (pHRM->m_cm_cancelling == false)
		{
			if (ImGui::Button("End Mission", ImVec2(410, 20)))
			{
				pHRM->m_cm_cancelling = true;
			}
		}
		else
		{
			if (ImGui::Button("Are you sure?", ImVec2(200, 20)))
			{
				pHRM->MissionCancel();
				pHRM->m_cm_cancelling = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Continue", ImVec2(200, 20)))
			{
				pHRM->m_cm_cancelling = false;
			}
		}


	}
	else if (pHRM->m_mission_state == HRM::State_Mission_Cancelled)
	{

	}

	//FSEconomy Status
	if (pHRM->m_cm_enable_fse == true)
	{
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (pHRM->FSEIsConnected())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_green);
			ImGui::TextWrapped("FSE Connected");
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			ImGui::TextWrapped("FSE NOT Connected");
			ImGui::PopStyleColor();
		}

		if (pHRM->FSEIsFlying())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_green);
			ImGui::TextWrapped("FSE Flying");
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			ImGui::TextWrapped("FSE NOT Flying");
			ImGui::PopStyleColor();
		}

		if ((pHRM->m_li_on_ground == 1) &&(pHRM->FSECanFinish()))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_green);
			ImGui::TextWrapped("FSE Can Finish Flight");
			ImGui::PopStyleColor();
		}
		else if (pHRM->m_li_on_ground == 1)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, color_red);
			ImGui::TextWrapped("FSE Cannot Finish Flight");
			ImGui::PopStyleColor();
		}
	}

	if (pHRM->m_li_vr_enabled > 0)
	{
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (pHRM->m_li_sim_ground_speed == 1)	ImGui::PushStyleColor(ImGuiCol_Text, color_green);
		else									ImGui::PushStyleColor(ImGuiCol_Text, color_yellow);
			
		ImGui::Text("Ground Speed: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(pHRM->m_li_sim_ground_speed).c_str());
		ImGui::PopStyleColor();

	}

	

	

	

	/*ImGui::Columns(4, 0, false);


	

	ImGui::InputInt("Scenery N r", &(pHRM->m_scenery_number), 1, 1);

	if (ImGui::Button("Save All"))
	{
		pHRM->SaveMissions();

	}

	if (ImGui::Button("Read ALL"))
	{
		pHRM->ReadMissions();

	}

	ImGui::Separator();

	static std::vector<HRM_Mission *> *p_mission_vector = &(pHRM->m_street_missions);
	static HRM_Mission * p_mission = NULL;
	static HRM_Mission * p_mission_old = NULL;
	static int selected_mission_type = 0;
	static int mission_listbox_item_current = 0;

	//ImGui::PushFont(font10);
	pHRM->mp_current_mission = NULL;

	if (ImGui::RadioButton("Street Missions", selected_mission_type == 0))
	{
		p_mission_vector = &(pHRM->m_street_missions);
		selected_mission_type = 0;
	}

	if (ImGui::RadioButton("Urban Missions", selected_mission_type == 1))
	{
		p_mission_vector = &(pHRM->m_urban_missions);
		selected_mission_type = 1;
	}

	if (ImGui::RadioButton("SAR Missions", selected_mission_type == 2))
	{
		p_mission_vector = &(pHRM->m_sar_missions);
		selected_mission_type = 2;
	}

	if (ImGui::RadioButton("Sling Missions", selected_mission_type == 3))
	{
		p_mission_vector = &(pHRM->m_sling_missions); 
		selected_mission_type = 3;
	}

	if (p_mission_vector->size() > 0)
	{
		const char* mission_listbox_items[1024];
		

		for (int index = 0; index < p_mission_vector->size(); index++)
		{
			mission_listbox_items[index] = p_mission_vector->at(index)->m_name.c_str();
		}

		ImGui::ListBox("Missions", &mission_listbox_item_current, mission_listbox_items, p_mission_vector->size(), 10);

		if (ImGui::Button("Delete Mission"))
		{
			HRM_Mission *p_del = p_mission_vector->at(mission_listbox_item_current);
			p_mission_vector->erase(p_mission_vector->begin() + mission_listbox_item_current);

			p_del->RemoveMission();

			delete p_del;
			mission_listbox_item_current = 0;
		}

	}

	if (ImGui::Button("New Mission"))
	{
		HRM_Mission *p_new = new HRM_Mission();

		p_mission_vector->push_back(p_new);

		p_new->m_mission_type = selected_mission_type;

	}

	

	ImGui::NextColumn();

	if (p_mission_vector->size() > 0)
	{
		if (mission_listbox_item_current < p_mission_vector->size())
		{
			p_mission = p_mission_vector->at(mission_listbox_item_current);
			pHRM->mp_current_mission = p_mission;

			p_mission->SetPosition(pHRM->m_ld_latitude, pHRM->m_ld_longitude, pHRM->m_lf_heading);

			if ((p_mission != p_mission_old) && (p_mission_old != NULL)) p_mission_old->RemoveMission();
			p_mission_old = p_mission;

			ImGui::InputText("M Name", &(p_mission->m_name));
			ImGui::InputText("M Start", &(p_mission->m_start_text));
			ImGui::InputText("M Pickup", &(p_mission->m_pickup_text));
			ImGui::InputText("M End", &(p_mission->m_end_text));
			ImGui::InputText("M Arr Failed", &(p_mission->m_failed_arr_text));
			ImGui::InputText("M Hosp Failed", &(p_mission->m_failed_hosp_text));

			ImGui::InputInt("S Start", &(p_mission->m_sound_start), 1, 1);
			ImGui::InputInt("S Arr", &(p_mission->m_sound_arr), 1, 1);
			ImGui::InputInt("S Pickup", &(p_mission->m_sound_pickup), 1, 1);
			ImGui::InputInt("S End", &(p_mission->m_sound_end), 1, 1);
			ImGui::InputInt("SF Arr", &(p_mission->m_sound_failed_arr), 1, 1);
			ImGui::InputInt("SF Hosp", &(p_mission->m_sound_failed_hops), 1, 1);

			ImGui::NextColumn();

			const char* object_listbox_items[1024];

			static int object_listbox_item_current = 0;


			for (int index = 0; index < p_mission->m_object_vector.size(); index++)
			{
				object_listbox_items[index] = p_mission->m_object_vector.at(index)->m_obj_path.c_str();
			}

			ImGui::ListBox("Objects", &object_listbox_item_current, object_listbox_items, p_mission->m_object_vector.size(), 10);

			if (ImGui::Button("Delete Object"))
			{
				HRM_Object *p_del = p_mission->m_object_vector.at(object_listbox_item_current);
				p_mission->m_object_vector.erase(p_mission->m_object_vector.begin() + object_listbox_item_current);

				p_del->DestroyInstance();

				delete p_del;
				object_listbox_item_current = 0;
			}

			if (ImGui::Button("New Object"))
			{
				HRM_Object *p_new = new HRM_Object();

				p_mission->m_object_vector.push_back(p_new);

				p_mission->RemoveMission();
				p_mission->DrawMission();

			}

			if (ImGui::Button("Reset Position to ACF"))
			{
				pHRM->UpdatePosition();

				p_mission->RemoveMission();
				p_mission->DrawMission();
			}

			if (ImGui::Button("Redraw Mission"))
			{
				p_mission->RemoveMission();
				p_mission->DrawMission();
			}



			ImGui::NextColumn();

			if (object_listbox_item_current < p_mission->m_object_vector.size())
			{

				HRM_Object *p_HRM_obj = p_mission->m_object_vector.at(object_listbox_item_current);

				if (p_HRM_obj)
				{
					
					ImGui::InputText("O Path", &(p_HRM_obj->m_obj_path));

					//ImGui::InputDouble("Angle", &(p_HRM_obj->m_zero_angle), 1, 1, 0, 0);
					//ImGui::InputDouble("Dist", &(p_HRM_obj->m_zero_distance), 1, 1, 0, 0);

					ImGui::InputDouble("X [m]:", &(p_HRM_obj->m_dist_x), 1, 1, 0, 0);
					ImGui::InputDouble("Y [m]:", &(p_HRM_obj->m_dist_y), 1, 1, 0, 0);

					ImGui::InputDouble("Elev", &(p_HRM_obj->m_elevation), 0.1, 0.1, 0, 0);

					ImGui::InputDouble("Heading", &(p_HRM_obj->m_heading), 1, 1, 0, 0);
					ImGui::InputDouble("Pitch", &(p_HRM_obj->m_pitch), 1, 1, 0, 0);
					ImGui::InputDouble("Roll", &(p_HRM_obj->m_roll), 1, 1, 0, 0);

					ImGui::Checkbox("IsPatient", &(p_HRM_obj->m_is_patient));

					if (ImGui::Button("Set Pos", ImVec2(180, 20)))
					{
						p_HRM_obj->SetPositionCart(pHRM->m_ld_latitude, pHRM->m_ld_longitude, pHRM->m_lf_heading);
					}

				}
			}


		}
	}
	
	//ImGui::PopFont();



	/*
	const char* object_listbox_items[1024];

	static int object_listbox_item_current = 0;
	

	for (int index = 0; index < pHRM->m_object_vector.size(); index++)
	{
		object_listbox_items[index] = pHRM->m_object_vector.at(index)->m_obj_path.c_str();
	}

	ImGui::ListBox("listbox\n(single select)", &object_listbox_item_current, object_listbox_items, pHRM->m_object_vector.size(), 10);
	
	if (object_listbox_item_current >= pHRM->m_object_vector.size()) return;

	HRM_Object *p_HRM_obj = pHRM->m_object_vector.at(object_listbox_item_current);
	
	win_width = ImGui::GetWindowWidth();
	win_height = ImGui::GetWindowHeight();

	ImGui::Text("Hello World NR 2");

	if (p_HRM_obj)
	{
		ImGui::Text(p_HRM_obj->m_obj_path.c_str());
		ImGui::InputDouble("Angle", &(p_HRM_obj->m_zero_angle), 1, 1, 0, 0);
		ImGui::InputDouble("Dist", &(p_HRM_obj->m_zero_distance), 1, 1, 0, 0);
		ImGui::InputDouble("Elev", &(p_HRM_obj->m_elevation), 0.1, 0.1, 0, 0);

		ImGui::InputDouble("Heading", &(p_HRM_obj->m_heading), 1, 1, 0, 0);
		ImGui::InputDouble("Pitch", &(p_HRM_obj->m_pitch), 1, 1, 0, 0);
		ImGui::InputDouble("Roll", &(p_HRM_obj->m_roll), 1, 1, 0, 0);

		if (ImGui::Button("Set Pos", ImVec2(180, 20)))
		{
			p_HRM_obj->SetPosition(pHRM->m_ld_latitude, pHRM->m_ld_longitude, pHRM->m_lf_heading);
		}

	}

	

	/*ImGui::Spacing();
	ImGui::Columns(6, 0, false);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
	if (ImGui::Button("MyFlight", ImVec2(150, 20))) selected_tab = 1;
	ImGui::NextColumn();
	if (ImGui::Button("Graphs", ImVec2(150, 20))) selected_tab = 2;
	ImGui::NextColumn();
	if (ImGui::Button("Areas of Concern", ImVec2(150, 20))) selected_tab = 3;
	ImGui::NextColumn();
	if (ImGui::Button("Logbook", ImVec2(150, 20))) selected_tab = 4;
	ImGui::NextColumn();
	if (ImGui::Button("Aircraft Configuration", ImVec2(180, 20))) selected_tab = 6;
	ImGui::NextColumn();
	if (ImGui::Button("Settings", ImVec2(150, 20))) selected_tab = 5;
	ImGui::Columns(1);
	ImGui::PopStyleVar();
	ImGui::Separator();
	if (selected_tab == 1) {
		ImGui::Columns(2, 0, true);
		ImGui::SetColumnWidth(-1, 150);
		ImGui::Text("Call-Outs:");
		//for (int i = 0; i < (cols - 1); i++) ImGui::NextColumn();
		ImGui::Checkbox(" 60 kt", &(pHRM->m_ivyConfig->m_kt60_enabled));
		ImGui::Checkbox(" 80 kt", &(pHRM->m_ivyConfig->m_kt80_enabled));
		ImGui::Checkbox("100 kt", &(pHRM->m_ivyConfig->m_kt100_enabled));

		//ImGui::NextColumn();
		ImGui::Text(" ");
		ImGui::InputInt("V1", &(pHRM->m_ivyAircraft->m_li_v1), 1, 1);
		
		
		ImGui::InputInt("VR", &(pHRM->m_ivyAircraft->m_li_vr), 1, 1);
		
		
		
		ImGui::InputInt("V2", &(pHRM->m_ivyAircraft->m_li_v2), 1, 1);

		

		output_dh = XPLMGetDataf(pHRM->m_f_decision_height);
		
		//output_dh = XPLMGetDataf(pHRM->m_f_decision_height);
		ImGui::InputFloat("DH", &output_dh,1,1,0,0);
		XPLMSetDataf(pHRM->m_f_decision_height, output_dh);
		
		ImGui::NextColumn();
		//const char* listbox_items[1000];

		ImGui::Text("Errors :");

		for (auto line : pHRM->m_error_list)
		{
			ImGui::Text(line.c_str());
		}
		
		//static int listbox_item_current = 0;
		//listbox_item_current = 0;
		
		//ImGui::ListBox("", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 20);
	}

	// Show Graphs
	if (selected_tab == 2)
	{
		ImGui::Columns(2, 0, true);
		ImGui::SetColumnWidth(-1, 200);
		ImGui::Spacing();

		if (ImGui::Button("Altitude", ImVec2(180, 20))) selected_graph = 1;
		if (ImGui::Button("Climb Rate", ImVec2(180, 20))) selected_graph = 2;
		if (ImGui::Button("G-Forces Vertical", ImVec2(180, 20))) selected_graph = 3;
		if (ImGui::Button("G-Forces Forward", ImVec2(180, 20))) selected_graph = 4;
		if (ImGui::Button("G-Forces Side", ImVec2(180, 20))) selected_graph = 5;

		ImGui::NextColumn();

		// Altitude
		if (selected_graph == 1)
		{
			float *p_values = pHRM->m_graph_altitude;

			float max_value = 0;
			float min_value = p_values[0];
			for (int index = 0; index < MAX_GRAPH_DATA; index++)
			{
				if (max_value < p_values[index])
					max_value = p_values[index];

				if (min_value > p_values[index])
					min_value = p_values[index];
			}

			ImGui::PlotLines("", p_values, MAX_GRAPH_DATA, 0, "Altitude", min_value - 1, max_value + 1, ImVec2(win_width - 300, win_height - 100));

			std::string text = std::to_string((int)max_value) + " ft";
			ImGui::SetCursorScreenPos(ImVec2(win_width - 90, 40));
			ImGui::Text(text.c_str());

			text = std::to_string((int)min_value) + " ft";
			ImGui::SetCursorScreenPos(ImVec2(win_width - 90, win_height - 75));
			ImGui::Text(text.c_str());
		}
		// Climb Rate
		else if (selected_graph == 2)
		{
			float *p_values = pHRM->m_graph_climb;

			float max_value = 0;
			float min_value = p_values[0];

			float abs_max = 0;

			for (int index = 0; index < MAX_GRAPH_DATA; index++)
			{
				if (max_value < p_values[index])
					max_value = p_values[index];

				if (min_value > p_values[index])
					min_value = p_values[index];
			}

			abs_max = abs(max_value);
			if (abs_max < abs(min_value))
				abs_max = abs(min_value);

			max_value = abs_max;
			min_value = -1 * abs_max;

			ImGui::PlotLines("", p_values, MAX_GRAPH_DATA, 0, "Climb Rate", min_value - 1, max_value + 1, ImVec2(win_width - 300, win_height - 100));

			std::string text = std::to_string((int)max_value) + " ft/min";
			ImGui::SetCursorScreenPos(ImVec2(win_width - 90, 40));
			ImGui::Text(text.c_str());

			text = std::to_string((int)min_value) + " ft/min";
			ImGui::SetCursorScreenPos(ImVec2(win_width - 90, win_height - 75));
			ImGui::Text(text.c_str());
		}
		// G Vertical
		else if (selected_graph == 3)
		{
			float *p_values = pHRM->m_graph_g_vert;

			float max_value = 0;
			float min_value = p_values[0];

			float abs_max = 0;

			for (int index = 0; index < MAX_GRAPH_DATA; index++)
			{
				if (max_value < p_values[index])
					max_value = p_values[index];

				if (min_value > p_values[index])
					min_value = p_values[index];
			}

			abs_max = abs(max_value);
			if (abs_max < abs(min_value))
				abs_max = abs(min_value);

			max_value = abs_max + 0.1;
			min_value = -1 * abs_max - 0.1;

			ImGui::PlotLines("", p_values, MAX_GRAPH_DATA, 0, "G Forces Vertical", min_value, max_value, ImVec2(win_width - 300, win_height - 100));

			std::stringstream text;
			text << std::fixed << std::setprecision(2) << max_value << " g";
			ImGui::SetCursorScreenPos(ImVec2(win_width - 90, 40));
			ImGui::Text(text.str().c_str());

			text.str("");
			text << std::fixed << std::setprecision(2) << min_value << " g";
			ImGui::SetCursorScreenPos(ImVec2(win_width - 90, win_height - 75));
			ImGui::Text(text.str().c_str());
		}
		// Forward G
		else if (selected_graph == 4)
		{
			float *p_values = pHRM->m_graph_g_horiz;

			float max_value = 0;
			float min_value = p_values[0];

			float abs_max = 0;

			for (int index = 0; index < MAX_GRAPH_DATA; index++)
			{
				if (max_value < p_values[index])
					max_value = p_values[index];

				if (min_value > p_values[index])
					min_value = p_values[index];
			}

			abs_max = abs(max_value);
			if (abs_max < abs(min_value))
				abs_max = abs(min_value);

			max_value = abs_max + 0.1;
			min_value = -1 * abs_max - 0.1;

			ImGui::PlotLines("", p_values, MAX_GRAPH_DATA, 0, "G Forces Forward", min_value, max_value, ImVec2(win_width - 300, win_height - 100));

			std::stringstream text;
			text << std::fixed << std::setprecision(2) << max_value << " g";
			ImGui::SetCursorScreenPos(ImVec2(win_width - 90, 40));
			ImGui::Text(text.str().c_str());

			text.str("");
			text << std::fixed << std::setprecision(2) << min_value << " g";
			ImGui::SetCursorScreenPos(ImVec2(win_width - 90, win_height - 75));
			ImGui::Text(text.str().c_str());
		}
		// Side G
		else if (selected_graph == 5)
		{
			float *p_values = pHRM->m_graph_g_side;

			float max_value = 0;
			float min_value = p_values[0];

			float abs_max = 0;

			for (int index = 0; index < MAX_GRAPH_DATA; index++)
			{
				if (max_value < p_values[index])
					max_value = p_values[index];

				if (min_value > p_values[index])
					min_value = p_values[index];
			}

			abs_max = abs(max_value);
			if (abs_max < abs(min_value))
				abs_max = abs(min_value);

			max_value = abs_max + 0.1;
			min_value = -1 * abs_max - 0.1;

			ImGui::PlotLines("", p_values, MAX_GRAPH_DATA, 0, "G Forces Sideways", min_value, max_value, ImVec2(win_width - 300, win_height - 100));

			std::stringstream text;
			text << std::fixed << std::setprecision(2) << max_value << " g left";
			ImGui::SetCursorScreenPos(ImVec2(win_width - 90, 40));
			ImGui::Text(text.str().c_str());

			text.str("");
			text << std::fixed << std::setprecision(2) << min_value << " g right";
			ImGui::SetCursorScreenPos(ImVec2(win_width - 90, win_height - 75));
			ImGui::Text(text.str().c_str());
		}



	}
	// Logbook
	if (selected_tab == 4)
	{
		for (auto line : boost::adaptors::reverse(pHRM->m_logbook_entries))
		{
			ImGui::Text(line.c_str());
		}
		
	}

	// Areas of Concern
	if (selected_tab == 3)
	{
		ImGui::Columns(2, 0, true);
		ImGui::SetColumnWidth(-1, 350);

		ImGui::Text("Areas of Concern:");
		ImGui::Separator();

		ImVec4 col = ImColor(255, 0, 0);
		ImGui::PushStyleColor(ImGuiCol_Text, col);
		for (auto ivyObj : pHRM->m_aoc_list_high)
			ImGui::Text(ivyObj->m_error_string.c_str());
		ImGui::PopStyleColor();

		col = ImColor(255, 255, 0);
		ImGui::PushStyleColor(ImGuiCol_Text, col);
		for (auto ivyObj : pHRM->m_aoc_list_med)
			ImGui::Text(ivyObj->m_error_string.c_str());
		ImGui::PopStyleColor();

		col = ImColor(0, 255, 0);
		ImGui::PushStyleColor(ImGuiCol_Text, col);
		for (auto ivyObj : pHRM->m_aoc_list_low)
			ImGui::Text(ivyObj->m_error_string.c_str());
		ImGui::PopStyleColor();

		ImGui::NextColumn();

		ImGui::PlotHistogram("", pHRM->m_ivy_error_count_history, MAX_ERROR_HISTORY, 0, "Number of Errors per Flight", 0, pHRM->m_ivy_error_count_history_max, ImVec2(win_width - 400, win_height - 100));
		ImGui::SetCursorPosX(362);
		ImGui::Text("0");
		ImGui::SameLine();
		ImGui::SetCursorPosX(win_width - 60);
		ImGui::Text(std::to_string(MAX_ERROR_HISTORY - 1).c_str());



	}

	// Settings
	if (selected_tab == 5)
	{
		ImGui::Columns(3, 0, true);
		ImGui::SetColumnWidth(-1, 350);
		ImGui::Text("Audio Settings:");

		ImGui::Spacing();

		for (int index = 0; index < pHRM->m_ivyConfig->m_audio_names.size(); index++)
		{
			if (ImGui::RadioButton(pHRM->m_ivyConfig->m_audio_names[index].c_str(), selected_radio == index)) {
				selected_radio = index;
			}
		}
		ImGui::Spacing();

		if (ImGui::Button("Apply Audio Settings", ImVec2(250, 20)))
		{
			pHRM->m_ivyConfig->m_mp3_dir = pHRM->m_ivyConfig->m_audio_dirs[selected_radio];
			pHRM->m_ivyConfig->SetAudioDirectory();
			pHRM->IvyLoadSoundFiles(true);
		}

		ImGui::NextColumn();

		ImGui::Checkbox("Enable Ivy", &(pHRM->m_ivyConfig->m_ivy_enable));
		ImGui::Checkbox("Enable Callouts", &(pHRM->m_ivyConfig->m_callouts_enable));
		ImGui::Checkbox("Enable Errors", &(pHRM->m_ivyConfig->m_errors_enable));
		ImGui::Checkbox("Enable Pre-Warnings", &(pHRM->m_ivyConfig->m_pre_warnings));
		ImGui::Checkbox("Enable Ouch", &(pHRM->m_ivyConfig->m_ouch_enabled));

		ImGui::Spacing();

		ImGui::Checkbox("Enable Screaming", &(pHRM->m_ivyConfig->m_passengers_screaming));
		ImGui::Checkbox("Enable Applause", &(pHRM->m_ivyConfig->m_passengers_applause));

		//ImGui::Checkbox("Enable Errors", &(pHRM->m_ivyConfig->m_p));


	}

	// Aircraft Configuration
	if (selected_tab == 6)
	{
		if (pHRM->m_ivyAircraft->m_aircraft_number == 0)
		{
			static bool create_failed = false;

			ImGui::Spacing();
			ImGui::Text("Unconfigured Aircraft");

			if (ImGui::Button("Create New Aircraft Configuration", ImVec2(250, 20)))
			{
				MyIvyConfigAircraft *p_ivyAircraft = new MyIvyConfigAircraft(pHRM->m_ivyConfig->m_config_path.c_str(),0, pHRM->m_ls_acf_descrip);

				if (p_ivyAircraft->m_aircraft_number == 0)
				{
					delete p_ivyAircraft;
					create_failed = true;
				}
				else
				{
					pHRM->m_ivy_aircraft_list->push_back(p_ivyAircraft);
					p_ivyAircraft->WriteConfigFile();
					pHRM->m_ivyAircraft = p_ivyAircraft;
				}
			}

			if (create_failed)
			{
				ImGui::Spacing();
				ImGui::Text("Aircraft Creation Failed");
			}
		}
		else
		{
			ImGui::Columns(3, 0, true);
			ImGui::SetColumnWidth(-1, 300);
			ImGui::Spacing();
			ImGui::Text("Aircraft Configuration #");
			ImGui::SameLine();
			ImGui::Text(std::to_string(pHRM->m_ivyAircraft->m_aircraft_number).c_str());
			ImGui::Text("Aircraft Name: ");
			
			ImGui::TextWrapped(pHRM->m_ivyAircraft->m_name);
			//ImGui::Separator();
			ImGui::Spacing();
			

			ImGui::InputInt("V1 Static         ", &(pHRM->m_ivyAircraft->m_li_v1),1,1);
			ImGui::InputInt("VR Static         ", &(pHRM->m_ivyAircraft->m_li_vr), 1, 1);
			ImGui::InputInt("V2 Static         ", &(pHRM->m_ivyAircraft->m_li_v2), 1, 1);

			ImGui::Checkbox("Dynamic V-Speeds", &(pHRM->m_ivyAircraft->m_vspeeds_enabled));
			ImGui::InputText("V1 Dataref", &(pHRM->m_ivyAircraft->m_lx_v1_data_ref));
			ImGui::InputText("VR Dataref", &(pHRM->m_ivyAircraft->m_lx_vr_data_ref));
			ImGui::InputText("V2 Dataref", &(pHRM->m_ivyAircraft->m_lx_v2_data_ref));

			ImGui::PushItemWidth(200);

			if (ImGui::Button("Update Datarefs",ImVec2(280,25)))
			{
				pHRM->m_ivyAircraft->InitDataRefs();
			}

			if (ImGui::Button("Write Configuration File", ImVec2(280, 25)))
			{
				pHRM->m_ivyAircraft->WriteConfigFile();
			}
			ImGui::PopItemWidth();

			ImGui::NextColumn();

			ImGui::SetColumnWidth(-1, 500);
			
			
			ImGui::PushItemWidth(100);
			ImGui::Checkbox("Enable Slats", &(pHRM->m_ivyAircraft->m_slats_enabled));
			//ImGui::SameLine();
			ImGui::InputFloat("Slats Tolerance", &(pHRM->m_ivyAircraft->m_slats_tolerance), 0.01, 0.01, 2, 0);
			ImGui::SameLine();
			ImGui::InputFloat("Slats Value", &(pHRM->m_ivyAircraft->m_lf_slats), 0.01, 0.01, 2, ImGuiInputTextFlags_ReadOnly);

			ImGui::PopItemWidth();
			ImGui::PushItemWidth(350);
			ImGui::InputText("SDataRef", &(pHRM->m_ivyAircraft->m_lf_slats_data_ref));
			ImGui::PopItemWidth();
			
			//ImGui::Spacing();
						
			for (int index = 0; index < IVY_FS_MAX; index++)
			{
				char buffer[1024];

				ImGui::PushItemWidth(100);
				sprintf(buffer, "Pos S%2i", index + 1);
				ImGui::InputInt(buffer, &(pHRM->m_ivyAircraft->m_slats_deploy_pos[index]), 1, 1, 0);

				ImGui::SameLine();

				sprintf(buffer, "Val S%2i", index + 1);
				ImGui::InputFloat(buffer, &(pHRM->m_ivyAircraft->m_slats_deploy_value[index]), 0.01, 0.01, "%.2f", ImGuiInputTextFlags_ReadOnly );
				ImGui::SameLine();

				ImGui::PopItemWidth();
				
				sprintf(buffer, "SET S%2i", index + 1);
				if (ImGui::Button(buffer)) pHRM->m_ivyAircraft->m_slats_deploy_value[index] = pHRM->m_ivyAircraft->m_lf_slats;
				ImGui::SameLine();
				sprintf(buffer, "DEL S%2i", index + 1);
				if (ImGui::Button(buffer)) pHRM->m_ivyAircraft->m_slats_deploy_value[index] = -100;

				
			}
			



			ImGui::NextColumn();

			ImGui::SetColumnWidth(-1, 500);

			ImGui::PushItemWidth(100);
			ImGui::Checkbox("Enable Flaps", &(pHRM->m_ivyAircraft->m_flaps_enabled));
			//ImGui::SameLine();
			ImGui::InputFloat("Flaps Tolerance", &(pHRM->m_ivyAircraft->m_flaps_tolerance), 0.01, 0.01, 2, 0);
			ImGui::SameLine();
			ImGui::InputFloat("Flaps Value", &(pHRM->m_ivyAircraft->m_lf_flaps), 0.01, 0.01, 2, ImGuiInputTextFlags_ReadOnly);

			ImGui::PopItemWidth();
			ImGui::PushItemWidth(350);
			ImGui::InputText("FDataRef", &(pHRM->m_ivyAircraft->m_lf_flaps_data_ref));
			ImGui::PopItemWidth();

			//ImGui::Spacing();

			for (int index = 0; index < IVY_FS_MAX; index++)
			{
				char buffer[1024];

				ImGui::PushItemWidth(100);
				sprintf(buffer, "Pos F%2i", index + 1);
				ImGui::InputInt(buffer, &(pHRM->m_ivyAircraft->m_flaps_deploy_pos[index]), 1, 1, 0);

				ImGui::SameLine();

				sprintf(buffer, "F Val #%2i", index + 1);
				ImGui::InputFloat(buffer, &(pHRM->m_ivyAircraft->m_flaps_deploy_value[index]), 0.01, 0.01, "%.2f", ImGuiInputTextFlags_ReadOnly);
				ImGui::SameLine();

				ImGui::PopItemWidth();

				sprintf(buffer, "SET F%2i", index + 1);
				if (ImGui::Button(buffer)) pHRM->m_ivyAircraft->m_flaps_deploy_value[index] = pHRM->m_ivyAircraft->m_lf_flaps;
				ImGui::SameLine();
				sprintf(buffer, "DEL F%2i", index + 1);
				if (ImGui::Button(buffer)) pHRM->m_ivyAircraft->m_flaps_deploy_value[index] = -100;


			}

			

			
			
		}
	}
	*/
	
	//ImGui::TextUnformatted("Hello, World!");

	//ImGui::Text("Window size: width = %f  height = %f", win_width, win_height);

	//ImGui::TextUnformatted("Two Widgets");
}
