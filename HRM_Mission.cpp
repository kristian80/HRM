#include "HRM_Mission.h"



HRM_Mission::HRM_Mission() :
	m_object_vector()
{
}


HRM_Mission::~HRM_Mission()
{
	//for (auto p_obj : m_object_vector) delete p_obj;
}

void HRM_Mission::SetPosition(double zero_latitude, double zero_longitude, double zero_heading)
{
	m_ld_latitude = zero_latitude;
	m_ld_longitude = zero_longitude;
	m_lf_heading = zero_heading;
}

void HRM_Mission::SetObjectPosition()
{
	for (auto p_obj : m_object_vector) 
		p_obj->SetPositionCart(m_ld_latitude, m_ld_longitude, m_lf_heading);
}

bool HRM_Mission::IsSlingLoad()
{
	if (m_mission_type == 3) return true;
	return false;
}

void HRM_Mission::RemoveMission(void)
{
	for (auto p_obj : m_object_vector)
	{
		p_obj->DestroyInstance();
	}
}

void HRM_Mission::RemovePatients(void)
{
	for (auto p_obj : m_object_vector)
	{
		if (p_obj->m_is_patient == true) p_obj->DestroyInstance();
	}
}

void HRM_Mission::DrawMission(void)
{
	for (auto p_obj : m_object_vector)
	{
		p_obj->LoadObject();
		p_obj->SetPositionCart(m_ld_latitude, m_ld_longitude, m_lf_heading);
	}
}

void HRM_Mission::SaveMission(boost::property_tree::ptree & pt, int & mission_counter)
{
	int object_counter = 0;
	std::string mission = "mission_" + std::to_string(mission_counter) + ".";

	//std::string node = mission + "." + "name";
	pt.put(mission + "name", m_name);

	pt.put(mission + "type", m_mission_type);
	pt.put(mission + "start_text", m_start_text);
	pt.put(mission + "pickup_text", m_pickup_text);
	pt.put(mission + "flight2_text", m_flight2_text);
	pt.put(mission + "failed_arr_text", m_failed_arr_text);
	pt.put(mission + "failed_hosp_text", m_failed_hosp_text);
	pt.put(mission + "end_text", m_end_text);
	pt.put(mission + "sound_start", m_sound_start);
	pt.put(mission + "sound_arr", m_sound_arr);
	pt.put(mission + "sound_pickup", m_sound_pickup);
	pt.put(mission + "sound_end", m_sound_end);
	pt.put(mission + "sound_failed_arr", m_sound_failed_arr);
	pt.put(mission + "sound_failed_hops", m_sound_failed_hops);

	for (auto p_obj : m_object_vector) p_obj->SaveObject(pt, mission, object_counter);
	

	mission_counter++;
}

bool HRM_Mission::ReadMission(boost::property_tree::ptree & pt, int & mission_counter)
{
	std::string mission = "mission_" + std::to_string(mission_counter) + ".";

	try { m_name = pt.get<std::string>(mission + "name"); }
	catch (...) { return false; }

	try { m_mission_type = pt.get<int>(mission + "type"); }
	catch (...) {  }
	try { m_start_text = pt.get<std::string>(mission + "start_text"); }
	catch (...) {}
	try { m_pickup_text = pt.get<std::string>(mission + "pickup_text"); }
	catch (...) {}
	try { m_flight2_text = pt.get<std::string>(mission + "flight2_text"); }
	catch (...) {}
	try { m_failed_arr_text = pt.get<std::string>(mission + "failed_arr_text"); }
	catch (...) {}
	try { m_failed_hosp_text = pt.get<std::string>(mission + "failed_hosp_text"); }
	catch (...) {}
	try { m_end_text = pt.get<std::string>(mission + "end_text"); }
	catch (...) {}
	try { m_sound_start = pt.get<int>(mission + "sound_start"); }
	catch (...) {}
	try { m_sound_arr = pt.get<int>(mission + "sound_arr"); }
	catch (...) {}
	try { m_sound_pickup = pt.get<int>(mission + "sound_pickup"); }
	catch (...) {}
	try { m_sound_end = pt.get<int>(mission + "sound_end"); }
	catch (...) {}
	try { m_sound_failed_arr = pt.get<int>(mission + "sound_failed_arr"); }
	catch (...) {}
	try { m_sound_failed_hops = pt.get<int>(mission + "sound_failed_hops"); }
	catch (...) {}

	bool object_found = true;
	HRM_Object *p_obj = NULL;
	int object_counter = 0;

	while (object_found)
	{
		p_obj = new HRM_Object();

		object_found = p_obj->ReadObject(pt, mission, object_counter);
		if (object_found)
			m_object_vector.push_back(p_obj);
		else
			delete p_obj;
	}

	// try { = pt.get<>(mission + ""); }
	// catch (...) {}
	mission_counter++;
	return true;
}
