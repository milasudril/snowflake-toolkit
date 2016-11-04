function [data,N_rows]=aggregates_frame_load_last(basedir)
% Loads statistics from the last frame.
%	
	frame_data=aggregates_framedata_load(basedir);
	[data,N_rows]=aggregates_frame_load(basedir,frame_data.Frames(end));
end
