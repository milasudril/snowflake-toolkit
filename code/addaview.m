function addaview(points,exedir)
% addaview(points)
%
% This function passes points to addaview for visualization. This program
% may have better performance than a regular plot3 command, since it uses
% bare-bone OpenGL.

	cmd='';
	if nargin()<2
		cmd='addaview';
	else
		cmd=[exedir,'/addaview'];
	end
	pipe_write({cmd},@(filename)(dlmwrite(filename,points,' ')));
end
