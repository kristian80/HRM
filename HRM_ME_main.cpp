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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "XPLMUtilities.h"
#include "XPLMProcessing.h"
#include "XPLMPlugin.h"
#include "HRM_PlugIn.h"
#include "HRM.h"

// OS X: we use this to convert our file path.
#if APL
//#include <Carbon/Carbon.h>
#endif






/**************************************************************************************************************
 * Global Variables 
 **************************************************************************************************************/
HRM_PlugIn * pHRM;
std::ofstream hrm_output_file;


/*
// Mac specific: this converts file paths from HFS (which we get from the SDK) to Unix (which the OS wants).
// See this for more info:
//
// http://www.xsquawkbox.net/xpsdk/mediawiki/FilePathsAndMacho

#if APL
int ConvertPath(const char * inPath, char * outPath, int outPathMaxLen) {

	CFStringRef inStr = CFStringCreateWithCString(kCFAllocatorDefault, inPath, kCFStringEncodingMacRoman);
	if (inStr == NULL)
		return -1;
	CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, inStr, kCFURLHFSPathStyle, 0);
	CFStringRef outStr = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
	if (!CFStringGetCString(outStr, outPath, outPathMaxLen, kCFURLPOSIXPathStyle))
		return -1;
	CFRelease(outStr);
	CFRelease(url);
	CFRelease(inStr);
	return 0;
}
#endif*/

// Initialization code.

static float InitPlugin(float elapsed, float elapsed_sim, int counter, void * ref)
{
	HRMDebugString("HRM: Initializing.\n");
	pHRM->PluginStart();
	return 0.0f;
}

PLUGIN_API int XPluginStart(char * name, char * sig, char * desc)
{
	HRMDebugString("HRM: Startup.\n");
	strcpy(name, "HRM");
	strcpy(sig, "k80.HRM");
	strcpy(desc, "The nagging co-pilot");

	hrm_output_file.open("HRMLog.txt");

	pHRM = new HRM_PlugIn();

	if (sizeof(unsigned int) != 4 ||
		sizeof(unsigned short) != 2)
	{
		HRMDebugString("HRM: This plugin was compiled with a compiler with weird type sizes.\n");
		return 0;
	}

	// Use Native Paths
	XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);

	// Do deferred sound initialization. See http://www.xsquawkbox.net/xpsdk/mediawiki/DeferredInitialization
	// for more info.
	XPLMRegisterFlightLoopCallback(InitPlugin, -1.0, NULL);

	

	return 1;
}

PLUGIN_API void XPluginStop(void)
{
	pHRM->PluginStop();
	delete pHRM;
	hrm_output_file.close();
}

PLUGIN_API int XPluginEnable(void)
{
	pHRM->PluginEnable();
	return 1;
}

PLUGIN_API void XPluginDisable(void)
{
	pHRM->PluginDisable();
}



PLUGIN_API void XPluginReceiveMessage(XPLMPluginID from, int msg, void * p)
{
	pHRM->PluginReceiveMessage(from, msg, p);
}

void WrapMenuHandler(void * in_menu_ref, void * in_item_ref)
{
	return pHRM->PluginMenuHandler(in_menu_ref, in_item_ref);
}

int WrapToggleControlWindowCallback(XPLMCommandRef cmd, XPLMCommandPhase phase, void* refcon)
{
	return pHRM->ToggleControlWindowCallback(cmd, phase, refcon);
}

/*
int WrapHRMVSpeedHandler(XPWidgetMessage  inMessage, XPWidgetID  inWidget, intptr_t inParam1, intptr_t inParam2)
{
	return pHRM->HRMVSpeedHandler(inMessage,inWidget,inParam1,inParam2);
}
int WrapHRMLogbookHandler(XPWidgetMessage  inMessage, XPWidgetID  inWidget, intptr_t inParam1, intptr_t inParam2)
{
	return pHRM->HRMLogbookHandler(inMessage,inWidget,inParam1,inParam2);
}
int WrapHRMLogbookScrollHandler(XPWidgetMessage  inMessage, XPWidgetID  inWidget, intptr_t inParam1, intptr_t inParam2)
{
	return pHRM->HRMLogbookScrollHandler(inMessage,inWidget,inParam1,inParam2);
}

void WrapHRMDrawOutputWindow(XPLMWindowID in_window_id, void * in_refcon)
{
	return pHRM->HRMDrawOutputWindow(in_window_id,in_refcon);
}

int WrapSayBaroCallback(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon)
{
	return pHRM->SayBaroCallback(cmd,phase,refcon);
}
int WrapSayWindCallback(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon)
{
	return pHRM->SayWindCallback(cmd,phase,refcon);
}
int WrapAnnouncementCallback(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon)
{
	return pHRM->AnnouncementCallback(cmd,phase,refcon);
}
int WrapResetHRMCallback(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon)
{
	return pHRM->ResetHRMCallback(cmd,phase,refcon);
}
int WrapToogleWindowCallback(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon)
{
	return pHRM->ToogleWindowCallback(cmd,phase,refcon);
}

void WrapKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey, void * inRefcon, int losingFocus)
{
	return pHRM->KeyCallback(inWindowID,inKey,inFlags,inVirtualKey,inRefcon,losingFocus);
}
int WrapMouseClickCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void * inRefcon)
{
	return pHRM->MouseClickCallback(inWindowID,x,y,inMouse,inRefcon);
}*/

