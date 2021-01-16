
axis equal;
clc;
close all;
R = 287;
M1 = 3;
qhv = 120e6;
g = 1.4;
T5 = 1800;
Cp = 1004.5;
Cv = 717.5;
w = 2;
T = 0.015;


H = 30;
if H == 30
    P1 = 1197;
    T1 = 226;
    rho1 = P1./R./T1;
elseif H == 20
    P1 = 5475.21;
    T1 = 216.649;
    rho1 = 0.08803;
elseif H == 10
    P1 = 26437.3;
    T1 = 223.150;
    rho1 = 0.41271;
elseif H == 35
    P1 = 558.969;
    T1 = 237.048;
    rho1 = 0.00821;
end
A4 = 6; %+ k./10;
A3 = 1.8112; %设计点计算得来
lengthinlet = [2.95917360499513,1.66093473765629,1.02164763594759,1.40245542423833];
%Inlet
thetaf = 36;
n = 4; %number of oblique shocks
theta = [];
for k = 1:n
    theta(1,k) = thetaf./n;
end
theta(end+1) = 0;
x = 0;
open_system('ramjet.slx');