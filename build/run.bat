@echo off
::
ninja
pyocd flash -t STM32F103C8 TEST_LED.elf
pause