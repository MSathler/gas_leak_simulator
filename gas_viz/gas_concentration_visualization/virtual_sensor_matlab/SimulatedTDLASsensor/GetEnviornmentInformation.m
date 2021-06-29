function [ wall ] = GetEnviornmentInformation(environment, currentX, currentY)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

if environment(currentX, currentY)==1
    wall = 1;
else
    wall = 0;

end