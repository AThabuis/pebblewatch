clc; clear all; close all;

% Loads data file
load('Data_Matlab_TP/dataTP1.mat');

% Get x,y,z datas at 200 Hz
xdata_200Hz = dataTP.data(:,1);
ydata_200Hz = dataTP.data(:,2);
zdata_200Hz = dataTP.data(:,3);

% Get left and right foot reference timestamps
left_foot = dataTP.leftFootRef; 
right_foot = dataTP.rightFootRef; 

Fs_init = 200; % Initial data sampling frequency in Herz
Fs      =  25; % Resampling frequency in Herz

dt_init = 1/Fs_init; % time step for initial frequency
dt      = 1/Fs;      % time step for resampled data


t_start = 0;
t_end   = dt_init*length(xdata_200Hz);

% time vector for initial data
t = t_start:dt_init:t_end-dt_init;

% time vector for resampled data
tt = t_start:dt:t_end-dt;

% resampling data
[xdata,tx] = resample(xdata_200Hz, t, Fs);
[ydata,ty] = resample(ydata_200Hz, t, Fs);
[zdata,tz] = resample(zdata_200Hz, t, Fs);

u_l = ones(size(left_foot)); 
u_r = ones(size(right_foot)); 

% Compute moving average mean
avr_pnts = 2;

xdata_mean = tsmovavg(xdata,'s',avr_pnts,1);
ydata_mean = tsmovavg(ydata,'s',avr_pnts,1);
zdata_mean = tsmovavg(zdata,'s',avr_pnts,1);

% Remove mean to data (NOT DONE HERE - SEE IF STILL NECESSARY)
xdata_unbiased = xdata_mean;
ydata_unbiased = ydata_mean;
zdata_unbiased = zdata_mean;

% Compute norm of acceleration (over x,y,z)
data_norm_sq      = xdata.^2+ydata.^2+zdata.^2;
data_norm_sq_mean = xdata_unbiased.^2+ydata_unbiased.^2+zdata_unbiased.^2;

% Plot data norms
figure;
plot(tx, data_norm_sq,'r'); hold on;
plot(tx, data_norm_sq_mean,'k');
plot(t(left_foot),u_l, 'xr');
plot(t(right_foot),u_r, '+b');
hold off;
legend('norm squared','norm squared of averaged data');

% Steps frequencies - change this... (if guys stops then freq changes)
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

t_start_fft = 34; % start time for fft in sec
t_tot_fft   = 5;  % total time for fft in sec

id_s = find(ty < t_start_fft + 1e-2 & ty > t_start_fft - 1e-2);
t_s = ty(id_s);
k = 0;
while(1)
    
    if ty(end-k) <= t_s + t_tot_fft;
        break;
    end
    k = k+1;
end

t_e = ty(end-k);
id_e = find(ty == t_e);

% Computes fft for t_tot_fft time of samples
%L = length(data_norm_sq_mean(id_s:id_e));
%NFFT = 2^nextpow2(L);
%Y = fft(data_norm_sq_mean(id_s:id_e), NFFT)/L;
%f = Fs/2*linspace(0,1,NFFT/2+1);

%L = length(data_norm_sq(id_s:id_e));
%NFFT = 2^nextpow2(L);
%Y = fft(data_norm_sq(id_s:id_e), NFFT)/L;
%f = Fs/2*linspace(0,1,NFFT/2+1);

% Plot fft
figure;
plot(f,2*abs(Y(1:NFFT/2+1)));
title('FFT for a small amount of samples squared norm of smoothed');