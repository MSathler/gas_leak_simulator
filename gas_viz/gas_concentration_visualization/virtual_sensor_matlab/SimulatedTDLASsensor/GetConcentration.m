function [ realConcentration ] = GetConcentration( laserCoordinatorX, laserCoordinatorY, laserCoordinatorZ, logFileMatrix)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

positions = [laserCoordinatorX, laserCoordinatorY, laserCoordinatorZ];

[~,Locb] = ismember(logFileMatrix(:,1:3), positions,'rows');


realConcentration = logFileMatrix( find(Locb),4);