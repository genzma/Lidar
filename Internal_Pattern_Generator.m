zGes = 684;
sGes = 608;

cropLR = 50;
cropTD = 200;

zPrt = floor((zGes-2*cropTD)/8);
sPrt = floor((sGes-2*cropLR)/12);
    
A = zeros(zGes,sGes); 

counter = 1;
imgNr = 1;

for zMv = 0:7
    for sMv = 0:11
        
        
        A(cropTD+zMv*zPrt+1:cropTD+(zMv+1)*zPrt,cropLR+sMv*sPrt+1:cropLR+(sMv+1)*sPrt) = 255;
            
        A = A/255;
        
        if imgNr < 10 
            imwrite(A, ['0' int2str(imgNr) '.bmp']);
        else
            imwrite(A, [int2str(imgNr) '.bmp']);
        end
            
        imgNr = imgNr + 1;
        A = zeros(zGes,sGes);
        
        counter = counter + 1;
        
    end
end

