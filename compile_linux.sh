#!/bin/bash
g++ -c -m64 -w -pipe -Wall -Wextra -Wshadow -Wfloat-equal -pedantic -fvisibility=hidden -O2 -fmessage-length=0 -D_FORTIFY_SOURCE=2 -fstack-protector -funwind-tables -fasynchronous-unwind-tables -W -fPIC -DXPLM200 -DAPL=0 -DIBM=0 -DLIN=1 \
-I../SDK/CHeaders/Widgets \
-I../SDK/CHeaders/Wrappers \
-I../SDK/CHeaders/XPLM \
-I../HRM \
-I../boost_1_69_0 \
-I../imgui \
-I../imgui4xp \
-I../xsb_public \
-I/usr/include/glbinding/gl \
-I/usr/include/GL \
-DXPLM200=1 \
-DXPLM210=1 \
-DXPLM300=1 \
-DXPLM301=1 \
-DIBM=0 \
-DLIN=1 \
-DAPL=0 \
-o HRM_ME_main.o HRM_ME_main.cpp
