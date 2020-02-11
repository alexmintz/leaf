@set PATH=.\x64\bin;%PATH%
@set TCE_HOST=192.168.10.64
@set TCE_PORT=9301

START "MD_MONITOR" .\x64\bin\md_monitor.exe -z_config_dir .\CONFIG
