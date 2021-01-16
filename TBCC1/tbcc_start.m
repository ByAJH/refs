clc,clear
rocket_setup_everything;
ramjet_setup_everything;
addpath('.\ramjet');
addpath(genpath(pwd));
disp(".\TBCC-Test")
cd  .\TBCC-Test
%------ Performs example setup ---------
TBCC_setup_everything;
cd ..
clear TMATS;