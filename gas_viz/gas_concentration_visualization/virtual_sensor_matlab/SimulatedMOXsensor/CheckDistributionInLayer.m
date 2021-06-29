% function [ visMap ] = CheckDistributionInLayer( dataMatrix, layer, timeStep )
% %UNTITLED11 Summary of this function goes here
% %   Detailed explanation goes here
% 
% numElementsLayer = 4960;
% startingIndex = 1+numElementsLayer*(layer-1);
% endingIndex = 1+numElementsLayer*(layer);
% 
% layerMatrix = dataMatrix(startingIndex:endingIndex,timeStep+3);
% 
% xSize = 121;
% ySize = 41;
% 
% visMap = reshape(layerMatrix, xSize, ySize);
% 
% imshow(visMap)
% 
% end


fidin=fopen('D:\simulationResults\gasType0_simulation1'); % open the file of simulation result of the first snapshot
fidout=fopen('D:\rawData\mkmatlab.txt','w'); % creat a temporary .txt that excludes the first line (text) of the original data file.

while ~feof(fidin) % judge if it is the end of the file
    tline=fgetl(fidin); % read in line
    if double(tline(1))>=48&&double(tline(1))<=57 % judge if the current line is text or number
        fprintf(fidout,'%s\n\n',tline); % if the current line contains numbers only, then this line is to be writen into mkmatlab.txt 
        continue % otherwise continue to read the next line
    end
end
fclose(fidout);
dataSample=importdata('D:\rawData\mkmatlab.txt'); % import the sample into the workspace.

