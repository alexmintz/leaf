@set PATH=.\x64\bin;%PATH%
@set TCE_HOST=192.168.10.73
@set TCE_PORT=9300

START "MD_MONITOR" .\x64\bin\md_monitor.exe -z_config_dir .\CONFIG
