function [Etot,Ex,Ey,Ez]=ringofcharge(a,p,x,y,z,N)
epsilon=8.854e-12;
dphi=2*pi/N; % Discretize the total line length of 2h into N pieces
phiprime = linspace (0, 2*pi-dphi,N); % The linspace command creates a vector that ranges

for k=1:size(phiprime)
 % Evaluate the expression which is the same for each component of E

 % Evaluate the differential elements for each component of E, which
 % arise from that small part of the line, i.e., dQ at zprime(k).
 dEx(k)=dphi * ((x-a*cos(phiprime(k)))/((((x-a*cos(phiprime(k))).^2) + ((y-a*sin(phiprime(k))).^2)  + z.^2).^(3/2)));
 dEy(k)=dphi * ((y-a*sin(phiprime(k)))/((((x-a*cos(phiprime(k))).^2) + ((y-a*sin(phiprime(k))).^2)  + z.^2).^(3/2)));
 dEz(k)=dphi * (1/((((x-a*cos(phiprime(k))).^2) + ((y-a*sin(phiprime(k))).^2)  + z.^2).^(3/2)));
end
% Do the "integration" by summing up the differential pieces that result from
% each value of zprime.
Ex=((p*a)/(4*pi*epsilon))*sum(dEx);
Ey=((p*a)/(4*pi*epsilon))*sum(dEy);
Ez=(p*a*z/(4*pi*epsilon))*sum(dEz);
Etot=(Ex^2+Ey^2+Ez^2)^0.5;