PLUGIN_API float WrapFlightLoopCallback(float elapsedMe, float elapsedSim, int counter, void * refcon)
{
	return pHRM->PluginFlightLoopCallback(elapsedMe,elapsedSim,counter,refcon);
}

double calc_distance_m(double lat1, double long1, double lat2, double long2)
{
	lat1 = lat1 * M_PI / 180;
	long1 = long1 * M_PI / 180;
	lat2 = lat2 * M_PI / 180;
	long2 = long2 * M_PI / 180;

	double rEarth = 6372797;

	double dlat = lat2 - lat1;
	double dlong = long2 - long1;

	double x1 = sin(dlat / 2);
	double x2 = cos(lat1);
	double x3 = cos(lat2);
	double x4 = sin(dlong / 2);

	double x5 = x1 * x1;
	double x6 = x2 * x3 * x4 * x4;

	double temp1 = x5 + x6;

	double y1 = sqrt(temp1);
	double y2 = sqrt(1.0 - temp1);

	double temp2 = 2 * atan2(y1, y2);

	double range_m = temp2 * rEarth;

	return range_m;
}

double calc_distance_nm(double lat1, double long1, double lat2, double long2)
{
	lat1 = lat1 * M_PI / 180;
	long1 = long1 * M_PI / 180;
	lat2 = lat2 * M_PI / 180;
	long2 = long2 * M_PI / 180;

	double rEarth = 6372.797;

	double dlat = lat2 - lat1;
	double dlong = long2 - long1;

	double x1 = sin(dlat / 2);
	double x2 = cos(lat1);
	double x3 = cos(lat2);
	double x4 = sin(dlong / 2);

	double x5 = x1 * x1;
	double x6 = x2 * x3 * x4 * x4;

	double temp1 = x5 + x6;

	double y1 = sqrt(temp1);
	double y2 = sqrt(1.0 - temp1);

	double temp2 = 2 * atan2(y1, y2);

	double rangeKm = temp2 * rEarth;

	double CalcRange = rangeKm * 0.539957;

	return CalcRange;
}

double WrapReaddoubleCallback(void* inRefcon)
{
	
		return *((double*)inRefcon);
}

void WrapWritedoubleCallback(void* inRefcon, double inValue)
{
	
		* ((double*)inRefcon) = inValue;
}
double WrapReadDoubleCallback(void* inRefcon)
{
	
		return *((double*)inRefcon);
}

void WrapWriteDoubleCallback(void* inRefcon, double inValue)
{
	
		* ((double*)inRefcon) = inValue;
}

int WrapReaddoubleArrayCallback(
	void* inRefcon,
	float* outValues,    /* Can be NULL */
	int                  inOffset,
	int                  inMax)
{
	
		double* array = (double*)inRefcon;
	for (int i = 0; i < inMax; i++) outValues[i] = (float)array[i + inOffset];
	return 10;
}

void WrapWritedoubleArrayCallback(
	void* inRefcon,
	float* inValues,
	int                  inOffset,
	int                  inCount)
{
	
		double* array = (double*)inRefcon;
	for (int i = 0; i < inCount; i++) array[i + inOffset] = inValues[i];

	memcpy(array + inOffset, inValues, sizeof(double) * inCount);
}

int WrapReadIntCallback(void* inRefcon)
{
	
		return (int)(*((bool*)inRefcon));
}

void WrapWriteIntCallback(void* inRefcon, int inValue)
{
	
		* ((int*)inRefcon) = inValue;
}

int WrapReadStringCallback(
	void* inRefcon,
	void* outValue,    /* Can be NULL */
	int                  inOffset,
	int                  inMaxLength)
{
	
		std::string* pStr = (std::string*) inRefcon;

	if (pStr->size() < inMaxLength)
	{
		strcpy((char*)outValue, pStr->c_str());
		return 2048;
	}
	return (int)pStr->length();

}

void WrapWriteStringCallback(
	void* inRefcon,
	void* inValue,
	int                  inOffset,
	int                  inLength)
{
	
		std::string* pStr = (std::string*) inRefcon;
	*pStr = (char*)inValue;
}