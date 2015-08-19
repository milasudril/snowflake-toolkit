function frame_data=frame_data_load(basedir)
	filename=sprintf('%s/frame_data.txt',basedir);
	try
		frame_data=dlmread(filename,'\t',1,0);
	catch err
		disp(sprintf('Error loading %s',filename));
		frame_data=[];
	end
end
