function MWS = setup_Inputs(MWS)
%		T-MATS -- setup_Inputs.m
% *************************************************************************
% written by Jeffyres Chapman
% NASA Glenn Research Center, Cleveland, OH
% Dec 19th, 2012
%
% This file contains setup constants for the Plant Solver Template
% defining the dynamic input. To create different test vectors simply over
% right this file with the desired time dependent input.
% *************************************************************************

MWS.in.SimTime = 90; % MWS.Solve.T set to 0.015sec
MWS.in.t_vec = [0 5 10 15 30 45 60];
MWS.in.Ndmd = [9000 9000 10000 10000 11000 11000 11000];
MWS.in.Ndmd_stop = [7000 7000 7000 7000 7000 7000 7000];
MWS.in.Ndmd_stop1 = [4000 4000 4000 4000 4000 4000 4000];
MWS.in.Ndmd_stop2 = [3000 3000 3000 3000 3000 3000 3000];
MWS.in.Alt = [0 0 0 0 0 0 0];
MWS.in.Ma = [0 0 0.5 0.5 1 1 1];
MWS.in.W = [56.7 56.7 56.7 56.7 56.7 56.7 56.7];


