function [ totalConcentration ] = VirtualSsensorResponse2D_TDLAS(laserLength, azimuthal_start, azimuthal_end, numAzimuthal, originalX, originalY, originalZ, logFileMatrix, environment )

cellSize = 0.5;

% % starting angle
% % azimuthal_start = 3*(pi/2);
% % ending angle
% % azimuthal_end = 5*(pi/2);

azimuthalStepSize = (azimuthal_end - azimuthal_start)/numAzimuthal;

for azimuthal = azimuthal_start:azimuthalStepSize:azimuthal_end
    hit = 0; % this variable is to indicate the obstacle
    
    internalIndex = 1;
    
    x_endingPoint = originalX + laserLength*cos(azimuthal);
    y_endingPoint = originalY + laserLength*sin(azimuthal);
    
    if x_endingPoint<originalX
        x_startingPoint = x_endingPoint;
        x_endingPoint = originalX;
    else
        x_startingPoint = originalX;
    end
    
    if y_endingPoint<originalY
        y_startingPoint = y_endingPoint;
        y_endingPoint = originalY;
    else
        y_startingPoint = originalY;
    end
    
    laser_x = x_startingPoint:cellSize:x_endingPoint;
    laser_y = y_startingPoint:cellSize:y_endingPoint;
    
    size_x = length(laser_x);
    size_y = length(laser_y);
    
    if size_x>=size_y
        numBox = size_x;
        laser_y = (laser_x - ones(size(laser_x))*x_startingPoint)*tan(azimuthal) + y_startingPoint*ones(size(laser_x));
    else
        numBox = size_y;
        laser_x = (laser_y - ones(size(laser_y))*y_startingPoint)*cot(azimuthal) + x_startingPoint*ones(size(laser_y));
    end
    
    % plot(laser_x, laser_y, '-*')
    
    laser_z = ones(1,numBox)*originalZ;
    
    concentratonInCell = zeros(1,numBox);
    
    laserCoordinatorX = zeros(numBox,1);
    laserCoordinatorY = zeros(numBox,1);
    laserCoordinatorZ = zeros(numBox,1);
    
    while internalIndex<=numBox && hit==0
        currentX = laser_x(internalIndex);
        if currentX>60
            currentX = 60;
            hit = 1;
        elseif currentX<=0
            currentX = 0;
            hit = 1;
        end
        
        currentY = laser_y(internalIndex);
        if currentY>20
            currentY = 20;
            hit = 1;
        elseif currentY<=0
            currentY = 0;
            hit = 1;
        end
        
        currentZ = laser_z(internalIndex);
        if currentZ>10
            currentZ = 10;
            hit = 1;
        elseif currentZ<=0
            currentZ = 0;
            hit = 1;
        end
        
        %         offset = currentZ*(2*(numRow*numColumn + 1)) + 2*(currentX*numColumn + currentY);
        
        %     coordinate = [currentX, currentY];
        %     disp(coordinate)
        
        value = GetEnviornmentInformation(environment, round(currentX)+1, round(currentY)+1);
        
        if value == 1
            hit = 1;
        else
            
            laserCoordinatorX(internalIndex) = GetCellCenter(currentX, cellSize);
            laserCoordinatorY(internalIndex) = GetCellCenter(currentY, cellSize);
            laserCoordinatorZ(internalIndex) = GetCellCenter(currentZ, cellSize);
            
        end
        
        if hit ~=1
            
            %             disp([internalIndex_x, laserCoordinatorX(internalIndex_x), laserCoordinatorY(internalIndex_y), laserCoordinatorZ(internalIndex_z)])
            concentratonInCell(internalIndex) = GetConcentration( laserCoordinatorX(internalIndex), laserCoordinatorY(internalIndex), laserCoordinatorZ(internalIndex), logFileMatrix);
            
            internalIndex = internalIndex+1;
            
        elseif isempty(find(laserCoordinatorX>0, 1)) || isempty(find(laserCoordinatorY>0, 1)) || isempty(find(laserCoordinatorZ>0, 1))
            disp('no laser')
            totalConcentration = 0;
            
            return
        end
    end
    
    segmentInCell = IntegralMeasurementDecomposition2D(x_startingPoint, y_startingPoint, laserCoordinatorX, laserCoordinatorY, cellSize, azimuthal);
    
    concentrationAlongLaser = zeros(size(segmentInCell));
    concentrationAlongLaser(1:length(concentratonInCell)) = concentratonInCell;
    
    totalConcentration = segmentInCell'*concentrationAlongLaser;
    
    results = [azimuthal,  totalConcentration];
    disp(results);
end
