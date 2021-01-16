function y = nozzle(P1,M5,P5,T5,f,mdot,F)
    g = 1.4;
    R = 287;
    G = g.*(2./(g+1)).^((g+1)./2./(g-1));
    mdotf = f*mdot;
    [T05, P05] = isentropic(M5,T5,P5);
    A4 = 6;
    P7 = P1;
    P07 = P05;
    thetaf = 36;
    M7 = ((2./(g-1)).*((P07./P7).^((g-1)./g)-1)).^0.5;
    a0 = (g.*R.*T05).^0.5;
    Astar = mdot*a0/(P05*G);
    
    %A7 = Astar./M7.*(2./(g+1).*(1+(g-1)./2.*M7.^2)).^((g+1)./2./(g-1));
    
    A7 = 11.3826; %Î²Åç¹ÜÃæ»ý
    [T7,~] = rev_isentropic(M7,T05,P05);
    a7 = (g.*R.*T7).^0.5;
    u7 = a7.*M7;
    
    %thrust calculation

    pae = P7.*A7;
    
    T = (mdot+mdotf)*u7 + pae - F;
    display(T);
    Isp = T./mdotf./9.81;
    y = [M7,T,Isp];