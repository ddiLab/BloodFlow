clear
%% normalize the parameters
%----- normalized value ---------------------%
dx = 0.5e-6;%0.5e-6;%1e-6; % m
tau=1;%0.95;% best accuracy
vis=1e-3; %pa*s
rho=1e3;%kg/m^3
dt=(tau-0.5)/3*dx^2*rho/vis;

%-- palabos incompressible parameters--------%
u_phy = 5e-3; % m/s
u_max = u_phy/(dx/dt);% U_physical = 1
Re =  dx*u_phy/(vis/rho);
Nreso=1; % dx as 1, 
nu_lb = Nreso*u_max/Re;
fprintf('Re %e\n',Re);
fprintf('Nreso %d\n',Nreso);
fprintf('u_Max %e\n',u_max);
fprintf('dx %e\n',dx);
fprintf('dt %e\n',dt);

dm = rho*dx*dx*dx; % water mass
epsilon = dm*(dx/dt)^2;
kBT=1.3806e-23*296;
T = kBT/epsilon;
fprintf('scaled temperature %e\n\n',T);
% ---- cell size in dimensionless units -----%
r_p = 4e-6/dx;
xlo = 0; xhi = 62;
ylo = 0; yhi = 62;
zlo = 0; zhi = 62;


%% cell position
%how many layers of cells do you want in each direction?
nCellX = 4;
nCellY = 3;
nCellZ = 3;

%offset values to shift layers of cells
%be carefull not to push cells out of the simulation box
offsetY = 2;    
offsetZ = 2;

%spaces cell centers out throughout the domain
cnt_x = linspace(4,xhi-4,nCellX)'; %(4:6.5:(xhi-4))';
cnt_y = linspace(14,yhi-14,nCellY)'; %(10:17:(yhi-10))';
cnt_z = linspace(14,zhi-14,nCellZ)'; %(10:17:(zhi-10))';

%checks if generated cell membranes will overlap
if (cnt_x(2)-cnt_x(1)) < (4e-6/dx)
    error('Error. nCellX is too large, cells will overlap. Reduce nCellX or increase xhi.');
elseif cnt_y(2)-cnt_y(1) < (8.5e-6/dx)
    error('Error. nCellY is too large, cells will overlap. Reduce nCellY or increase yhi.');
elseif cnt_z(2)-cnt_z(1) < (8.5e-6/dx)
    error('Error. nCellZ is too large, cells will overlap. Reduce nCellZ or increase zhi.');
end

%generate the grid layout of all the cells in the domain
cnt_x_rep = repmat(cnt_x,nCellY*nCellZ,1);
cnt_y_rep = repmat(cnt_y,nCellX*nCellZ,1);
cnt_z_rep = repmat(sort(repmat(cnt_z,nCellY,1)),nCellX,1);

cnt = [sort(cnt_x_rep) cnt_y_rep cnt_z_rep];

%stagger layers of cells
for i=1:nCellX
    offset = offsetY*(-1)^i;
    cnt(cnt(:,1)==cnt_x(i),2) = cnt(cnt(:,1)==cnt_x(i),2) + offset;
end
for i=1:nCellZ
    offset = offsetZ*(-1)^i;
    cnt(cnt(:,3)==cnt_z(i),2) = cnt(cnt(:,3)==cnt_z(i),2) + offset;
end

%calculate the hematocrit in the domain
V_box = xhi*yhi*zhi*dx^3;
V_cell = length(cnt)*9e-17;
Ht = V_cell/V_box;
fprintf('Hematocrit in box: %f\n\n',Ht);


% [p,t]=Init3D(1,0.12); % works for uniformmesh
[p,t]=Init3D_div(1,0.25); % works for uniformmesh
% 0.1: 10485762  ;0.18:40962;  0.2: 10242; 0.25: 2562; 0.5:162;
% p=RBC_x(p,2);
p=RBC_x(p,2);
p=p*r_p;

s_numb=size(cnt,1); % cells number

n0=length(p);
X = repmat(p,s_numb,1);
for i=1:s_numb
    for j=1:3
        X((i-1)*n0+1:i*n0,j)= X((i-1)*n0+1:i*n0,j)+cnt(i,j);
    end
