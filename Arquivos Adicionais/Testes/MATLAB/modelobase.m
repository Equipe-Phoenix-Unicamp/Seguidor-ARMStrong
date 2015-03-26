clear all; close all; clc;
m = 0.22;   % massa Kg
h = 0.108;  % comprimento m
w = 0.145;  % largura m

% Specs do motor:
% Specs
% Gear ratio: 30:1
% Free-run speed @ 6V: 1000 rpm
% Free-run current @ 6V: 120 mA
% Stall current @ 6V: 1600 mA
% Stall torque @ 6V: 8 ozáin
Tm = 0.05649241466664; %N.m

% Extended motor shaft?: No
% Size: 24 x 10 x 12 mm
% Weight: 0.34 oz
% Shaft diameter: 3 mm

J = m* (h^2+w^2)/12;


