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

