function [img,res]=adda_shadow(points,alpha,beta,gamma,exepath,exename)
% function [img,res]=adda_shadow(points,alpha,beta,gamma,exepath,exename)
%
% ADDA file shadow renderer
%
% This function renders the shadow of the point, projected using the Euler
% angles alpha, beta, and gamma. The renderer requires a graphical terminal,
% since it first creates a window, and then uses OpenGL to render the shadow.
%
% IN
%	points	A N x 3 matrix with the points to render
%	alpha	The first euler angle
%	beta	The second euler angle
%	gamma 	The third euler angle
%	exepath	The directory containeing the raseterizer executable
%	exename The name of the rasterizer executable
%
% OUT
%	img	A matrix containing the shadow map. Use imshow, image, or imagesc 
%		to show the result.
%	res	The number of pixels per length unit
%	

	alpha=['--alpha=',num2str(alpha)];
	beta=['--beta=',num2str(beta)];
	gamma=['--gamma=',num2str(gamma)];

	n=nargin();
	cmd='';
	switch nargin()
		case 1
			cmd='adda_shadow';
		case 2
			cmd='adda_shadow';
		case 3
			cmd='adda_shadow';
		case 4
			cmd='adda_shadow';
		case 5
			cmd=[exepath,'/adda_shadow'];
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

	rng=fopen('/dev/urandom','r');
	x=fread(rng,1,'uint32');
	y=fread(rng,3,'uint16');
	z=fread(rng,3,'uint16');
	fclose(rng);
	name_cookie=sprintf('/tmp/%08x-%04x-%04x-%04x-%04x%04x%04x',x,y(1),y(2),y(3),z(1),z(2),z(3));
	tmp=fopen(name_cookie,'w');
	fclose(tmp);
	cookie_param=['--cookie=',name_cookie];
	pipe_write({cmd,alpha,beta,gamma,img_param,cookie_param}...
		,@(filename)(dlmwrite(filename,points,' ')));
	while exist(name_cookie,'file')
	end
	info=imfinfo(name);
	img=imread(name);
	res=[info.XResolution,info.YResolution]/1024;
	delete(name);
end
