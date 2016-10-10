clc; clear all; close all;

%%%%%%% TIME PROBLEMS (left foot, right foot)!!!!!

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

plot(tx,xdata,'k'); hold on;
plot(ty,ydata,'r');
plot(tz,zdata,'b');
plot(t(left_foot),u_l, 'xr');
plot(t(right_foot),u_r, '+b');
hold off;

figure;
plot(t, xdata_200Hz,'k'); hold on;
plot(t, ydata_200Hz,'r');
plot(t, zdata_200Hz,'b');

plot(t(left_foot),u_l, 'o');
plot(t(right_foot),u_r, 'o'); 

hold off;
title('200Hz');

legend('X data','Y data','Z data');