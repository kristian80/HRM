#pragma once
#include "HRM.h"
class HRMImguiWidget :
	public ImgWindow
{
public:
	HRMImguiWidget(HRM_PlugIn *pHRMNew, int left, int top, int right, int bot, int decoration);
	~HRMImguiWidget();

	void Visible(bool visible);

public:
	void configureImguiContext() override;
	void buildInterface() override;
private:
	ImFont* font2;
	ImFont* font3;
	ImFont* font4;
	ImFont* font5;
	ImFont* font6;
	ImFont* font7;
	ImFont* font8;
	ImFont* font9;
	ImFont* font10;
	ImFont* font11;
	ImFont* font12;
	ImFont* font13;
	ImFont* font14;
	ImFont* font15;

	float win_width = 0;
	float win_height = 0;

	float output_dh = 0;

	int selected_tab = 1;
	int selected_graph = 1;

	int selected_radio = 0;

	HRM_PlugIn *pHRM = NULL;
};

