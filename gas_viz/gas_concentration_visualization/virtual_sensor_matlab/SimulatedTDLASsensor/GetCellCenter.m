function [ cellCenter ] = GetCellCenter( coordinate, cellSize )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here

if (coordinate-round(coordinate))>=0
    cellCenter = round(coordinate) + cellSize/2;
else
    cellCenter = round(coordinate) - cellSize/2;
end

if cellCenter<=0
    cellCenter = cellSize/2;
end




end