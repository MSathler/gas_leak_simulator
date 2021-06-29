function filteredResponse = SimulatedMoxTest(realConcentration,sensorTimeArray, filteredResponse)
cons(1,1) = {[6.018, -0.01662, 0.9832, -0.00056651;
              18.79, -0.01662, 6.138,  -0.0002136;
              3.884, -0.0127,  0.8745, -0.0003222]};
          
cons(1,2) = {[0.4668,  -0.3249, 0.3293, -0.01051;
              0.2202,  -0.1122, 0.8356, -0.001932;
              0.0,     0.0,     0.0,    0.0]};
           
cons(1,3) = {[4.766,  -0.001639, 3.497, -7.3480e-05;
              3.286,  -0.002211, 1.806, -0.000103;
              4.535,  -0.001723, 2.69,  -5.191e-05]};  


sensorModel = 1;
gasType = 1;


% initial sensor values:             
if initialization==true
    if sensorModel==1
        filteredResponse = 21;
        sensorTimeArray(2)=21;
    elseif sensorModel==2 && gasType ~=3
        filteredResponse = 1;
        sensorTimeArray(2)=1;
    elseif sensorModel == 3
        filteredResponse = 8.7;
        sensorTimeArray(2)=8.7;
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
        y_out = (cons{sensorModel}(gasType,1)*exp(cons{sensorModel}(gasType,2)*realConcentration)) + (cons{sensorModel}(gasType,3)*exp(cons{sensorModel}(gasType,4)*realConcentration));
    else
        y_out = (cons{sensorModel}(1,1)*exp(cons{sensorModel}(1,2)*realConcentration)) + (cons{sensorModel}(1,3)*exp(cons{sensorModel}(1,4)*realConcentration));
    end
end

% storing sensor response in array for comparision:
sensorTimeArray(2) = y_out;

% comparision between last value and current value (the case of decay):
if sensorTimeArray(2)> sensorTimeArray(2-1)
    tau = 40.7*0.15;
end

% comparision between last value and current value (the case of rise):
if sensorTimeArray(2)<= sensorTimeArray(2-1)
    tau = 40.7*0.04;
end

alpha = 1/(tau+1);

disp(alpha)

% filtered response:
filteredResponse = (alpha*y_out)+(1-alpha)*filteredResponse;

end