end

tt=repmat(t,s_numb,1);
tsz=length(t);
for i=1:s_numb
    tt((i-1)*tsz+1:i*tsz,:)= tt((i-1)*tsz+1:i*tsz,:)+(i-1)*n0;
end

[bond_list0, dihedral_list0, faceShareEdge0]=bondDihedralList(p,t);
sz=length(bond_list0);
bond_list=repmat(bond_list0,s_numb,1);
dihedral_list=repmat(dihedral_list0,s_numb,1);
%faceShareEdge=repmat(faceShareEdge0,s_numb,1);
for i=1:s_numb
    bond_list((i-1)*sz+1:i*sz,:)=bond_list((i-1)*sz+1:i*sz,:)+n0*(i-1);
    dihedral_list((i-1)*sz+1:i*sz,:)=dihedral_list((i-1)*sz+1:i*sz,:)+n0*(i-1);
    %faceShareEdge((i-1)*sz+1:i*sz,:)=faceShareEdge((i-1)*sz+1:i*sz,:)+tsz*(i-1);
end

solid.type=1;% cell
solid.numb=s_numb;
solid.X=X;
solid.t=tt;
solid.bond_list=bond_list;
solid.dihedral_list=dihedral_list;
% only single cell face and dihedrals defined as 
% angle type and dihedral types
[cnt,n,A,VA,L,c_s,n1,n2,t1,t2]=geoCalculating(solid.X,solid.t,solid);


%----- for wlc coarse graining from rbc -----%
pL0=18.7e-9;   % persistence length
kBT=1.3806e-23*296;
ref_nn=27344; % standard one
pL=pL0*sqrt((n0-2)/(ref_nn-2)); % scale from standard one
x0=0.5;%0.4545;
Kwlc=kBT/pL*dt^2/dm/dx; % scale kwlc to lb units
KrpW=Kwlc*(0.25/(1-x0)^2-0.25+x0)*(L.^2); % for wlc model
Krpw_0=Kwlc*(0.25/(1-x0)^2-0.25+x0)*(mean(L)^2);% average Krpw_0
%nu=sqrt(3)*kBT/4/pL/(mean(L)*dx)*(3/(4*(1-x0)^2)-3/4+4*x0+x0/(2*(1-x0)^3));% wlc-c
nu=sqrt(3)*kBT/4/pL/(mean(L)*dx)*(x0/(2*(1-x0)^3)-0.25/(1-x0)^2+0.25)+sqrt(3)*Krpw_0*3/4/mean(L)^3*dm/dt^2;% wlc-pow

%rotating the cells
% % % for i=1:s_numb
% % %     a0=2*pi*rand;b0=2*pi*rand;c0=2*pi*rand;
% % %              Rotx=[1 0 0;
% % %                  0  cos(a0) -sin(a0);
% % %                  0  sin(a0)  cos(a0)];      
% % %              Roty=[cos(b0) 0 sin(b0);
% % %                    0     1     0;
% % %                  -sin(b0) 0 cos(b0)]; 
% % %              Rotz=[cos(c0) -sin(c0) 0;
% % %                  sin(c0) cos(c0) 0;
% % %                  0  0   1];
% % %         X((i-1)*n0+1:i*n0,:)=(X((i-1)*n0+1:i*n0,:)-repmat(cnt_c(i,:),n0,1))*Rotx*Roty*Rotz;
% % %         X((i-1)*n0+1:i*n0,:)= X((i-1)*n0+1:i*n0,:)+repmat(cnt_c(i,:),n0,1);
% % % end

