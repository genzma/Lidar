zGes = 684;
sGes = 608;


tl = 12;

zPrt = round(zGes/tl);
sPrt = round(sGes/tl);

rot = 1;
gruen = 2;
blau = 3;

S = [1 0 0; 1 0 0; 4 0 0; 8 0 0; 16 0 0; 32 0 0; 64 0 0; 128 0 0;
     0 1 0; 0 2 0; 0 4 0; 0 8 0; 0 16 0; 0 32 0; 0 64 0; 0 128 0;
     0 0 1; 0 0 2; 0 0 4; 0 0 8; 0 0 16; 0 0 32; 0 0 64; 0 0 128];

A = zeros(zGes,sGes,3);

imgNr = 1;

wdh = 1;

for wdh = 1:20
    
    counter = 1;
    
    rgb = [3 1 2];
    
    for zMv = 0:tl-1
        for sMv = 0:tl-1
            
            for j = 1:3
                A(zMv*zPrt+1:(zMv+1)*zPrt+1,sMv*sPrt+1:(sMv+1)*sPrt+1,rgb(j)) = S(counter,j);
            end
            
            
            
            if counter == 24
                counter = 0;
                
                A = A/255;
                if imgNr < 10
                    imwrite(A, ['frame_000' int2str(imgNr) '.bmp']);
                else if imgNr < 100
                        imwrite(A, ['frame_00' int2str(imgNr) '.bmp']);
                    else if imgNr < 1000
                            imwrite(A, ['frame_0' int2str(imgNr) '.bmp']);
                        else
                            imwrite(A, ['frame_' int2str(imgNr) '.bmp']);
                        end
                    end
                end
                
                imgNr = imgNr + 1;
                A = zeros(zGes,sGes,3);
            end
            
            counter = counter + 1;
            
        end
    end
    
end

