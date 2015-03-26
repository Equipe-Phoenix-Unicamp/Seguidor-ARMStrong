clear all; close all; clc;
Vf = 5; %V
Ra = 4; %Ohm
w = 920; %rpm
w = w*pi/30; %rad/s
Io = 0.053;
k = (Vf - Io * (Ra+1))/w

B = Io * k / w;

Ipicotravado = 973.17;
Rb = Vf / Io - (Ra+1)
Ra1 = Ra +1;
Ia = Vf *(1/(Ra1+Rb)+Rb/(Ra1*(Ra1+Rb))*exp(-1))

m = 0.164 %kg
Diam = .033

J = m/2*(Diam/2)^2
J/B
g = 9.81
m/2 * g * Diam /2

%Encontrado 70.2 rad / s
v = 70.2 * 0.033