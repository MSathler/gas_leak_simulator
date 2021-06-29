% function [ totalConcentration ] = VirtualSsensorResponse_TDLAS(laserLength, azimuthal_start, azimuthal_end, numAzimuthal, originalX, originalY, originalZ, logFileMatrix, environment )

cellSize = 0.5;
logFileMatrix = MK;
laserLength = 5;
laserCoordinatorX = zeros(laserLength,1);
laserCoordinatorY = zeros(laserLength,1);
laserCoordinatorZ = zeros(laserLength,1);

azimuthal_xy = pi/3;

originalX= 9;
originalY= 5;
originalZ=0.5;

azimuthal_z = pi/10;

hit = 0; % this variable is to indicate the obstacle

internalIndex = 1;

laserLength_xy = abs(laserLength*cos(azimuthal_z));

x_endingPoint = originalX + laserLength_xy*cos(azimuthal_xy);
y_endingPoint = originalY + laserLength_xy*sin(azimuthal_xy);
z_endingPoint = originalZ + laserLength*sin(azimuthal_z);

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

if z_endingPoint<originalZ
    z_startingPoint = z_endingPoint;
    z_endingPoint = originalZ;
else
    z_startingPoint = originalZ;
end

laser_x = x_startingPoint:cellSize:x_endingPoint;
laser_y = y_startingPoint:cellSize:y_endingPoint;
laser_z = z_startingPoint:cellSize:z_endingPoint;

size_x = length(laser_x);
size_y = length(laser_y);
size_z = length(laser_z);

numBoxCrossed = max([size_x, size_y, size_z]);

switch numBoxCrossed
    case size_x
        numBox = size_x;
        laser_y = (laser_x - ones(size(laser_x))*x_startingPoint)*tan(azimuthal_xy) + y_startingPoint*ones(size(laser_x));
        laser_z = (laser_x - ones(size(laser_x))*x_startingPoint)/cos(azimuthal_xy)*tan(azimuthal_z) + z_startingPoint*ones(size(laser_x));
        
    case size_y
        numBox = size_y;
        laser_x = (laser_y - ones(size(laser_y))*y_startingPoint)*cot(azimuthal_xy) + x_startingPoint*ones(size(laser_y));
        laser_z = (laser_y - ones(size(laser_y))*y_startingPoint)/sin(azimuthal_xy)*tan(azimuthal_z) + z_startingPoint*ones(size(laser_y));
        
    case size_z
        numBox = size_z;
        laser_x = (laser_z - ones(size(laser_z))*z_startingPoint)*cot(azimuthal_z)*cos(azimuthal_xy) + x_startingPoint*ones(size(laser_z));
        laser_y = (laser_z - ones(size(laser_z))*z_startingPoint)*cot(azimuthal_z)*sin(azimuthal_xy) + y_startingPoint*ones(size(laser_z));
end

% plot3(laser_x, laser_y, laser_z, '-*')

concentratonInCell = zeros(1,numBox);

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
    
    segmentInCell = IntegralMeasurementDecomposition3D(x_startingPoint, y_startingPoint, z_startingPoint, laserCoordinatorX, laserCoordinatorY, cellSize, azimuthal_xy, azimuthal_z);
    
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

concentrationAlongLaser = zeros(size(segmentInCell));
concentrationAlongLaser(1:length(concentratonInCell)) = concentratonInCell;

totalConcentration = segmentInCell'*concentrationAlongLaser;

results = [azimuthal_xy,  totalConcentration];
disp(results);