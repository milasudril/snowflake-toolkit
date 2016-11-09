function [frame_data,N_rows]=frame_data_load(basedir)
% function [frame_data,N_rows]=frame_data_load(basedir)
%
% Loads information about frame data generated during aggregate simulation
%
% IN
%	basedir	the directory for the frame files
%
% OUT
%	frame_data	A struct containing 1 x N_rows matrices with the contents of each column
%	N_rows	The number of rows
%
	filename=sprintf('%s/frame_data.txt',basedir);
	try
		[frame_data,N_rows]=csvread2(filename,'\t');
	catch err
		disp(sprintf('Error loading %s: %s',filename,err.message));
		frame_data=struct();
		N_rows=0;
	end
end
