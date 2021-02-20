function [Ex,Ey,Ez, Etot]=ringofcharge(a,p,x,y,z,N)
epsilon=8.854e-12;
dphi=2*pi/N; % Discretize the total line length of 2h into N pieces
phiprime = linspace (-pi, pi,N); % The linspace command creates a vector that ranges

for k=1:length(phiprime)
 integrand = dphi/(((x-a*cos(phiprime(k)))^2) + ((y-a*sin(phiprime(k)))^2)  + z^2)^(3/2);
 dEx(k)=  (x-a*cos(phiprime(k)))*integrand;
 dEy(k)= (y-a*sin(phiprime(k)))*integrand;
 dEz(k)= integrand;
 phi(k)= 3*sin(phiprime(k));
end

Ex=((sum(p)*a)/(4*pi*epsilon))*sum(dEx);
Ey=((sum(p)*a)/(4*pi*epsilon))*sum(dEy);
Ez=(sum(p)*a*z/(4*pi*epsilon))*sum(dEz);
Etot=sqrt(Ex^2 + Ey^2 +Ez^2);

%3sin(phi)