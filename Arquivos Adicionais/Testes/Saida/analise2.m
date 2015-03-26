clear all; close all; clc;
s = tf('s');
Dt = 0.000897;
Fs = 1/Dt;
L = 7000;
fileID = fopen('curva06.txt','r');
formatSpec = '%f';
A = fscanf(fileID,formatSpec);
fclose(fileID);
fileID = fopen('tempo06.txt','r');
formatSpec = '%f';
t = fscanf(fileID,formatSpec);
fclose(fileID);
temp = 0;
figure
plot (t(1:1042),A(1:1042),'o')
t = t(1:294)
A = A(1:294)
figure
plot (t,A,'o')
figure
plot (t,smooth(A,200))
NFFT = 2^nextpow2(L);
Y = fft(A,NFFT)/L;
f = Fs/2*linspace(0,1,NFFT/2+1);
figure
plot(f,2*abs(Y(1:NFFT/2+1))) 
wd = 1.796
y = sin(2*pi*wd .*t-pi/2)
figure
plot (t,y,t,A,'or')
syms x y
f = sin(2*pi*wd *x-pi/2);
laplace(f, x, y)
