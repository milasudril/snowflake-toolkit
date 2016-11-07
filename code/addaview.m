function addaview(points,exedir)
% addaview(points,exedir)'
%
% This function passes points to addaview for visualization. This program
% may have better performance than a regular plot3 command, since it uses
% bare-bone OpenGL.
%
% IN
%	points	A N x 3 matrix of vertices, in order x, y, and z.
%	exedir	Directory containeing the excutable
%
	cmd='';
	if nargin()<2
		cmd='addaview';
	else
		cmd=[exedir,'/addaview'];
	end
	pipe_write({cmd},@(filename)(dlmwrite(filename,points,' ')));
end
