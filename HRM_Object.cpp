#include "HRM_Object.h"



void HRM_Object::CreateInstance()
{
	HRMDebugString("Creating Instance");

	const char * drefs[] = { NULL, NULL };
	m_inst_ref = XPLMCreateInstance(m_obj_ref, drefs);
}

void HRM_Object::DestroyInstance()
{
	if (m_inst_ref != NULL)
	{
		XPLMDestroyInstance(m_inst_ref);
		m_inst_ref = NULL;
	}

	if (m_probe != NULL)
	{
		XPLMDestroyProbe(m_probe);
		m_probe = NULL;
	}

	if (m_obj_ref != NULL)
	{
		XPLMUnloadObject(m_obj_ref);
		m_obj_ref = NULL;
	}

}

void HRM_Object::SetPositionCart(double zero_latitude, double zero_longitude, double zero_heading)
{
	m_zero_angle = atan2(m_dist_x, m_dist_y) * 180 / M_PI;
	m_zero_distance = sqrt((m_dist_x * m_dist_x) + (m_dist_y * m_dist_y));
	SetPosition(zero_latitude, zero_longitude, zero_heading);

}

void HRM_Object::SetPosition(double zero_latitude, double zero_longitude, double zero_heading)
{
	if (!m_obj_ref)
	{
		HRMDebugString("No Object Loaded");
		return;
	}

	if (!m_inst_ref)
	{
		HRMDebugString("No Instance Created");
		return;
	}

	if (!m_probe)
	{
		HRMDebugString("No Probe Created");
		return;
	}

	double meter_latitude = 0;
	double meter_longitude = 0;

	HRM_Object::GetDegreesPerMeter(zero_latitude, zero_longitude, meter_latitude, meter_longitude);

	double total_heading = zero_heading + m_zero_angle;
	if (total_heading > 360) total_heading -= 360;
	

	m_latitude = zero_latitude + cos(total_heading * M_PI / 180) * meter_latitude * m_zero_distance;
	m_longitude = zero_longitude + sin(total_heading * M_PI / 180) * meter_longitude * m_zero_distance;

	double zero_x, zero_y, zero_z;
	XPLMWorldToLocal(m_latitude, m_longitude, 0, &zero_x, &zero_y, &zero_z);

	

	XPLMProbeInfo_t info;
	info.structSize = sizeof(info);

	XPLMProbeResult result = XPLMProbeTerrainXYZ(m_probe, zero_x, zero_y, zero_z, &info);
	result = XPLMProbeTerrainXYZ(m_probe, info.locationX, info.locationY, info.locationZ, &info);  // Twice for improved precision
	
	double local_long;
	double local_lat;
	double local_alt;

	XPLMLocalToWorld(info.locationX, info.locationY, info.locationZ, &local_lat, &local_long, &local_alt);
	
	XPLMWorldToLocal(local_lat, local_long, local_alt + m_elevation, &zero_x, &zero_y, &zero_z); // incorporate elevation



	if (result == xplm_ProbeHitTerrain)
	{
		
		double total_pitch = (-1.f * asin(info.normalX) * 180 / M_PI) + m_pitch;
		double total_roll = (-1.f * acos(info.normalY) * 180 / M_PI) + m_roll;

		if (total_pitch > 360) total_pitch -= 360.f;
		if (total_roll > 360) total_roll -= 360.f;

		total_heading = zero_heading + m_heading;
		if (total_heading > 360) total_heading -= 360;

		XPLMDrawInfo_t		dr;
		dr.structSize = sizeof(dr);
		dr.x = zero_x;
		dr.y = zero_y;
		dr.z = zero_z;
		dr.pitch = total_pitch;
		dr.heading = total_heading;
		dr.roll = total_roll;

		XPLMInstanceSetPosition(m_inst_ref, &dr, &m_angle);

	}
	else
	{
		HRMDebugString("Probe: No Terrain Hit");
	}
}

void HRM_Object::GetDegreesPerMeter(double zero_latitude, double zero_longitude, double & meter_latitude, double & meter_longitude)
{
	double meter_per_lat = calc_distance_m(zero_latitude, zero_longitude, zero_latitude + 1, zero_longitude);
	double meter_per_long = calc_distance_m(zero_latitude, zero_longitude, zero_latitude, zero_longitude + 1);

	meter_latitude = 1 / meter_per_lat;
	meter_longitude = 1 / meter_per_long;

}

void HRM_Object::SaveObject(boost::property_tree::ptree & pt, std::string mission, int & object_counter)
{
	std::string object = mission + "object_" + std::to_string(object_counter) + ".";

	pt.put(object + "obj_path", m_obj_path);
	pt.put(object + "elevation", m_elevation);
	pt.put(object + "dist_x", m_dist_x);
	pt.put(object + "dist_y", m_dist_y);
	pt.put(object + "heading", m_heading);
	pt.put(object + "pitch", m_pitch);
	pt.put(object + "roll", m_roll);
	pt.put(object + "is_patient", m_is_patient);

	object_counter++;
}

bool HRM_Object::ReadObject(boost::property_tree::ptree & pt, std::string mission, int & object_counter)
{
	std::string object = mission + "object_" + std::to_string(object_counter) + ".";

	try { m_obj_path = pt.get<std::string>(object + "obj_path"); }
	catch (...) { return false; }

	try { m_elevation = pt.get<float>(object + "elevation"); }
	catch (...) {}

	try { m_dist_x = pt.get<float>(object + "dist_x"); }
	catch (...) {}
	try { m_dist_y = pt.get<float>(object + "dist_y"); }
	catch (...) {}
	try { m_heading = pt.get<float>(object + "heading"); }
	catch (...) {}
	try { m_pitch = pt.get<float>(object + "pitch"); }
	catch (...) {}
	try { m_roll = pt.get<float>(object + "roll"); }
	catch (...) {}
	try { m_is_patient = pt.get<bool>(object + "is_patient"); }
	catch (...) {}

	object_counter++;

	return true;
}

bool HRM_Object::LoadObject()
{
	HRMDebugString("Object Lookup Start: " + m_obj_path);

	if (m_obj_ref || m_probe) DestroyInstance();

	XPLMLookupObjects(m_obj_path.c_str(), 0, 0, load_cb, &m_obj_ref);

	if (!m_obj_ref)
	{
		HRMDebugString("Nothing found");
		return false;
	}

	HRMDebugString("Object Lookup Finished");

	CreateInstance();

	m_probe = XPLMCreateProbe(xplm_ProbeY);

	return true;
}

HRM_Object::HRM_Object()
{
}


HRM_Object::~HRM_Object()
{
	HRMDebugString("Destroying Probe");
	if (!m_probe) XPLMDestroyProbe(m_probe);
}
