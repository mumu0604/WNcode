--[[
	$Id: display.lua 772 2012-08-03 13:59:49Z lwy $
	$URL: http://172.17.8.1/svn/QUESS/tags/QKDBench-2.2.1/display.lua $
]]--
--[[
		<Displays>
		<Display index="0" name="����ʾ"/>
		<Display index="1" name="��ʾ���ؽ������"/>
		<Display index="2" name="��ʾʮ������"/>
		<Display index="3" name="��ʾƽ̨ʱ��"/>
		<Display index="4" name="��ʾGPSʱ��"/>
		<Display index="5" name="��ʾ����+28V����"/>
		<Display index="6" name="��ʾ����APD320V��ѹ"/>
		<Display index="7" name="��ʾLD���ʼ��ֵ"/>
		<Display index="8" name="��ʾGP1оƬ�����¶�"/>
		<Display index="9" name="��ʾLD������ѹ+5V"/>
		<Display index="10" name="��ʾAPD��ѹ+5V"/>
		<Display index="11" name="��ʾAPD LD+5V��ѹ"/>
		<Display index="12" name="��ʾLD�廷���¶�"/>
		<Display index="13" name="��ʾ���ʼ����¶�"/>
		<Display index="14" name="��ʾLD����������¶�"/>
		<Display index="15" name="��ʾAPDѩ���������¶�"/>
		<Display index="16" name="��ʾ���ʼ��ֵ"/>
		<Display index="17" name="��ʾFPGA�¶�"/>
		<Display index="18" name="��ʾ�������"/>
		<Display index="19" name="��ʾ����+28V��ѹ(AD)">ֻ��plot.xml�������ã�Ϊ��ʹ�˿ؽ���ͻطŽ�����������ʾ������ʾ��ʽ�̶�Ϊ19�����ܸ��ġ�</Display>
		<Display index="20" name="��ʾ����������ѹ(AD)">ֻ��plot.xml�������ã�Ϊ��ʹ�˿ؽ���ͻطŽ�����������ʾ������ʾ��ʽ�̶�Ϊ20�����ܸ��ġ�</Display>
		<Display index="21" name="��ʾͬ����+5V��ѹ(AD)">ֻ��plot.xml�������ã�Ϊ��ʹ�˿ؽ���ͻطŽ�����������ʾ������ʾ��ʽ�̶�Ϊ21�����ܸ��ġ�</Display>
		<Display index="31" name="��ʾQKD��ѧ���ݴ���״̬"/>
		<Display index="32" name="��ʾ��������"/>
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
		s = string.format("%03d��%02dʱ%02d��%02d��%03d����",day,hour,minute,second,milisecond)
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
		s1 = os.date("%Y��%m��%d��%Hʱ%M��%S��",tm)
		
		local s
		s = string.format("%04d��%06d�� %s",week,second, s1)
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
	s = string.format("%.1f��",y)
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
	s = string.format("%.1f��",y)
	return y,s
end

function Calc13(x)
	local y
	y = string.byte(x,1)
	if y > 127 then
		return 0,"�������ݴ���(����127)"
	else
		y = math.log(y*4.0/(255 - y*2))
		y = 1221800.0/(298*y + 4100) - 273
		local s
		s = string.format("%.1f��",y)
		return y,s
	end
end

function Calc14(x)
	local y
	y = string.byte(x,1)
	y = math.log(y/51.0)
	y = 1177100.0/(298*y + 3950) - 273
	local s
	s = string.format("%.1f��",y)
	return y,s
end

function Calc15(x)
	local y
	y = string.byte(x,1)
	y = math.log(y*164.0/1275);
	y = 1067138.0/(298*y + 3581) - 273.15
	local s
	s = string.format("%.1f��",y)
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
	s = string.format("%.1f��",y)
	return y,s
end

function Calc18(x)
	local y
	for i=1, string.len(x) do
		y = string.byte(x,i)
		if y ~= 0 then
			return 0,"������ݴ���"
		end
	end
	return 0,"���������ȷ"
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
		s = "���ݴ�������"
	elseif y == 1 then
		s = "���ݴ���ʧ��"
	elseif y == 2 then
		s = "�ڴ濽��Դ��ַ��Ч"
	elseif y == 3 then
		s = "�ڴ濽��Ŀ�ĵ�ַ��Ч"
	elseif y == 4 then
		s = "���ݴ����ӹ�����Ч"
	elseif y == 5 then
		s = "������Խ��"
	elseif y == 6 then
		s = "ԭʼ��Կ��Խ��"
	elseif y == 7 then
		s = "�ֶγ���Խ��"
	elseif y == 8 then
		s = "����ʧ�ܶ���Խ��"
	elseif y == 9 then
		s = "��ȫϵ��Խ��"
	elseif y == 10 then
		s = "��˽�Ŵ󳤶�Խ��"
	elseif y == 11 then
		s = "���ܳ���Խ��"
	elseif y == 12 then
		s = "Flash���"
	elseif y == 65535 then
		s = "�������ʧ��"
	else
		s = "���ݴ���δ֪����"
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
		s = "�����"
	elseif y == 2 then
		s = "ʱ���������"
	elseif y == 3 then
		s = "����λ������"
	elseif y == 4 then
		s = "��������"
	elseif y == 5 then
		s = "ԭʼ��Կλ������"
	elseif y == 6 then
		s = "ԭʼ��Կ"
	elseif y == 7 then
		s = "У�������"
	elseif y == 8 then
		s = "У��ͱȽ�����"
	elseif y == 9 then
		s = "��������ʽ"
	elseif y == 10 then
		s = "����ʧ��λ������"
	elseif y == 11 then
		s = "����������"
	elseif y == 12 then
		s = "��ȫ��Կ����"
	elseif y == 13 then
		s = "���Ӹ�������"
	elseif y == 14 then
		s = "��������"
	elseif y == 15 then
		s = "��������"
	elseif y == 16 then
		s = "��������"
	else
		s = "δ֪����"
	end
	return 0,s
end
