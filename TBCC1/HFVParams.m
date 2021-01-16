function [V_, a_, h_, theta_, q_] = HFVParams(V, a, q, theta, be, T)

m = 300;
g = 32.17;
Iyy = 50000;
S = 17;
p0 = 6.7429*10^(-5);
hs = 21358.8;
h0 = 85000;
c_ = 17;
zT = 8.36;
ce = -1.2897;
q_ = 70;

C0L = -0.018714;
CaL = 4.6773;
CbeL = 0.76224;
C0D = 0.010131;
CaD = -0.045315;
Ca2D = 5.8224;
CbeD = 2.7699*10^(-4);
Cbe2D = 0.81993;
C0Ma = -0.18979;
CaMa = 2.1335;
Ca2Ma = 6.2926;

b1 = -3.7693*10^5;
b2 = -3.7225*10^4;
b3 = 2.6814*10^4;
b4 = -1.7277*10^4;
b5 = 3.5542*10^4;
b6 = -2.4216*10^3;
b7 = 6.3785*10^3;
b8 = -1.0090*10^2;


L = q_*S*(CaL*a + CbeL*be + C0L);
D = q_*S*(Ca2D*a^2 + CaD*a +Cbe2D*be^2 + CbeD*be + C0D);
M = zT*T + q_*S*c_*(Ca2Ma*a^2 + CaMa*a + C0Ma+ce*be);% M = 1;

V_ = (T*cos(a) - D)/m - g*sin(theta-a);
a_ = (-L - T*sin(a))/(m*V) + q + g*cos(theta-a)/V;
h_ = V*sin(theta-a);
theta_ = q;
q_ = M/Iyy;

end