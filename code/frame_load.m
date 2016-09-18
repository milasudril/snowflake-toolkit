function [data,N_rows]=frame_load(basedir,prefix,frame)
	filename=sprintf('%s/%s-%d.txt'...
		,basedir,prefix,frame);
	try
		[data,N_rows]=csvread2(filename,'\t');
	catch err
		disp(sprintf('Error loading %s: %s',filename,err.message));
		data=struct();
		N_rows=0;
	end
end
