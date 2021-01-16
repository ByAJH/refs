function y = combustion(M4,T4,P4)
    R = 287;
    g = 1.4;
    k = 4;
    Tcombuste = 1800;
    a4 = (g.*R.*T4).^0.5;
    u4 = M4.*a4;
    A4 = 6;
    [M41,T41,P41,pstarf,p0starf,Tstarf] = flameholder(M4,T4,P4,k);
    rho41 = P41./R./T41;
    a41 = (g.*R.*T41).^0.5;
    u41 = M41.*a41;
    mdot41 = rho41.*A4.*u41;
    
    [M5,P5,T5,f,Pstarc,Tstarc] = combustionchamber(M41,T41,P41,Tcombuste);
    rho5 = P5./R./T5;
    a5 = (g.*R.*T5).^0.5;
    u5 = M5.*a5;
    
    uavg = 0.5.*(u4 + u5);
    pb = P4./101.325e3;
    Tb = T4.*(1+(g+1)./2.*M4.^2);
    tb = 325e-4.*pb.^(-1.6).*exp(-8e-4.*Tb);
    L = uavg.*tb; %º∆À„»º…’ “≥§∂»
    y = [M5,P5,T5,f,mdot41];
end
    
    
    
    
    

