function addaview(points,exedir)
% addaview(points)
%
% This function passes points to addaview for visualization. This program
% may have better performance than a regualr plot3 command, since it uses
% bare-bone OpenGL.

	file=mkfifo();
	n=nargin();
	cmd=ternary(@()n<2,@()'addaview',@()[exedir,'/addaview']);
	file_param=['--file=',file];
	system_wrapper({cmd,file_param},0);
	dlmwrite(file,points,' ');
	delete(file);
end