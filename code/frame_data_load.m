function [frame_data,N_rows]=frame_data_load(basedir)
	filename=sprintf('%s/frame_data.txt',basedir);
	try
		[frame_data,N_rows]=csvread2(filename,'\t');
	catch err
<<<<<<< HEAD
		disp(sprintf('Error loading %s: %s',filename,err.message));
=======
		disp(sprintf('Error loading %s',filename));
>>>>>>> master
		frame_data=struct();
		N_rows=0;
	end
end
