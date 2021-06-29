 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 addpath('C:\Program Files\MATLAB\Multiprecision Computing Toolbox\')
 mp.Digits(650);
 
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 % constants: A, B, C, D

% TGS2620
cons(1,1) = {[6.018, -0.01662, 0.9832, -0.00056651;
    18.79, -0.01662, 6.138,  -0.0002136;
    3.884, -0.0127,  0.8745, -0.0003222]};
% TGS2600
cons(1,2) = {[0.4668,  -0.3249, 0.3293, -0.01051;
    0.2202,  -0.1122, 0.8356, -0.001932;
    0.0,     0.0,     0.0,    0.0]};
% TGS2611
cons(1,3) = {[4.766,  -0.001639, 3.497, -7.3480e-05;
    3.286,  -0.002211, 1.806, -0.000103;
    4.535,  -0.001723, 2.69,  -5.191e-05]};

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Preparation
% Load the simulation result of the first snapshot in order to know the
% number of the positions where are considered to take samples.

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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Load data

% finf = dir('D:\simulationResults\*');
% n = length(finf);
% % data = cell(n,1);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Initialization
numSensors = size(dataSample,1);
numSnapshots = 200;
gasType = 1;


filteredResponse = zeros(numSensors,1);

sensorArray = [1 2 3];

sensorCell{1,1} = zeros(numSensors, numSnapshots);
sensorCell{1,2} = zeros(numSensors, numSnapshots);
sensorCell{1,3} = zeros(numSensors, numSnapshots);

readingCell{1,1} = zeros(numSensors, numSnapshots);
readingCell{1,2} = zeros(numSensors, numSnapshots);
readingCell{1,3} = zeros(numSensors, numSnapshots);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% for k=1:n
for k=1:200
 %     fileName = ['D:\simulationResults\',finf(k).name];
    
    % open the log file of the No.k snapshot
    fileName = ['D:\simulationResults\gasType0_simulation',num2str(k)];
   
    
    fidin=fopen(fileName);
   
    if fidin>0
        disp(fileName);
        
        % create a temporary .txt file to keep the read data  
        fidout=fopen('D:\rawData\mkmatlab.txt','w'); 
        
        disp('writing')
        while ~feof(fidin) % judge if the current line is in the end of the file
            tline=fgetl(fidin); % read the line from the file
            if double(tline(1))>=48&&double(tline(1))<=57 % judge if the first element of the current line is a number
                fprintf(fidout,'%s\n\n',tline); % if the current line is of numbers, then write this line into the temporary .txt file.
                continue % otherwise read the next line
            end
        end
        fclose(fidout);
        
        disp('importing the data to the workspace')
        MK=importdata('D:\rawData\mkmatlab.txt'); % import the temporary .txt file into the workspace
        
        disp('MOX sensor starts working...')
        
       
        
        timeStep = k;
        
        for positionID = 1:numSensors
            for sensorModel = 1:length(sensorArray)
                
                realConcentration = mp(MK(positionID,4)*4.06*10^(-17)); % translate the unit molecules/m^3 to ppm
                sensorID = positionID;
                
                sensorTimeArray = sensorCell{sensorModel}(sensorID,:);
                
                if timeStep<2
                    initialization = true;
                    filteredResponse = readingCell{sensorModel}(sensorID,timeStep);
                else
                    initialization = false;
                    filteredResponse = readingCell{sensorModel}(sensorID,timeStep-1);
                end
                
                [sensorTimeArray, filteredResponse] = VirtualSensorResponse_MOX( realConcentration, gasType, sensorModel, sensorTimeArray, filteredResponse, timeStep, cons, initialization);
                
                sensorCell{sensorModel}(sensorID,:) = sensorTimeArray;
                readingCell{sensorModel}(sensorID,timeStep) = filteredResponse;
            end
            
        end
        
        disp(['the current time step is ', num2str(timeStep)])
        %         timeStep = timeStep + 1;
        
    end
    
    
    
end

% the first three elements of each row are (x,y,z), which is the coordinate where the MOX sensor is set.

resistanceRatioMatrix_TGS2620 = [MK(:,1:3) readingCell{1,1}];
resistanceRatioMatrix_TGS2600 = [MK(:,1:3) readingCell{1,2}];
resistanceRatioMatrix_TGS2611 = [MK(:,1:3) readingCell{1,3}];

% the path(s) for the resulting .mat file(s)
save D:\resistanceMatrix_TGS2620  resistanceRatioMatrix_TGS2620
save D:\resistanceMatrix_TGS2600  resistanceRatioMatrix_TGS2600
save D:\resistanceMatrix_TGS2611  resistanceRatioMatrix_TGS2611

conductanceMatrix_TGS2620 = [resistanceRatioMatrix_TGS2620(:,1:3) 1./resistanceRatioMatrix_TGS2620(:,4:end)];
conductanceMatrix_TGS2600 = [resistanceRatioMatrix_TGS2600(:,1:3) 1./resistanceRatioMatrix_TGS2600(:,4:end)];
conductanceMatrix_TGS2611 = [resistanceRatioMatrix_TGS2611(:,1:3) 1./resistanceRatioMatrix_TGS2611(:,4:end)];

save D:\conductanceMatrix_TGS2620  conductanceMatrix_TGS2620
save D:\conductanceMatrix_TGS2600  conductanceMatrix_TGS2600
save D:\conductanceMatrix_TGS2611  conductanceMatrix_TGS2611