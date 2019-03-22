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

	void RemoveMission(void);
	void DrawMission(void);

	void SaveMission(boost::property_tree::ptree &pt, int &mission_counter);
	bool ReadMission(boost::property_tree::ptree &pt, int &mission_counter);

};

