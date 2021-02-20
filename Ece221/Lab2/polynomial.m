function [y] = polynomial(x,vec)
vecsize= size(vec);
if vecsize(1) == 1
goal = vecsize(2);
else
goal = vecsize(1);
end
y=0;
z=0;
for i=1:goal
    y = y + (vec(i))*x.^(i-1);
    z = (vec(i))*x.^(i-1);
    plot (x,z);
   legendInfo{i} = ['Term' num2str(i)]
 hold all;
end
plot (x,y);
xlabel('x');
ylabel('y');
legend(legendInfo, 'y(x)');


