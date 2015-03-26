clear all; close all; clc;
s = tf('s');
Dt = 0.000897;
Fs = 1/Dt;
L = 7000;
fileID = fopen('curvahell.txt','r');
formatSpec = '%f';
A = fscanf(fileID,formatSpec);
fclose(fileID);
fileID = fopen('tempohell.txt','r');
formatSpec = '%f';
t = fscanf(fileID,formatSpec);
fclose(fileID);
temp = 0;
for i =1:587
    temp = temp + t(i);
    t(i)=temp*8;
end
figure
plot (t(1:587),A(1:587),'o')
