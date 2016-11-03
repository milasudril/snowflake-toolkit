function [data,N_rows]=aggregate_frame_load(basedir,frame_number)
% Loads statistics about a particular frame. Valid frame numbers can be 
% retrieved by loading the file `frame_data.txt` from basedir.
%
	filename=sprintf('%s/frame-%d.txt'...
		,basedir,prefix,frame_number);
	try
		[data,N_rows]=csvread2(filename,'\t');
	catch err
		disp(sprintf('Error loading %s: %s',filename,err.message));
		data=struct();
		N_rows=0;
	end
end
