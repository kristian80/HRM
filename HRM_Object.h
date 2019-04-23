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

class HRM_Object
{
public:
	double m_latitude = -10000;
	double m_longitude = -10000;
	
	double m_elevation = 0;

	double m_x = 0;
	double m_y = 0;
	double m_z = 0;

	double m_dist_x = 0;
	double m_dist_y = 0;

	double m_heading = 0;
	double m_pitch = 0;
	double m_roll = 0;

	double m_zero_angle = 0;
	double m_zero_distance = 0;

	float m_angle = 0;

	bool m_is_patient = false;

	std::string m_obj_path = "3D_people_library/commercial_pilot.obj";

	XPLMObjectRef m_obj_ref = NULL;
	XPLMInstanceRef m_inst_ref = NULL;
	XPLMProbeRef m_probe = NULL;

	void CreateInstance();
	void DestroyInstance();
	void SetPositionCart(double zero_latitude, double zero_longitude, double zero_heading);
	void SetPosition(double zero_latitude, double zero_longitude, double zero_heading);

	static void GetDegreesPerMeter(double zero_latitude, double zero_longitude, double &meter_latitude, double &meter_longitude);

	void SaveObject(boost::property_tree::ptree &pt, std::string mission, int &object_counter);
	bool ReadObject(boost::property_tree::ptree &pt, std::string mission, int &object_counter);

	bool LoadObject();

	HRM_Object();
	~HRM_Object();
};

