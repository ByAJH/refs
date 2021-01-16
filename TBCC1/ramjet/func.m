function F = func(x, A, B) 

F = 2.*cot(x).*(A^2.*sin(x).^2 - 1)/(A^2.*(1.4+cos(x.*2))+2) - tan(B*pi/180);


