function [integ] = integraler(mi, ma)
delta = 0.000001;
x = [mi : delta : ma];
y= x.^2 ;
index = (ma-mi)/delta;
integ = 0;
for i=1:index
    area_of_box = y(i) * delta;
    integ = integ + area_of_box;
end
