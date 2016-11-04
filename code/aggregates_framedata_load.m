function [frame_data,N_rows]=frame_data_load(basedir)
% function [frame_data,N_rows]=frame_data_load(basedir)
%
% Loads information about frame data generated during aggregate simulation
	filename=sprintf('%s/frame_data.txt',basedir);
	try
		[frame_data,N_rows]=csvread2(filename,'\t');
	catch err
		disp(sprintf('Error loading %s: %s',filename,err.message));
		frame_data=struct();
		N_rows=0;
	end
end
