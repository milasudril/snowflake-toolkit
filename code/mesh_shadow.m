function img=mesh_shadow(ice_file,alpha,beta,gamma,exepath,exename)
% function img=mesh_shadow(ice_file,alpha,beta,gamma,exepath,exename)
%
% Mesh shadow renderer
%
% This function renders the shadow of ice_file, projected using the Euler
% angles alpha, beta, and gamma. The renderer requires a graphical terminal,
% since it first creates a window, and then uses OpenGL to render the shadow.
%
% IN
%	ice_file	File to project
%	alpha	The first euler angle
%	beta	The second euler angle
%	gamma 	The third euler angle
%	exepath	The directory containeing the raseterizer executable
%	exename The name of the rasterizer executable
%
% OUT
%	A matrix containing the shadow map. Use imshow, image, or imagesc 
%	to show the result.
%

	prototype=['--prototype=',ice_file];
	alpha=['--alpha=',num2str(alpha)];
	beta=['--beta=',num2str(beta)];
	gamma=['--gamma=',num2str(gamma)];

	n=nargin();
	cmd='';
	switch nargin()
		case 1
			cmd='mesh_shadow';
		case 2
			cmd='mesh_shadow';
		case 3
			cmd='mesh_shadow';
		case 4
			cmd='mesh_shadow';
		case 5
			cmd=[exepath,'/mesh_shadow'];
		otherwise
			cmd=[exepath,'/',exefile];
	end

	rng=fopen('/dev/urandom','r');
	x=fread(rng,1,'uint32');
	y=fread(rng,3,'uint16');
	z=fread(rng,3,'uint16');
	fclose(rng);
	name=sprintf('/tmp/%08x-%04x-%04x-%04x-%04x%04x%04x',x,y(1),y(2),y(3),z(1),z(2),z(3));
	img_param=['--image=',name];
	system_wrapper({cmd,prototype,alpha,beta,gamma,img_param},1);
	img=imread(name);
	delete(name);
end
