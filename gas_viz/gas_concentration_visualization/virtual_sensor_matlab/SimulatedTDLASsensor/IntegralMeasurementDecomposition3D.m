function distanceInGrid = IntegralMeasurementDecomposition3D(x_startingPoint, y_startingPoint, z_startingPoint, laserCoordinatorX, laserCoordinatorY, laserCoordinatorZ, cellSize, azimuthal_xy, azimuthal_z)

last_x = x_startingPoint;
last_y = y_startingPoint;
last_z = z_startingPoint;
distanceInGrid = zeros(size(laserCoordinatorX));

for i=1:length(laserCoordinatorX)
    if cos(azimuthal_xy)>0
        xGridWall = laserCoordinatorX(i) + cellSize/2;
    else
        xGridWall = laserCoordinatorX(i) - cellSize/2;
    end
    
    if sin(azimuthal_z)>0
        zGridWall = laserCoordinatorZ(i) + cellSize/2;
    else
        zGridWall = laserCoordinatorZ(i) - cellSize/2;
    end
    
    if sin(azimuthal_xy)>0
        yGridWall = laserCoordinatorY(i) + cellSize/2;
    else
        yGridWall = laserCoordinatorY(i) - cellSize/2;
    end
    
    xDistance = xGridWall-last_x;
    yDistance = yGridWall-last_y;
    zDistance = zGridWall-last_z;
    
    closestDistance = min([xDistance, yDistance, zDistance]);
    
    switch closestDistance
        
        case  xDistance
            next_x = xGridWall;
            next_y = last_y + (next_x-last_x)*tan(azimuthal_xy);          
            next_z = last_z + tan(azimuthal_z)*norm([next_x-last_x, next_y-last_y]);
            
        case  yDistance
            next_y = yGridWall;
            next_x = last_x + (next_y-last_y)*cot(azimuthal_xy);
            next_z = last_z + tan(azimuthal_z)*norm([next_x-last_x, next_y-last_y]);
            
        case  zDistance
            next_z = zGridWall;
            next_x = last_x + (next_z-last_z)*cot(azimuthal_z)*cos(azimuthal_xy);
            next_y = last_y + (next_z-last_z)*cot(azimuthal_z)*sin(azimuthal_xy);
            
    end

    pointLast = [last_x, last_y, last_z];
    pointNext = [next_x, next_y, next_z];
    
    distanceInGrid(i) = pdist([pointLast; pointNext]);

end