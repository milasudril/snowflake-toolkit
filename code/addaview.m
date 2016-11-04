function addaview(points,exedir)
% addaview(points)
%
% This function passes points to addaview for visualization. This program
% may have better performance than a regular plot3 command, since it uses
% bare-bone OpenGL.

	file=mkfifo();
	cmd='';
	if nargin()<2
		cmd='addaview';
	else
		cmd=[exedir,'/addaview'];
	end
	file_param=['--file=',file];
	system_wrapper({cmd,file_param},0);
	dlmwrite(file,points,' ');
	delete(file);
end