% function Simulated_TDLAS(originalX, originalY, originalZ)

% Load the environment file and convert it to a matrix.
environment = LoadEnviornment;

originalX= 9; 
originalY= 5;
originalZ=0.5;

%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

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
        
        disp('TDLAS sensor starts working...')
    end
    
    timeStep = k;
    
    %     [ concentratonAlongLaser ] = VirtualSsensorResponse2D_TDLAS(4, -(pi/2), 3*(pi/2), 6, originalX, originalY, originalZ, MK, environment);
    [ concentratonAlongLaser ] = VirtualSsensorResponse3D_TDLAS(4, -(pi/2), 3*(pi/2), 6, originalX, originalY, originalZ, MK, environment);
    disp(['the current time step is ', num2str(timeStep)]);
    
    %     disp(strrep(['concentration along the laser: (' sprintf(' %d,', concentratonAlongLaser) ')'], ',)', ')'))
    %     disp(concentratonAlongLaser);
end



% end
