function [Ex, Ey, Ez, Etot]=ringofcharge(P, a, N)
epsilon = 8.854e-12;
delta = ((2*pi)/N);
x = P(1,1);
y = P(1,2);
z = P(1,3);
q = (( a )/(4*pi*epsilon));
theta = linspace(0, 2*pi-delta, N);

for k = 1:length(theta)
    dEx(k) = ((delta)*(x - a * cos(theta(k))))/(((x-a*cos(theta(k)))^2 + (y-a*sin(theta(k)))^2 + z^2)^(3/2));
    dEy(k) = ((delta)*(y - a * sin(theta(k))))/(((x-a*cos(theta(k)))^2 + (y-a*sin(theta(k)))^2 + z^2)^(3/2));
    dEz(k) = ((delta))/(((x-a*cos(theta(k)))^2 + (y-a*sin(theta(k)))^2 + z^2)^(3/2));
end    
a = (sin(theta)).*dEx;
b = (sin(theta)).*dEy;
c = (sin(theta)).*dEz;


Ex = q * sum(a) *1e-6;
Ey = q * sum(b) *1e-6;
Ez = q * z * sum(c) *1e-6;
Etot = sqrt(Ex^2 + Ey^2 + Ez^2);

