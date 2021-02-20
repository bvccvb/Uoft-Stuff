function [Ex, Ey, Ez, Etot]=plotringofcharge(radius, N)
x = 0;
y = 0;
epsilon = 8.854e-12;
z = linspace(-3, 3, 300);
a = radius;

for q = 1:length(z)
    [Ex(q), Ey(q), Ez(q), Etot(q)] = ringofcharge([0 0 z(q)],radius, N);
end

figure;
hold all;
plot(z, Ez, 'r');
%plot(z, Ex, 'b');
plot(z, Ey, 'g');
hold all;
%plot(z, k, 'b');


