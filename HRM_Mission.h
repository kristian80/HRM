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
#include "HRM_Object.h"


class HRM_Mission
{

public:
	std::string m_name = "NEW MISSION";
	int m_mission_type = 0;

	std::string m_start_text = "Nothing";
	std::string m_pickup_text = "The patient is now on board. You should try to get to the hospital as fast, but also as smoothly as possible";
	std::string m_flight2_text = "The patient is on board and we are on our way to the hospital. We should go as quickly, but also as smoothly as possible to ensure our patient is alright";
	std::string m_failed_arr_text = "We came too late";
	std::string m_failed_hosp_text = "The patient did not make it";
	std::string m_end_text = "Mission completed";

	int m_sound_start = 0;
	int m_sound_arr = 0;
	int m_sound_pickup = 0;
	int m_sound_end = 0;
	int m_sound_failed_arr = 0;
	int m_sound_failed_hops = 0;

	double m_ld_latitude = 0;
	double m_ld_longitude = 0;
	float m_lf_heading = 0;

	std::vector<HRM_Object *> m_object_vector;

	HRM_Mission();
	~HRM_Mission();

	void SetPosition(double zero_latitude, double zero_longitude, double zero_heading);
	void SetObjectPosition();

	bool IsSlingLoad();

	void RemoveMission(void);
	void RemovePatients(void);
	void DrawMission(void);

	void SaveMission(boost::property_tree::ptree &pt, int &mission_counter);
	bool ReadMission(boost::property_tree::ptree &pt, int &mission_counter);

};

