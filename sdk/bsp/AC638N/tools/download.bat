@echo off
Setlocal enabledelayedexpansion
@echo ********************************************************************************
@echo           SDK AC638N            
@echo ********************************************************************************
@echo %date%

cd /d %~dp0

set OBJDUMP=C:\JL\pi32\bin\llvm-objdump.exe
set OBJCOPY=C:\JL\pi32\bin\llvm-objcopy.exe
set ELFFILE=../output/sdk.elf

set OBJSIZEDUMP=C:\JL\pi32\bin\llvm-objsizedump.exe

if not exist %OBJCOPY% goto download
if not exist %ELFFILE% goto download

%OBJCOPY% -O binary -j .text %ELFFILE% ../output/text.bin
%OBJCOPY% -O binary -j .data %ELFFILE% ../output/data.bin
%OBJCOPY% -O binary -j .data_code %ELFFILE% ../output/data_code.bin


%OBJDUMP% -section-headers -address-mask=0x1ffffff %ELFFILE%
%OBJSIZEDUMP% -skip-zero -lite  -enable-dbg-info %ELFFILE%  >symbol_tbl.txt

copy /b ..\output\text.bin+..\output\data.bin+..\output\data_code.bin  ..\output\app.bin
copy ..\output\app.bin app.bin

:download

set output_base_name=jl_isd

isd_download.exe isd_config.ini -tonorflash -dev br34 -boot 0x20000 -div8 -wait 300 -output %output_base_name% -uboot uboot.boot -app app.bin -res p11_code.bin -uboot_compress

@rem 常用命令说明
@rem -format vm        //擦除VM 区域
@rem -format 0x3f0-2   //表示从第 0x3f0 个 sector 开始连续擦除 2 个 sector(第一个参数为16进制或10进制都可，第二个参数必须是10进制)



@rem 添加配置脚本的版本信息到 FW 文件中
fw_add.exe -noenc -fw %output_base_name%.fw -add script.ver -out %output_base_name%.fw


ufw_maker.exe -fw_to_ufw %output_base_name%.fw
copy %output_base_name%.ufw update.ufw
del %output_base_name%.ufw




