@echo on


if not exist "%ZINSTALL_ROOT%"\CONFIG	mkdir "%ZINSTALL_ROOT%"\CONFIG


COPY "%PROJECTS_ROOT%"\config\*.cfg	 	"%ZINSTALL_ROOT%"\CONFIG\
COPY "%PROJECTS_ROOT%"\config\*.wav	 	"%ZINSTALL_ROOT%"\CONFIG\
COPY "%PROJECTS_ROOT%"\scripts\*.bat	 	"%ZINSTALL_ROOT%"\



