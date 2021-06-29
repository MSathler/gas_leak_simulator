function distanceInGrid = IntegralMeasurementDecomposition(x_startingPoint, y_startingPoint, laserCoordinatorX, laserCoordinatorY, cellSize, azimuthal)

last_x = x_startingPoint;
last_y = y_startingPoint;

distanceInGrid = zeros(size(laserCoordinatorX));

for i=1:length(laserCoordinatorX)
    if cos(azimuthal)>0
        xGridWall = laserCoordinatorX(i) + cellSize/2;
    else
        xGridWall = laserCoordinatorX(i) - cellSize/2;
    end
    
    if sin(azimuthal)>0
        yGridWall = laserCoordinatorY(i) + cellSize/2;
    else
        yGridWall = laserCoordinatorY(i) - cellSize/2;
    end
    
    if (xGridWall-last_x) > (yGridWall-last_y)
        next_y = yGridWall;
        next_x = last_x + (next_y-last_y)*cot(azimuthal);
        
    else
        next_x = xGridWall;
        next_y = last_y + (next_x-last_x)*tan(azimuthal);
    end

    pointLast = [last_x, last_y];
    pointNext = [next_x, next_y];
    
    distanceInGrid(i) = pdist([pointLast; pointNext]);

end