function [sensorTimeArray, filteredResponse] = VirtualSensorResponse_MOX( realConcentration, gasType, sensorModel, sensorTimeArray, filteredResponse, timeStep, cons, initialization )


% initial sensor values:
if initialization==true
    if sensorModel==1
        filteredResponse = 21;
        sensorTimeArray(timeStep)=21;
    elseif sensorModel==2 && gasType ~=3
        filteredResponse = 1;
        sensorTimeArray(timeStep)=1;
    elseif sensorModel == 3
        filteredResponse = 8.7;
        sensorTimeArray(timeStep)=8.7;
    end
    
    return
end

if realConcentration==0
    switch sensorModel
        case 1
            y_out = 21;
        case 2
            y_out = 1;
        case 3
            y_out = 8.7;
            
        otherwise
            disp('unknown sensor model')
    end
else
    %     A*e^B + C*e^D
    if gasType<4
        y_out = mp((cons{sensorModel}(gasType,1)*exp(cons{sensorModel}(gasType,2)*realConcentration)) + (cons{sensorModel}(gasType,3)*exp(cons{sensorModel}(gasType,4)*realConcentration)));
    else
        y_out = mp((cons{sensorModel}(1,1)*exp(cons{sensorModel}(1,2)*realConcentration)) + (cons{sensorModel}(1,3)*exp(cons{sensorModel}(1,4)*realConcentration)));
    end
end

% storing sensor response in array for comparision:
sensorTimeArray(timeStep) = y_out;

% comparision between last value and current value (the case of decay):
if sensorTimeArray(timeStep)> sensorTimeArray(timeStep-1)
    tau = 40.7*0.15;
end

% comparision between last value and current value (the case of rise):
if sensorTimeArray(timeStep)< sensorTimeArray(timeStep-1)
    tau = 7.4*0.04;
end

if sensorTimeArray(timeStep) == sensorTimeArray(timeStep-1)
    if sensorTimeArray(timeStep)>filteredResponse
        tau = 40.7*0.15;
    elseif sensorTimeArray(timeStep)<filteredResponse
        tau = 7.4*0.04;
    else
        tau = 0;
    end
end

alpha = 1/(tau+1);

% filtered response:
filteredResponse = mp((alpha*y_out)+(1-alpha)*filteredResponse);

% TGS2600 hydrogen response=0.1.
if sensorModel==1 && gasType==2
    filteredResponse = 0.100000;
end

end
