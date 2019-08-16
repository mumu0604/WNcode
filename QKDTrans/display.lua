--[[
	$Id: display.lua 772 2012-08-03 13:59:49Z lwy $
	$URL: http://172.17.8.1/svn/QUESS/tags/QKDBench-2.2.1/display.lua $
]]--
--[[
		<Displays>
		<Display index="0" name="不显示"/>
		<Display index="1" name="显示比特解析结果"/>
		<Display index="2" name="显示十进制数"/>
		<Display index="3" name="显示平台时间"/>
		<Display index="4" name="显示GPS时间"/>
		<Display index="5" name="显示量子+28V电流"/>
		<Display index="6" name="显示量子APD320V高压"/>
		<Display index="7" name="显示LD功率检测值"/>
		<Display index="8" name="显示GP1芯片环境温度"/>
		<Display index="9" name="显示LD主备电压+5V"/>
		<Display index="10" name="显示APD电压+5V"/>
		<Display index="11" name="显示APD LD+5V电压"/>
		<Display index="12" name="显示LD板环境温度"/>
		<Display index="13" name="显示功率监测板温度"/>
		<Display index="14" name="显示LD激光二极管温度"/>
		<Display index="15" name="显示APD雪崩二极管温度"/>
		<Display index="16" name="显示功率检测值"/>
		<Display index="17" name="显示FPGA温度"/>
		<Display index="18" name="显示填充数据"/>
		<Display index="19" name="显示量子+28V电压(AD)">只有plot.xml可以配置，为了使运控界面和回放界面能正常显示，其显示方式固定为19，不能更改。</Display>
		<Display index="20" name="显示量子主备电压(AD)">只有plot.xml可以配置，为了使运控界面和回放界面能正常显示，其显示方式固定为20，不能更改。</Display>
		<Display index="21" name="显示同步光+5V电压(AD)">只有plot.xml可以配置，为了使运控界面和回放界面能正常显示，其显示方式固定为21，不能更改。</Display>
		<Display index="31" name="显示QKD科学数据处理状态"/>
		<Display index="32" name="显示数据类型"/>
	</Displays>
]]--

function Calc00(x)
		return 0, ""
end

function Calc01(x)
		return 0, ""
end

function Calc02(x)
		local y
		y = 0
		for i=1, string.len(x) do
			y = y * 256 + string.byte(x,i)
		end
		local s
		s = string.format("%d",y);
		return y, s
end

function Calc03(x)
		local sec, milisecond
		sec = 0
		for i=1,4 do
			sec = sec * 256 + string.byte(x,i)
		end
		milisecond = 0
		for i=5,6 do
			milisecond = milisecond * 256 + string.byte(x,i)
		end 
		milisecond = milisecond * 0.5
		local day,hour,minute,second

		day = math.floor(sec / 86400.0)
		sec = sec - day * 86400
		hour = math.floor(sec / 3600.0)
		sec = sec - hour * 3600.0
		minute = math.floor(sec / 60.0)
		second = sec - minute * 60.0
		local s
		s = string.format("%03d日%02d时%02d分%02d秒%03d毫秒",day,hour,minute,second,milisecond)
		return 0,s
end

function Calc04(x)
		local week, second
		week = 0
		for i=1,2 do
			week = week * 256 + string.byte(x,i)
		end
		second = 0
		for i=3,6 do
			second = second * 256 + string.byte(x,i)
		end 
		
		week = math.fmod(week,1024)
		second = math.fmod(second,604800)	
		
		t = {year=1999, month=8, day=22, hour=8}

		local tm
		tm = os.time(t)
		tm = tm + week*604800 + second
		
		local s1
		s1 = os.date("%Y年%m月%d日%H时%M分%S秒",tm)
		
		local s
		s = string.format("%04d周%06d秒 %s",week,second, s1)
		return 0,s
end

function Calc05(x)
	local y
	y = string.byte(x,1)
	y = y/255.0*6.25
	local s
	s = string.format("%.2fA",y)
	return y,s
end

function Calc06(x)
	local y
	y = string.byte(x,1)
	y = y/255.0*5.0*308.0
	local s
	s = string.format("%.1fV",y)
	return y,s
end

function Calc07(x)
	local y
	y = 0
	local s
	s = string.format("%.3fuW",y)
	return y,s
end

function Calc08(x)
	local y
	y = string.byte(x,1)
	y = math.log(y*2.0/(255 - y))
	y = 1221800.0/(298*y + 4100) - 273
	local s
	s = string.format("%.1f℃",y)
	return y,s
end

function Calc09(x)
	local y
	y = string.byte(x,1)
	y = y*5.0/255
	local s
	s = string.format("%.1fV",y)
	return y,s
end

function Calc10(x)
	local y
	y = string.byte(x,1)
	y = y*5.0/255*46/36
	local s
	s = string.format("%.1fV",y)
	return y,s
