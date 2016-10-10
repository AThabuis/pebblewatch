clc; clear all; close all;

% Loads data file
load('Data_Matlab_TP/dataTP1.mat');

xdata_200Hz = dataTP.data(:,1);
ydata_200Hz = dataTP.data(:,2);
zdata_200Hz = dataTP.data(:,3);

Fs_init = 200; %Hz
Fs      =  10; %Hz

dt = 1/Fs_init;
t_step = 1/Fs; 


% resampling data
t_start = 0;
t_end   = dt*length(xdata_200Hz);

t = t_start:dt:t_end-dt; 

[xdata,tx] = resample(xdata_200Hz, t, Fs);
[ydata,ty] = resample(ydata_200Hz, t, Fs);
[zdata,tz] = resample(zdata_200Hz, t, Fs);

left_foot = dataTP.leftFootRef; 
right_foot = dataTP.rightFootRef; 

u_l = ones(size(left_foot)); 
u_r = ones(size(right_foot)); 

tt = 0:t_step:t_end;

% Compute moving average mean
sample_size = 2;

xdata_mean = tsmovavg(xdata,'s',sample_size,1);
ydata_mean = tsmovavg(ydata,'s',sample_size,1);
zdata_mean = tsmovavg(zdata,'s',sample_size,1);

xdata_unbiased = xdata_mean;
ydata_unbiased = ydata_mean;
zdata_unbiased = zdata_mean;

data_smooth = sqrt(xdata_unbiased.^2+ydata_unbiased.^2+zdata_unbiased.^2);
data = sqrt(xdata.^2+ydata.^2+zdata.^2);

figure;
plot(tx, data,'r'); hold on;
plot(tx, data_smooth,'k');
plot(t(left_foot),u_l, 'xr');
plot(t(right_foot),u_r, '+b');
hold off;

figure;
hold on;
%plot(tx,xdata,'k');
plot(ty,ydata_unbiased,'r');
%plot(tz,zdata,'b');
plot(t(left_foot),u_l, 'xr');
plot(t(right_foot),u_r, '+b');
hold off;
title(strcat(num2str(Fs),' Hz'));

% figure;
% plot(t, xdata_200Hz,'k'); hold on;
% plot(t, ydata_200Hz,'r');
% plot(t, zdata_200Hz,'b');
% 
% plot(t(left_foot),u_l, 'o');
% plot(t(right_foot),u_r, 'o'); 
% 
% hold off;
% title('200Hz');

%legend('X data','Y data','Z data');

% step frequency change this...
l_step_f = length(left_foot)/(t(left_foot(end))-t(left_foot(1)));
r_step_f = length(right_foot)/(t(right_foot(end))-t(right_foot(1)));

disp('left foot frequency [Hz]:'); disp(l_step_f);
disp('right foot frequency [Hz]:'); disp(r_step_f);

% L = length(data_smooth(2:end));
% NFFT = 2^nextpow2(L);
% Y = fft(data_smooth(2:end), NFFT)/L;
% f = Fs/2*linspace(0,1,NFFT/2+1);
% 
% figure;
% plot(f,2*abs(Y(1:NFFT/2+1)));

id_s = find(ty < 4 + 1e-2 & ty > 4 - 1e-2);
t_s = ty(id_s);
k = 0;
while(1)
    
    if ty(end-k) <= t_s + 10;
        break;
    end
    k = k+1;
end

t_e = ty(end-k);
id_e = find(ty == t_e);

L = length(data_smooth(id_s:id_e));
NFFT = 2^nextpow2(L);
Y = fft(data_smooth(id_s:id_e), NFFT)/L;
f = Fs/2*linspace(0,1,NFFT/2+1);

figure;
plot(f,2*abs(Y(1:NFFT/2+1)));