nu0=5.3e-6; % N/m
fprintf('shear modulus %e, nu0=%e\n',nu,nu0);
%----- bending ------------------------------%
kb =2.3e-19;% 3e-19*2/sqrt(3);%2/sqrt(3)*70kBT %ref. fedosov: microvascular blood flow resistance, role of rbc migration
%kb =200*5e-19;% works for shear rate 10000/s, [20,100]*5e-19 did not work
kb = kb*dt^2/dm/dx^2;
%----- Area/Volume --------------------------%
%ka =0; %N/m parameters for rst_nice in vtk_cell, kb=200*5e-19
%kd =2.1e-3; % N/m
%kv =0; % N/m^2
ka =2.1e-4; %N/m
kd =2.1e-4; % N/m
kv =2.20; % N/m^2
ka = ka*dt^2/dm;
kd = kd*dt^2/dm;
kv = kv*dt^2*dx/dm;

% write output
name='in.one_cell'
%name=strcat(name,'.vtk');
fid = fopen(name,'w+');
fprintf(fid,'lammps cell input\n');
n=size(X,1);%+size(wall_particles,1);
fprintf(fid,'%d atoms\n', n);
nb=size(bond_list,1);%+size(bond_list_w,1);
fprintf(fid,'%d bonds\n', nb);
nf=size(tt,1);%+size(angle_list_w,1);
fprintf(fid,'%d angles\n', nf);
nd=length(dihedral_list);
fprintf(fid,'%d dihedrals\n\n', nd);

nat=2;
fprintf(fid,'%d atom types\n', nat);
nbt=length(bond_list0);%+size(bond_list0_w,1);
fprintf(fid,'%d bond types\n', nbt);
nangt=length(t);%+size(angle_list0_w,1);
fprintf(fid,'%d angle types\n', nangt);
ndt=length(dihedral_list0);
fprintf(fid,'%d dihedral types\n', ndt);

fprintf(fid,'%g %g xlo xhi\n',xlo,xhi);
fprintf(fid,'%g %g ylo yhi\n',ylo,yhi);
fprintf(fid,'%g %g zlo zhi\n\n',zlo,zhi);

fprintf(fid,'Masses\n\n');
%m=rho*4/3*pi*(3.75e-6)^3/dm/n0;%assume to be water, platelet size
m=90e-18*rho/dm/n0;%assume to be water, vol=90 fL
%m=rho*4/3*pi*xa*xb*xc*dx^3/dm/n0;%assume to be water, platelet size
fprintf(fid,'%d %g\n',1,m);
fprintf(fid,'%d %g\n',2,m);
fprintf(fid,'\n');

fprintf(fid,'Bond Coeffs\n\n');

nb0=length(bond_list0);
for i=1:nb0
    fprintf(fid,'%d %g %g %g %g\n ', i,Kwlc,L(i),x0,KrpW(i));
end
% nb1=length(bond_list0_w);
% for i=1:nb1
%     fprintf(fid,'%d %g %g %g %g\n ', i+nb0,Kwlc_w,L_w,x0_2,KrpW_w);
% end
fprintf(fid,'\n');

fprintf(fid,'Angle Coeffs\n\n');
cq=0;
q=0;
A0t=sum(A)/s_numb;
V0t=sum(VA)/s_numb;
nf0=length(t);
for i=1:nf0 % only one cell surface
    % type, cq, q ka a0t kv v0t kd a
    %fprintf(fid,'%d %g %g %g %g %g %g %g %g\n', i, cq, q,ka, A0t, kv, V0t, kd, A(i));
    fprintf(fid,'%d %g %g %g %g %g %g %g %g\n', i, cq, q,ka, A0t, kv, V0t, kd, A(i));
end
% nf0_w=size(angle_list0_w,1);
% kB_w = 10000000*kBT/dm/dx^2*dt^2;
% for i=1:nf0_w
%   fprintf(fid,'%d harmonic %g %g\n', i+nf0,kB_w,180);
% end
fprintf(fid,'\n');

fprintf(fid,'Dihedral Coeffs\n\n');
di_angle=zeros(ndt,1);
for i=1:ndt
%% different equilbirum angle
    if c_s(i,1)<0 && c_s(i,2) >0
        di_angle(i) = round((acos(c_s(i,1)) + pi)/pi*180);
    elseif c_s(i,1)<0 && c_s(i,2) <0
        di_angle(i) = round((pi - acos(c_s(i,1)))/pi*180);
    elseif c_s(i,1)>0 && c_s(i,2) >0
        di_angle(i) = round((acos(c_s(i,1)) + pi )/pi*180);
    elseif c_s(i,1)>0 && c_s(i,2) <0
        di_angle(i) = round((pi - acos(c_s(i,1)))/pi*180);
    elseif c_s(i,1)==0 && c_s(i,2)>0
        di_angle(i) = 270;
    elseif c_s(i,1)==0 && c_s(i,2)<0 
        di_angle(i) = 90;
    end
