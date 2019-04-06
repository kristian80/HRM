CXXFLAGS = -std=c++17 -m64 -w -pipe -Wall -Wextra -Wshadow -Wfloat-equal -pedantic -fvisibility=hidden -O2 -fmessage-length=0 -D_FORTIFY_SOURCE=2 -fstack-protector -funwind-tables -fasynchronous-unwind-tables -W -fPIC -DXPLM200 -DAPL=0 -DIBM=0 -DLIN=1 \
-I../SDK/CHeaders/Widgets \
-I../SDK/CHeaders/Wrappers \
-I../SDK/CHeaders/XPLM \
-I../HRM \
-I../boost_1_69_0 \
-I../imgui \
-I../imgui4xp \
-I./xsb_public_changes \
-I/usr/include/glbinding/gl \
-I/usr/include/GL \
-I. \
-DXPLM200=1 \
-DXPLM210=1 \
-DXPLM300=1 \
-DXPLM301=1 \
#-DIBM=0 \
#-DLIN=1 \
-DAPL=1 \

LINUXFLAGS = -DLIN=1
MACFLAGS = -DAPL=1

LDLIBS = -L../boost_1_69_0/libs 
LINUXLINKER = -shared -rdynamic -nodefaultlibs -undefined_warning
MACLINKER = -m64 -shared -F../SDK/Libraries/Mac -framework XPLM -framework OpenGL

OBJ = HRM_ME_main.o HRM_Object.o HRM_Mission.o HRM_PlugIn.o HRMImguiWidget.o ./xsb_public_changes/ImgWindow.o ../imgui/imgui_draw.o ../imgui/imgui_widgets.o ../imgui/imgui.o ../imgui/misc/cpp/imgui_stdlib.o 

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS) 

lin.xpl: $(OBJ)
	$(CXX) -o $@ $^ $(LDLIBS) $(LINUXLINKER)

mac.xpl: $(OBJ)
	$(CXX) -o $@ $^ $(LDLIBS) $(MACLINKER)

