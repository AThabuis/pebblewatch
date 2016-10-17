clc; clear all; close all;

% Loads data file
load('Data_Matlab_TP/dataTP2.mat');

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

% get time of each step
t_left  = t(left_foot);
t_right = t(right_foot);
u_l = zeros(size(left_foot)); 
u_r = zeros(size(right_foot)); 

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
plot(t_left,u_l, 'xr');
plot(t_right,u_r, '+b');
hold off;
legend('norm squared','norm squared of averaged data');

% L = length(data_smooth(2:end));
% NFFT = 2^nextpow2(L);
% Y = fft(data_smooth(2:end), NFFT)/L;
% f = Fs/2*linspace(0,1,NFFT/2+1);
% 
% figure;
% plot(f,2*abs(Y(1:NFFT/2+1)));

window_start = 0.5;
window_end = 5;

t_tot_fft   = 5;  % total time for fft in sec
kk_max = floor(t_end/t_tot_fft);

for kk = 0:kk_max
t_start_fft = kk*t_tot_fft; % start time for fft in sec

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

id_left  = find(t_left > t_s & t_left < t_e);
id_right = find(t_right > t_s & t_right < t_e);

if length(id_left) > 1
    l_step_f = (length(id_left)-1)/(t_left(id_left(end))-t_left(id_left(1)));
else
    l_step_f = 0; % modify this - use previous or next step to compute freq
end

if length(id_right) > 1
    r_step_f = (length(id_right)-1)/(t_right(id_right(end))-t_right(id_right(1)));
else
    r_step_f = 0; % modify this - use previous or next step to compute freq
end

step_f = (length(id_left)+length(id_right)-1)/...
         (max(t_left(id_left(end)),t_right(id_right(end)))- ...
          min(t_left(id_left(1)),t_right(id_right(1))));

% Computes fft for t_tot_fft time of samples
%L = length(data_norm_sq_mean(id_s:id_e));
%NFFT = 2^nextpow2(L);
%Y = fft(data_norm_sq_mean(id_s:id_e), NFFT)/L;
%f = Fs/2*linspace(0,1,NFFT/2+1);

L = length(data_norm_sq(id_s:id_e));
NFFT = 2^nextpow2(L);
Y = fft(data_norm_sq(id_s:id_e), NFFT)/L;

f = Fs/2*linspace(0,1,NFFT/2+1);

% Plot fft
figure;
stem(f,2*abs(Y(1:NFFT/2+1))); hold on;
title('FFT for a small amount of samples squared norm of smoothed');
plot([f(1),f(end)],[0.3,0.3]);

plot([window_start,window_end],[0,0],'k','LineWidth',2); plot([window_start,window_end],[1,1],'k','LineWidth',2);
plot([window_start,window_start],[0,1],'k','LineWidth',2); plot([window_end,window_end],[0,1],'k','LineWidth',2);

plot([l_step_f,l_step_f],[0, max(abs(Y))],'r-');
plot([r_step_f,r_step_f],[0, max(abs(Y))],'b-.');
plot([step_f,step_f]    ,[0, max(abs(Y))*0.75],'m-x');

text(l_step_f,max(abs(Y)),strcat('  l: ',num2str(l_step_f)),'VerticalAlignment','top');
text(r_step_f,max(abs(Y)),strcat('  r: ',num2str(r_step_f)),'VerticalAlignment','bottom');
text(step_f  ,max(abs(Y))*0.75,strcat('  b: ',num2str(step_f))  ,'VerticalAlignment','top');

hold off;
disp(t_start_fft);
pause;
%close all;
end