%-------one angle for all------------%
%  di_angle(i)=180+round(acos((sqrt(3)*(n0-2)-5*pi)/(sqrt(3)*(n0-2)-3*pi))/pi*180);
end
for i=1:ndt
    %fprintf(fid,'%d %g %d %d\n',i,K,d,n);
    fprintf(fid,'%d %g %d\n',i,kb,di_angle(i));
end
fprintf(fid,'\n');

fprintf(fid,'Atoms\n\n');
% molecular type: aID, mID, aTy, x, y, z
n0=length(p);

% for i=1:length(id)
%     fprintf(fid,'%d %d %g %g %g\n',id(i),3,xs(i),ys(i),zs(i));
% end

for i=1:s_numb
    for j=1:n0
        k=(i-1)*n0+j;
         fprintf(fid,'%d %d %d %g %g %g\n',k,i,1,X(k,1),X(k,2),X(k,3));
%          fprintf(fid,'%d %d %d %g %g %g\n',k,i,1,X(k,1)+1,X(k,2)+1,X(k,3)+1); % shift for mesh input
    end   
end

% for i=1:length(wall_particles)
%     fprintf(fid,'%d %d %d %g %g %g\n',i+k,s_numb+1,2,wall_particles(i,1),wall_particles(i,2),wall_particles(i,3));
% end
% n0_w=size(p2,1);
% nn_cell=size(X,1);
% for i=1:s_numb_2
%   for j=1:n0_w
%     k=(i-1)*n0_w+j;
%     fprintf(fid,'%d %d %d %g %g %g\n',k+nn_cell,i+s_numb,2,X2(k,1),X2(k,2),X2(k,3));
%   end
% end
fprintf(fid,'\n');

fprintf(fid,'Bonds\n\n');
nb0=length(bond_list0);
for i=1:s_numb
    for j=1:nb0
        k=(i-1)*nb0+j;
        fprintf(fid,'%d %d %d %d\n',k,j,bond_list(k,1),bond_list(k,2));
    end
end
% nb_cell=size(bond_list,1);
% nb0_w=length(bond_list0_w);
% for i=1:s_numb_2
%     for j=1:nb0_w
%         k=(i-1)*nb0_w+j;
%         fprintf(fid,'%d %d %d %d\n',k+nb_cell,j+nb0,bond_list_w(k,1)+nn_cell,bond_list_w(k,2)+nn_cell);
%     end
% end
fprintf(fid,'\n');

fprintf(fid,'Angles\n\n');
% angleID, aType, atom1,2,3
for i=1:s_numb
    for j=1:nf0
        k=(i-1)*nf0+j;
        fprintf(fid,'%d %d %d %d %d\n',k,j,tt(k,1),tt(k,2),tt(k,3));
    end
end
% nf_cell=size(tt,1);
% for i=1:s_numb_2
%     for j=1:nf0_w
%         k=(i-1)*nf0_w+j;
%         fprintf(fid,'%d %d %d %d %d\n',k+nf_cell,j+nf0,angle_list_w(k,1)+nn_cell,angle_list_w(k,2)+nn_cell,angle_list_w(k,3)+nn_cell);
%     end
% end
fprintf(fid,'\n');

fprintf(fid,'Dihedrals\n\n');
% dID, dType, atom1,2,3,4 
for i=1:s_numb
    for j=1:ndt
        k=(i-1)*ndt+j;
        fprintf(fid,'%d %d %d %d %d %d\n',k,j,dihedral_list(k,1),...
            dihedral_list(k,2),dihedral_list(k,3),dihedral_list(k,4));
    end
end
fprintf(fid,'\n');

fclose(fid);

