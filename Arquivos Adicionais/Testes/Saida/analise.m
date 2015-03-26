clear all; close all; clc;
s = tf('s');
Dt = 0.000270;
Fs = 1/Dt;
L = 7000;
fileID = fopen('outM.txt','r');
formatSpec = '%f';
A = fscanf(fileID,formatSpec);
fclose(fileID);
t = (0:6999).*Dt;
figure
plot (t,smooth(A,200))
NFFT = 2^nextpow2(L);
Y = fft(A,NFFT)/L;
f = Fs/2*linspace(0,1,NFFT/2+1);
figure
plot(f,2*abs(Y(1:NFFT/2+1))) 
title('Single-Sided Amplitude Spectrum of y(t)')
xlabel('Frequency (Hz)')
ylabel('|Y(f)|')
axis([0 50 0 1.4])
%Frequência de Oscilação: 1.356 Hz
% te2% = 1.81 s
% te2% ~ 4 / ksi * wn = 4 / tau
% 
%A curva pode ser aproximada  por:
wd = 1.356;
tau = 4 / 1.81;
t1 = 0:0.01:3;
x = 3.5 * exp(-tau*t1).*(sin(2*pi*wd .*t1-pi/2));
% Nessa função contínua:
%   pss está em 0.34
%   picos: (0.34,1.602)(1.07,0.3134) 
figure
plot (t1,x)

% A partir dos gráficos adquiridos, pode-se deduzir as constantes do
% sistema:
gain = 3.5;
% Comentado porque ficou muito fora da realidade,
% tentando algo mais sutil
% pss = 100*0.34/3.5;
% ksi = log (100/pss)/sqrt(pi^2+(log(100/pss))^2);
% wn = wd / sqrt(1 - ksi^2);

delta = log(1.602/0.3134);
ksi = delta / sqrt(4*pi^2+delta^2);
wn = (tau / ksi);

% Agora, pode-se obter a pseudo-planta:
ps =  gain* wn^2 / (s^2+2*ksi*wn*s+wn^2);



%Daqui pra frente, Rooney brincando com controladores do Serpa



figure
step(ps)
% Sistema indo de 0 a 7 agora, tecnicamente.
% Pensando, o limite de pss é 7, e busca-se minimizar o tempo de
% estabilização.

%Zieger nichols

figure
rlocus(ps)
% Nota-se que o sistema é sempre teoricamente estável, mas também que o seu
% overshoot aumenta com o ganho, e também a frequência de oscilação.
% Para um ganho teórico de Kp = 1 (100% de aproveitamento dos sensores):
% Km = Kp / .6 = 1.667, onde tem-se wm = 22.9 rad/s
% O controlador seria:
km = 10/6;
wm = 22.9;
kp = 1
kd = kp * pi / (4*km)
ki = kp * wm / pi
ks = kp + kd * s + ki / s;
gs = ks * ps;
ts = feedback(gs,1)
figure
step(3.5*ts)

%Ot lembrou que essa não é a planta de verdade. É realimentada já. Eu
%pensei, e dá pra obter matematicamente a saída da planta se souber o H(s).
%Não sei exatamente como fazer isso, to fazendo considerações. por exemplo,
%se a planta tiver um H(s) unitário, basta fazer feedback(ps,-1)

ps2 = feedback (ps,-1)
figure
step(ps2)
figure
rlocus (ps2)
%Nesse caso, para ganho 1.66, wm = 16 rad/s
% Pode-se fazer então:
km = 10/6;
wm = 16;
kp = 1
kd = kp * pi / (4*km)
ki = kp * wm / pi
ks2 = kp + kd * s + ki / s;
gs2 = ks2 * ps2;
ts = feedback(gs2,1)
figure
step(3.5*ts)
