function environment = LoadEnviornment

fileName = 'E:\environments\original.env';


fidin=fopen(fileName);

if fidin>0
    disp(fileName);
    
    % create a temporary .txt file to keep the read data
    fidout=fopen('D:\rawData\environmentMatrix.txt','w');
    
    disp('writing')
    while ~feof(fidin) % judge if the current line is in the end of the file
        tline=fgetl(fidin); % read the line from the file
        if double(tline(1))>=48 && double(tline(1))<=57 % judge if the first element of the current line is a number
            fprintf(fidout,'%s\n\n',tline); % if the current line is of numbers, then write this line into the temporary .txt file.
            continue % otherwise read the next line
        else
            fclose(fidout);
            break
        end
    end
    
    try 
        fclose(fidout);
    catch
        disp('environment has multiple layers.')
    end

    environment=importdata('D:\rawData\environmentMatrix.txt'); % import the temporary .txt file into the workspace
end