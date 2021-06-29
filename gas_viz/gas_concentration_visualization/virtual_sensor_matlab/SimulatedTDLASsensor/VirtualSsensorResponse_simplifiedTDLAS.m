function [ concentratonAlongLaser ] = VirtualSsensorResponse_simplifiedTDLAS( originalX, originalY, originalZ, logFileMatrix, environment )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

cellSize = 0.5;

numInteral = 20;
laserCoordinatorX = zeros(numInteral,1);
laserCoordinatorY = zeros(numInteral,1);
laserCoordinatorZ = zeros(numInteral,1);

% starting angle
azimuthal_start = 3*(pi/2);

% ending angle
azimuthal_end = 5*(pi/2);

% angle of incedence
polar = 8.5*(pi/18);


stepSize = (azimuthal_end-azimuthal_start)/8;
hit = 0; % this variable is to indicate the obstacle
for azimuthal = azimuthal_start:stepSize:azimuthal_end
    internalIndex = 1;
    
    while internalIndex<=numInteral && hit==0
        currentX = originalX + internalIndex*cos(azimuthal)*sin(polar);
        if currentX>60
            currentX = 60;
            hit = 1;
        end
        currentY = originalY + internalIndex*sin(azimuthal)*sin(polar);
        if currentY>20
            currentY = 20;
            hit = 1;
        end
        currentZ = originalZ + internalIndex*cos(polar);
        if currentZ>10
            currentZ = 10;
            hit = 1;
        end
        
        %         offset = currentZ*(2*(numRow*numColumn + 1)) + 2*(currentX*numColumn + currentY);
        
        value = GetEnviornmentInformation(environment, currentX, currentY);
        
        if value == 1
            hit = 1;
        else
            laserCoordinatorX(internalIndex) = GetCellCenter(currentX, cellSize);
            laserCoordinatorY(internalIndex) = GetCellCenter(currentY, cellSize);
            laserCoordinatorZ(internalIndex) = GetCellCenter(currentZ, cellSize);
            
        end
        
        internalIndex = internalIndex+1;
    end
    
    concentratonAlongLaser = zeros(internalIndex-1,1);
    for j=1:(internalIndex-1)
%         disp([laserCoordinatorX(j), laserCoordinatorY(j), laserCoordinatorZ(j)])
        concentratonAlongLaser(j) = GetConcentration( laserCoordinatorX(j), laserCoordinatorY(j), laserCoordinatorZ(j), logFileMatrix);
    end

    
end

