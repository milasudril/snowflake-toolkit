function [data,N_rows]=aggregates_frame_load(basedir,frame_number)
% function [data,N_rows]=aggregates_frame_load(basedir,frame_number)
%
% Loads statistics from a particular frame
%
% This function loads statistics from a particular frame. Valid frame numbers can be 
% retrieved by loading the file `frame_data.txt` from basedir.
%
% IN
%	basedir	the directory for the frame files
%	frame_number	The frame number to load. Valid frame numbers can be 
%		retrieved by loading the file `frame_data.txt` from basedir.
%
% OUT
%	data	A struct containing 1 x N_rows matrices with the contents of each column
%	N_rows	The number of rows in the frame file 
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
