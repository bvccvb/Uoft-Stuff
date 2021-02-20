function [V,Ex,Ey,C,We,We2,gridpointsx,gridpointsy,innerx,innery,outerx,outery] = bvprectangularcoax(a,b,c,d,xo,yo,er,Vo);
%
% This function used the finite difference method to solve the
% two-dimensional electrostatic boundary value problem related to a square
% coaxial cable.
% a = width of outer conductor
% b = height of outer conductor
% c = width of inner conductor
% d = height of inner conductor
% xo = the x-coordinate of the location of the bottom left corner of the inner conductor
% yo = the y-coordinate of the location of the bottom left corner of the inner conductor
% er = the relative permittivity of the dielectric which fills the space
% between the inner and outer conductor
% Vo = electric potential of the inner conductor (outer is grounded)
% Define the fundamental constant eo
eo=8.854e-12;
% Set number of nodes and node spacings
Nx=201;
hx=a/(Nx-1);
hy=hx;
Ny=round(b/hy+1);
% Set the initial values of V to zero
V = zeros(Nx,Ny);
% Set the known potential values (or boundary values)
V(1,1:Ny)=0; % Grounded left side
V(1:Nx,1)=0; % Grounded bottom side
V(Nx,1:Ny)=0; % Grounded right side
V(1:Nx,Ny)=0; % Grounded top side
innerstartx=round(xo/hx+1);
innerendx=round(innerstartx+c/hx);
innerstarty=round(yo/hy+1);
innerendy=round(innerstarty+d/hy);
V(innerstartx:innerendx,innerstarty:innerendy)=Vo; % Set potentials of inner conductor
% Determine the final voltage distributions (your code goes here???)

maxdev = 1e-4;
while(maxdev >= 1e-4)
    Vold = V(2:Nx-1,2:Ny-1);
    for i = 2:(Nx-1)
        for j = 2:(Ny-1)
            if (i < innerstartx || i > innerendx || j < innerstarty || j > innerendy)
                V(i, j) = (1/4)*(V(i+1, j) + V(i-1, j) + V(i, j+1) + V(i, j-1));
            end
        end
    end
    Vnew=V(2:Nx-1,2:Ny-1);
    maxdev=max(max(abs(100*(Vnew-Vold)./(Vnew))));
end

% gridpointsx is a matrix (size Nx x Ny) that contains the x-values of the
% locations of the nodes within the grid.
% gridpointsy is a matrix (size Nx x Ny) that contains the y-values of the
% locations of the nodes within the grid.
[gridpointsx,gridpointsy]=meshgrid(0:hx:a,0:hy:b);
% innerx and innery are matrices that contains the x- and y-values of the
% locations of the nodes that relate to the inner conductor.
[innerx,innery]=meshgrid((innerstartx-1)*hx:hx:(innerendx-1)*hx,(innerstarty-1)*hy:hy:(innerendy - 1)*hy);
% outerx and outery are matrices that contains the x- and y-values of the
% locations of the nodes that relate to the outer conductor.
outerx=[0:hx:a,zeros(1,Ny-2),a:-hx:0,zeros(1,Ny-2)];
outerx((Nx+1):(Nx+Ny-2))=a;
outery=[zeros(1,Nx),hy:hy:(b-hy),zeros(1,Nx),(b-hy):-hy:hy];
outery((Nx+Ny-1):(2*Nx+Ny-2))=b;
figure
plot(gridpointsx,gridpointsy,'b*');hold;
plot(outerx,outery,'kd');
plot(innerx,innery,'ro');

Qinner = 0;
  for x = innerstartx : innerendx
      for y = innerstarty : innerendy
          if (x == innerstartx && y == innerstarty) 
              Qinner = Qinner + abs(V(x, y) - V(x-1, y-1)) * eo * er;
          elseif (x == innerstartx && y == innerendy) 
              Qinner = Qinner + abs(V(x, y) - V(x-1, y+1)) * eo * er;
          elseif (x == innerendx && y == innerstarty) 
              Qinner = Qinner + abs(V(x, y) - V(x+1, y-1)) * eo * er;          
          elseif (x == innerendx && y == innerendy) 
              Qinner = Qinner + abs(V(x, y) - V(x+1, y+1)) * eo * er;
          elseif (x == innerstartx)
             Qinner = Qinner + abs(V(x, y) - V(x-1, y)) * eo * er;
          elseif(y == innerstarty)
             Qinner = Qinner + abs(V(x, y) - V(x, y-1)) * eo * er; 
          elseif (x == innerendx)
             Qinner = Qinner + abs(V(x, y) - V(x+1, y)) * eo * er;
          elseif(y == innerendy)
             Qinner = Qinner + abs(V(x, y) - V(x, y+1)) * eo * er; 
          end
      end
  end

  C = Qinner / Vo
  
[Ey,Ex]=gradient(-V,hx,hy);
figure;
meshc(gridpointsx,gridpointsy,V');
figure;
contourf(gridpointsx, gridpointsy, V');
figure;
quiver(gridpointsx,gridpointsy,Ex',Ey');