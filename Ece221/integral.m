function [integ] = integral(y, max,min)
delta = 0.00001;
x = [min, delta, max];
index = (max-min)/delta;
integ = 0;
for i=1:index
    area_of_box = y * delta;
    integ = integ + area_of_box;
end
