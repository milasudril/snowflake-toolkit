function [data,N_rows]=aggregates_frame_load_last(basedir)
% function [data,N_rows]=aggregates_frame_load_last(basedir)
%
% Loads statistics from the last frame
%
% IN
%	basedir	the directory for the frame files
%
% OUT
%	data	A struct containing 1 x N_rows matrices with the contents of each column
%	N_rows	The number of rows in the frame file 
%
	frame_data=aggregates_framedata_load(basedir);
	[data,N_rows]=aggregates_frame_load(basedir,frame_data.Frame(end));
end