end

function Calc11(x)
	local y
	y = string.byte(x,1)
	y = y*5.0/255
	local s
	s = string.format("%.1fV",y)
	return y,s
end

function Calc12(x)
	local y
	y = string.byte(x,1)
	y = math.log(y*2.0/(255 - y))
	y = 1221800.0/(298*y + 4100) - 273
	local s
	s = string.format("%.1f℃",y)
	return y,s
end

function Calc13(x)
	local y
	y = string.byte(x,1)
	if y > 127 then
		return 0,"采样数据错误(大于127)"
	else
		y = math.log(y*4.0/(255 - y*2))
		y = 1221800.0/(298*y + 4100) - 273
		local s
		s = string.format("%.1f℃",y)
		return y,s
	end
end

function Calc14(x)
	local y
	y = string.byte(x,1)
	y = math.log(y/51.0)
	y = 1177100.0/(298*y + 3950) - 273
	local s
	s = string.format("%.1f℃",y)
	return y,s
end

function Calc15(x)
	local y
	y = string.byte(x,1)
	y = math.log(y*164.0/1275);
	y = 1067138.0/(298*y + 3581) - 273.15
	local s
	s = string.format("%.1f℃",y)
	return y,s
end

function Calc16(x)
	local y
	y = 0
	local s
	s = string.format("%.3fuW",y)
	return y,s
end

function Calc17(x)
	local y
	y = string.byte(x,1)
	y = math.log(y*2.0/(255 - y))
	y = 1221800.0/(298*y + 4100) - 273
	local s
	s = string.format("%.1f℃",y)
	return y,s
end

function Calc18(x)
	local y
	for i=1, string.len(x) do
		y = string.byte(x,i)
		if y ~= 0 then
			return 0,"填充数据错误"
		end
	end
	return 0,"填充数据正确"
end

function Calc19(x)
	local y
	y = string.byte(x,2)*256 + string.byte(x,1)
	y = bit32.rshift(y,4)
	y = 10.0*y*7/2047
	local s
	s = string.format("%.1fV",y)
	return y,s
end

function Calc20(x)
	local y
	y = string.byte(x,2)*256 + string.byte(x,1)
	y = bit32.rshift(y,4)
	y = 10.0*y/2047
	local s
	s = string.format("%.1fV",y)
	return y,s
end

function Calc21(x)
	local y
	y = string.byte(x,2)*256 + string.byte(x,1)
	y = bit32.rshift(y,4)
	y = 10.0*y*1.8/2047
	local s
	s = string.format("%.1fV",y)
	return y,s
end

function Calc31(x)
	local y
	y = 0
	for i=1,2 do
		y = y * 256 + string.byte(x,i)
	end
	local s
	if y == 0 then 
		s = "数据处理正常"
	elseif y == 1 then
		s = "数据处理失败"
	elseif y == 2 then
		s = "内存拷贝源地址无效"
	elseif y == 3 then
		s = "内存拷贝目的地址无效"
	elseif y == 4 then
		s = "数据处理子功能无效"
	elseif y == 5 then
		s = "脉冲数越界"
	elseif y == 6 then
		s = "原始密钥数越界"
	elseif y == 7 then
		s = "分段长度越界"
	elseif y == 8 then
		s = "纠错失败段数越界"
	elseif y == 9 then
		s = "安全系数越界"
	elseif y == 10 then
		s = "隐私放大长度越界"
	elseif y == 11 then
		s = "加密长度越界"
	elseif y == 12 then
		s = "Flash溢出"
	elseif y == 65535 then
		s = "代码加载失败"
	else
		s = "数据处理未知错误"
	end
	return 0,s
end

function Calc32(x)
	local y
	y = string.byte(x,1)
	local s
	if y == 0 then
		s = ""
	elseif y == 1 then
		s = "随机数"
	elseif y == 2 then
		s = "时间测量数据"
	elseif y == 3 then
		s = "脉冲位置数据"
	elseif y == 4 then
		s = "脉冲数据"
	elseif y == 5 then
		s = "原始密钥位置数据"
	elseif y == 6 then
		s = "原始密钥"
	elseif y == 7 then
		s = "校验和数据"
	elseif y == 8 then
		s = "校验和比较数据"
	elseif y == 9 then
		s = "汉明伴随式"
	elseif y == 10 then
		s = "纠错失败位置数据"
	elseif y == 11 then
		s = "纠错结果数据"
	elseif y == 12 then
		s = "安全密钥数据"
	elseif y == 13 then
		s = "量子辅助数据"
	elseif y == 14 then
		s = "密文数据"
	elseif y == 15 then
		s = "代码数据"
	elseif y == 16 then
		s = "保留数据"
	else
		s = "未知数据"
	end
	return 0,s
end
