function [ Ex, Ey, Ez, Etot, V ] = sphere_of_charge(a, ps, x, y, z, N);
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

e0 = 8.854187817E-12;
k = 1/(4*pi*e0);

dphi = 2*pi/N;
dtheta = pi/N;

rphi = linspace(0,2*pi-dphi,N);
rtheta = linspace(0,pi,N);


for i = 1 : length(rphi)
    for j = 1 : length(rtheta)
        integrand = (sin(rtheta(j))*dtheta*dphi)/((x-a*sin(rtheta(j))*cos(rphi(i)))^2 + (y-a*sin(rtheta(j))*sin(rphi(i)))^2 + (z-a*cos(rtheta(j)))^2)^(3/2);
        Ex_integ1(j) = (x-a*sin(rtheta(j))*cos(rphi(i)))*integrand;
        Ey_integ1(j) = (y-a*sin(rtheta(j))*sin(rphi(i)))*integrand;
        Ez_integ1(j) = (z-a*cos(rtheta(j)))*integrand;
        V1(j) = (sin(rtheta(i))*dtheta*dphi)/((x-a*sin(rtheta(j))*cos(rphi(i)))^2 + (y-a*sin(rtheta(j))*sin(rphi(i)))^2 + (z-a*cos(rtheta(j)))^2)^(1/2);
    end
    Ex_integ2(i) = sum(Ex_integ1);
    Ey_integ2(i) = sum(Ey_integ1);
    Ez_integ2(i) = sum(Ez_integ1);
    V2(i) = sum(V1);
end

Ex = ps*a^2*k*sum(Ex_integ2);
Ey = ps*a^2*k*sum(Ey_integ2);
Ez = ps*a^2*k*sum(Ez_integ2);
if(ps > 0)
    Etot = sqrt(Ex^2 + Ey^2 + Ez^2);
else
    Etot = -sqrt(Ex^2 + Ey^2 + Ez^2);
end
V = ps*a^2*k*sum(V2);






